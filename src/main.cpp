#include <memory>
#include <iostream>


#include "file_manager/FileManager.h"
#include "p2p_network/p2p_manager/p2pnode.h"
#include "app/app.h"

using namespace std;

int main() {


    App::Settings settings;
    settings.app_port = 1001;
    settings.file_mgr_port = 1002;
    settings.server_port = 1003;

    settings.main_server = "127.0.0.1";
    settings.ms_port = 5005;

    auto f = std::unique_ptr<P2PNetworkInterface>(new P2PNode(settings.ms_port, 2020, 2021));
    f->start("127.0.0.1");
    auto q = std::unique_ptr<FileManagerInterface>(new FileManager(2021));
    std::thread t(&FileManagerInterface::run, &*q);

    App app(
        settings,
        std::unique_ptr<FileManagerInterface>(new FileManager(settings.file_mgr_port)),
        std::unique_ptr<P2PNetworkInterface>(new P2PNode(settings.ms_port, settings.server_port, settings.file_mgr_port)));

    app.run();
    const std::string fileName("/home/darina/Desktop/file"),
					  exportName("/home/darina/Desktop/file_received");

    if (!app.importFile(fileName)) {
        std::cerr << "Failed to add file to storage";
        return 0;
    }

    auto r = app.isFileInStorage(fileName);
    if (r != App::FileAvailability::High) {
        std::cerr << "File should be in in High availability";
        return 0;
    }

    if (!app.exportFile(fileName, exportName)) {
        std::cerr << "Failed to export file from storage";
        return 0;
    }

    std::cout << "\n\nAll good exiting";
    std::cin.get();
    return 0;
}
