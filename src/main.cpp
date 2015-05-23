#include <iostream>
#include <memory>
#include <thread>
#include "file_manager/FileManager.h"
#include "p2p_network/p2p_manager/p2pclient.h"
#include "app/app.h"
#include "p2p_network/p2p_manager/p2pnode.h"

using namespace std;

int main() {

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
    
    return 0;
}