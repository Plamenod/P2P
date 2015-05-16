#ifndef _P2P_NETWORK_INTERFACE_
#define _P2P_NETWORK_INTERFACE_

#include <string>
#include <vector>

class P2PNetworkInterface {
public:
	virtual std::vector<std::string> get_peers() const = 0;
	virtual ~P2PNetworkInterface() = 0;
};

#endif // _P2P_NETWORK_INTERFACE_