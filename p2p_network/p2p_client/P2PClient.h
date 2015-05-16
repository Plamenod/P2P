#ifndef P2PCLIENT_H_INCLUDED
#define P2PCLIENT_H_INCLUDED

#include "p2p_network_interface.h"

class P2PClient: public P2PNetworkInterface
{
public:
    P2PClient(){}
    virtual ~P2PClient() override;

    int start() const;
    int connect() const;

    virtual std::vector<PeerInfo> get_peers() const;

    P2PClient(const P2PClient&) = delete;
    P2PClient& operator=(const P2PClient&) = delete;

private:

};

#endif // P2PCLIENT_H_INCLUDED
