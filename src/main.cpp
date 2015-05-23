#include <iostream>
#include <memory>

#include "file_manager/FileManager.h"
#include "p2p_network/p2p_manager/p2pclient.h"
#include "app/app.h"


using namespace std;

int main() {
    for (int c = 0; c < 10; ++c) {
        P2PClient(23456, 1010, 1011).connectToServer("127.0.0.1");
    }
    cin.get();
    
    return 0;
}