#include "p2pnode.h"

P2PNode::~P2PNode()
{
    stop();
}

std::vector<PeerInfo> P2PNode::get_peers()
{
    std::vector<PeerInfo> peers;
    client.getPeersInfo(peers);
    return peers;
}

void P2PNode::setPorts(uint16_t ms_port, uint16_t server_port, uint16_t file_mgr_port)
{
    client.setPorts(ms_port, server_port, file_mgr_port);
}

void P2PNode::start(const std::string& server_ip)
{
    client.connectToServer(server_ip);
}

void P2PNode::stop()
{
    client.disconnectFromServer();
}