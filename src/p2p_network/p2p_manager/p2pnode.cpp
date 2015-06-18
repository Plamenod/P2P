#include "p2pnode.h"

P2PNode::~P2PNode()
{
    stopServer();
}
std::vector<PeerInfo> P2PNode::get_peers() const
{
    std::vector<PeerInfo> peers;
    client.getPeersInfo(peers);
    return peers;
}

void P2PNode::setPorts(uint16_t ms_port, uint16_t server_port, uint16_t file_mgr_port)
{
    client.setPorts(ms_port, server_port, file_mgr_port);
    server.setPorts(server_port);
}

void P2PNode::start(const std::string& server_ip)
{
    client.connectToServer(server_ip);
    startServer();
}

void P2PNode::stop() {
    stopServer();
}

void P2PNode::startServer()
{
    server_flag = true;
    server_thread = std::thread(&P2PServer::start, &server, std::ref(server_flag));
}

void P2PNode::stopServer()
{
    server_flag = false;
	if (server_thread.joinable()) {
		server_thread.join();
	}

}
