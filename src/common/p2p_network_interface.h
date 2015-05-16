#ifndef _P2P_NETWORK_INTERFACE_
#define _P2P_NETWORK_INTERFACE_

#include <string>
#include <vector>

#include "CommonDefines.h"

class P2PNetworkInterface {
public:
	virtual std::vector<PeerInfo> get_peers() const = 0;
	virtual ~P2PNetworkInterface(){}
};

#endif // _P2P_NETWORK_INTERFACE_
