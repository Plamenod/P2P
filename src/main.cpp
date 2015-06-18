#include <memory>
#include <iostream>
#include <sstream>

#include "file_manager/FileManager.h"
#include "p2p_network/p2p_manager/p2pnode.h"
#include "app/app.h"

using namespace std;

int main(int argc, char * argv[]) {

    if (argc != 4) {
        std::cerr << "File mgr port, Server port, Main server ip\n";
        return 1;
    }

    App::Settings settings;

    std::stringstream strm;
    for (int c = 1; c < argc; ++c) {
        strm << argv[c] << ' ';
    }

    strm >> settings.file_mgr_port >> settings.server_port
         >> settings.main_server;

	settings.main_server = "127.0.0.1";
	settings.ms_port = 5005;
	settings.app_port = -1;

    App app(
        settings,
        std::unique_ptr<FileManagerInterface>(new FileManager(settings.file_mgr_port)),
        std::unique_ptr<P2PNetworkInterface>(new P2PNode(settings.ms_port, settings.server_port, settings.file_mgr_port)));

    app.run();

    while (1) {
        std::string fileName, exportName;

        cout << "Enter file path to import:\n";
        std::getline(cin, fileName);
        cout << "Enter file path to export:\n";
        std::getline(cin, exportName);

        if (!app.importFile(fileName)) {
            std::cerr << "Failed to add file to storage\n";
            continue;
        } else {
            std::cout << "Imported!\n";
        }

        auto r = app.isFileInStorage(fileName);
        if (r == App::FileAvailability::None) {
            std::cerr << "File should be in in High availability... continuing\n";
        } else if (r != App::FileAvailability::High) {
            std::cerr << "File not available ! \n";
            continue;
        } else {
            std::cout << "Available!\n";
        }

        if (!app.exportFile(fileName, exportName)) {
            std::cerr << "Failed to export file from storage";
        } else {
            std::cout << "Exported!\n";
        }

    }


    std::cout << "\n\nAll good exiting";
    std::cin.get();
    return 0;
}
