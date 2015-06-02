#include "app.h"

#include <chrono>
#include <thread>
#include <algorithm>

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




bool App::addFileToStorage(const std::string & filePath) {
    return false;
}

App::FileAvailability App::isFileInStorage(const std::string & filePath) {
    auto file = storage.find(filePath);
    if (file == storage.end()) {
        return FileAvailability::None;
    }

    auto peers = networkManager->get_peers();
    FileAvailability fileStatus = FileAvailability::High;

    for (auto & chunk : file->second) {
        int currentChunk = 0;
        for (auto & host : chunk.hosts) {
            currentChunk +=
                std::any_of(peers.begin(), peers.end(), [&host](const PeerInfo & info) {
                    return info.address == host && info.connected;
                });
        }
        fileStatus = static_cast<FileAvailability>(
            std::min<int>(currentChunk, static_cast<int>(fileStatus)));
    }

    return fileStatus;
}
