#include "app.h"

#include <chrono>
#include <thread>
#include <algorithm>
#include <numeric>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

App::App(
    Settings settings,
    std::unique_ptr<FileManagerInterface> fileManager,
    std::unique_ptr<P2PNetworkInterface> networkManager) :
    settings(settings),
    fileManager(std::move(fileManager)),
    networkManager(std::move(networkManager)) {

    this->networkManager->setPorts(
        settings.ms_port,
        settings.server_port,
        settings.file_mgr_port);
}

void App::run() {
    running = true;
    networkManager->start(settings.main_server);

    fileMgrThread = new std::thread(&FileManagerInterface::run, &*fileManager);

    appThread = new std::thread(&App::listener, this);
}

void App::stop() {
    running = false;

    fileMgrThread->join();
    delete fileMgrThread;

    appThread->join();
    delete appThread;

    fileManager->stop();
    networkManager->stop();
}

App::~App() {
    stop();
}

void App::listener() {
    // bind, listen on settings.app_port and return fileManager->myIds();
    // stop when this->running == false
}

App::host_id_map App::getPeersIds() {
    // connect to all networkManager->get_peers():settings.app_port and get their ids
    return host_id_map();
}


namespace {
    vector<string> split(const string & str, const string & delimiter) {
        std::remove_const<decltype(string::npos)>::type start = 0, end = 0;
        vector<string> output;
        while ((end = str.find(delimiter, start)) != string::npos) {
            output.push_back(str.substr(start, end - start));
            start = end + 1;
        }
        output.push_back(str.substr(start));
        return output;
    }

    vector<string> fixPeers(const vector<PeerInfo> & inputPeers) {
        vector<string> fixedPeers;
        for (const auto & peer : inputPeers) {
            const auto parts = split(peer.address, "/");
            fixedPeers.push_back(parts[0] + ":" + parts.back());
        }
        return fixedPeers;
    }


    //TODO: make cleaner impl, maybe move to fileManager?
    std::vector<App::FileInfo> chunkifyFile(const std::string & filePath) {
        std::ifstream file(filePath, std::ios::in | std::ios::binary);
        if (!file) {
            return {};
        }
        file.seekg(0, std::ios::end);
        uint64_t size = file.tellg();
        file.close();

        std::vector<App::FileInfo> chunks;

        uint64_t c = 0;
        // push chunks with size App::fileChunkSize
        for (; size >= App::fileChunkSize; c += App::fileChunkSize) {
            chunks.push_back({ c, App::fileChunkSize, {} });
            size -= App::fileChunkSize;
        }

        // if there is more of the file, make it last chunk
        if (size > 0) {
            chunks.push_back({ c, size, {} });
        }

        return chunks;
    }
}

bool App::importFile(const std::string & filePath) {
    cout << "Imporintg file " << filePath << endl;

    if (storage.find(filePath) != storage.end()) {
        cout << "File already in storage" << endl;
        return false;
    }

    auto peers = fixPeers(networkManager->get_peers());

	if (!peers.size()) {
        cout << "No peers connected" << endl;
		return false;
	}

    std::vector<FileInfo> fileChunks = chunkifyFile(filePath);

	auto peerIter = peers.cbegin();

    // circular iterator over std::vector
    auto next = [](decltype(peerIter) & iter, decltype(peers) & container) {
        if (++iter == container.end()) {
            iter = container.begin();
        }
    };

    for (auto & chunk : fileChunks) {

        for (int c = 0; c < static_cast<int>(FileAvailability::High); ++c) {
            cout << "Sending chunk to " << *peerIter << endl;
            uint64_t sentChunkId = fileManager->send(*peerIter, filePath, chunk.start, chunk.start + chunk.size);

            if (sentChunkId == 0) {
                cout << "Failed to transfer chunk to host " << *peerIter << " chunk start " << chunk.start << endl;
                break;
            } else {
                cout << "Imorted chunk with ID " << sentChunkId << " to host " << *peerIter << endl;
                chunk.hosts.push_back(make_pair(*peerIter, sentChunkId));
            }
        }

        next(peerIter, peers);
    }

    storage[filePath] = fileChunks;
    return true;
}

App::FileAvailability App::isFileInStorage(const std::string & filePath) {
    cout << "Checking for file " << filePath << " in storage " << endl;
    auto file = storage.find(filePath);

    if (file == storage.end()) {
        cout << "File not in storage" << endl;
        return FileAvailability::None;
    }

    const auto & peers = networkManager->get_peers();

	if (!peers.size()){
        cout << "No peers connected" << endl;
		return FileAvailability::None;
	}

    FileAvailability fileStatus = FileAvailability::High;

    // Go trough all of the chunks and check which of them has least connected hosts
    for (auto & chunk : file->second) {
        int currentChunk = 0;
        for (auto & host : chunk.hosts) {

            cout << "Checking chunk with ID: " << host.second << " with host " << host.first << endl;

            currentChunk +=
                std::any_of(peers.begin(), peers.end(), [&host](const PeerInfo & info) {
                    const auto & parts = split(info.address, "/");
                    const auto & peerAddress = parts[0] + ":" + parts.back();

                    return peerAddress == host.first && info.connected;
                });
        }
        if (!currentChunk) {
            cout << "Chunk not found - " << chunk.start << endl;
        }
        fileStatus = static_cast<FileAvailability>(
            std::min<int>(currentChunk, static_cast<int>(fileStatus)));
    }

    return fileStatus;
}

bool App::exportFile(const std::string & storageFile, const std::string & outputFilePath) {
    cout << "Exporting file " << storageFile << " to " << outputFilePath << endl;
    auto file = storage.find(storageFile);
    if (file == storage.end()) {
        cout << "File not in storage" << endl;
        return false;
    }

    ofstream outputFile(outputFilePath, ios::trunc | ios::binary);
    if (!outputFile) {
        cout << "Failed to open export file " << outputFilePath << endl;
        return false;
    }

    for (const auto & chunk : file->second) {
        bool gotChunk = false;
        for (const auto & chunkLocation : chunk.hosts) {
            cout << "Getting chunk ID: " << chunkLocation.second << " from host " << chunkLocation.first << " ... ";
            auto data = fileManager->getFile(chunkLocation.first, chunkLocation.second);
            if (data) {
                cout << " Success!" << endl;
                gotChunk = true;
                if (!outputFile.write(data.get(), chunk.size)) {
                    return false;
                }
                break; // all good chink written to file
            } else {
                cout << " Failed!"<< endl;
            }

        }
        if (!gotChunk) {
            cout << "Chunk failed to export" << endl;
            return false;
        }
    }

    return true;
}
