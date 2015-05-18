#include <iostream>
#include <memory>

#include "file_manager/client/FileClient.h"
#include "common/p2p_network_interface.h"
#include "app/app.h"

class test : public P2PNetworkInterface {
public:
    virtual std::vector<PeerInfo> get_peers() const {
        return std::vector<PeerInfo>();
    };
    virtual ~test(){}
};

using namespace std;

int main() {
    std::unique_ptr<FileManagerInterface> fileManager(new FileClient());
    std::unique_ptr<P2PNetworkInterface> networkManager(new test());

    App app(std::move(fileManager), std::move(networkManager));
    app.run();
    return 0;
}