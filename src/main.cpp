#include <iostream>
#include <memory>
#include <thread>
#include "file_manager/FileManager.h"
#include "p2p_network/p2p_manager/p2pnode.h"
#include "app/app.h"
#include "p2p_network/p2p_manager/p2pnode.h"

using namespace std;

int main() {
<<<<<<< HEAD
    App::Settings settings;
    settings.app_port = 1001;
    settings.file_mgr_port = 1002;
    settings.server_port = 1003;

    settings.main_server = "127.0.0.1";
    settings.ms_port = 5005;
    
    App app(
        settings,
        std::unique_ptr<FileManagerInterface>(new FileManager(settings.file_mgr_port)), 
        std::unique_ptr<P2PNetworkInterface>(new P2PNode(settings.ms_port, settings.server_port, settings.file_mgr_port)));

    app.run();

    while (1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
=======

    //for (int c = 0; c < 10; ++c) {
    //    P2PClient(23456, 1010 + c, 1011 + c).connectToServer("127.0.0.1");
    //}
    
    std::cout << "------------------------------------------------\n";

    P2PNode node;
    node.setPorts(23456, 1010, 1011);
    
    std::thread t([&node]() {
        node.start("127.0.0.1");
    });
    t.detach();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    for (auto & p : node.get_peers()) {
        cout << p.address << " " << p.connected << std::endl;
    }

    cin.get();
>>>>>>> temp_fixes
    
    return 0;
}