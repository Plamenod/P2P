#include "app.h"

#include <chrono>
#include <thread>
#include <algorithm>
#include <fstream>

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

    fileMgrThread = new std::thread(
        std::bind(&FileManagerInterface::run, &*fileManager));

    appThread = new std::thread(std::bind(&App::listener, this));
}

void App::stop() {
    running = false;
    fileManager->stop();

    appThread->join();
    fileMgrThread->join();

    delete fileMgrThread;
    delete appThread;
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
    //TODO: make cleaner impl, maybe move to fileManager?
    std::vector<App::FileInfo> chunkifyFile(const std::string & filePath) {
        std::ifstream file(filePath, std::ios::in | std::ios::binary);
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


bool App::addFileToStorage(const std::string & filePath) {
    if (storage.find(filePath) != storage.end()) {
        return false;
    }

    const auto & peers = networkManager->get_peers();

    std::vector<FileInfo> fileChunks = chunkifyFile(filePath);

    int chunksPerHost =
        ((fileChunks.size() * static_cast<int>(FileAvailability::High)) / peers.size()) + 1;

    auto chunkIt = fileChunks.begin(), chunkEnd = fileChunks.end();
    for (const auto & host : peers) {
        for (int c = 0; c < chunksPerHost && chunkIt != chunkEnd; ++c, ++chunkIt) {

            uint64_t sentChunkId = fileManager->send(
                host.address, filePath, chunkIt->start, chunkIt->start + chunkIt->size);

            if (sentChunkId == 0) {
                break; // next host
            } else {
                chunkIt->hosts.insert(make_pair(host.address, sentChunkId));
            }
        }
        if (chunkIt == chunkEnd) {
            break;
        }
    }

    if (chunkIt != chunkEnd) {
        // maybe go trough hosts and remove sent chunks
        return false;
    }

    storage[filePath] = fileChunks;
    return true;
}

App::FileAvailability App::isFileInStorage(const std::string & filePath) {
    auto file = storage.find(filePath);
    if (file == storage.end()) {
        return FileAvailability::None;
    }

    const auto & peers = networkManager->get_peers();
    FileAvailability fileStatus = FileAvailability::High;

    // Go trough all of the chunks and check which of them has fewest connected hosts
    for (auto & chunk : file->second) {
        int currentChunk = 0;
        for (auto & host : chunk.hosts) {
            currentChunk +=
                std::any_of(peers.begin(), peers.end(), [&host](const PeerInfo & info) {
                    return info.address == host.first && info.connected;
                });
        }
        fileStatus = static_cast<FileAvailability>(
            std::min<int>(currentChunk, static_cast<int>(fileStatus)));
    }

    return fileStatus;
}
