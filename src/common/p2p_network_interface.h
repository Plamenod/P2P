#ifndef _P2P_NETWORK_INTERFACE_
#define _P2P_NETWORK_INTERFACE_

#include <string>
#include <vector>

#include "commondefines.h"

class P2PNetworkInterface {
public:
    virtual std::vector<PeerInfo> get_peers() = 0;

    virtual void start(const std::string& server_ip) = 0;
    virtual void stop() = 0;

    virtual void setPorts(uint16_t ms_port, uint16_t server_port, uint16_t file_mgr_port) = 0;
    virtual ~P2PNetworkInterface(){}
};

#endif // _P2P_NETWORK_INTERFACE_
