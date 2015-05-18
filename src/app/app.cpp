#include "app.h"

#include <chrono>
#include <thread>

App::App(
    std::unique_ptr<FileManagerInterface> fileManager,
    std::unique_ptr<P2PNetworkInterface> networkManager):
    fileManager(std::move(fileManager)),
    networkManager(std::move(networkManager)) {}

void App::run() {
    while(1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}