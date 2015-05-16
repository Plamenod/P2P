#include "P2PClient.h"

P2PClient::~P2PClient()
{

}

std::vector<PeerInfo> P2PClient::get_peers() const
{
    return std::vector<PeerInfo>();
}

int P2PClient::start() const
{
    return 0;
}
