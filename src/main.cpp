#include <iostream>
#include <memory>

#include "file_manager/client/FileClient.h"
#include "common/p2p_network_interface.h"
#include "app/app.h"


using namespace std;

int main() {
    std::unique_ptr<FileManagerInterface> fileManager(nullptr);
    std::unique_ptr<P2PNetworkInterface> networkManager(nullptr);

    App app(std::move(fileManager), std::move(networkManager));
    app.run();
    return 0;
}