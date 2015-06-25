#ifndef P2NODE_H_INCLUDED
#define P2PNODE_H_INCLUDED

#include "p2p_network_interface.h"
#include "p2pclient.h"
#include "p2pserver.h"
#include <thread>

#define MAIN_SERVER_PORT 23456
#define SERVER_PORT 23457
#define FILE_MANAGER_PORT 23458

class P2PNode: public P2PNetworkInterface
{
public:
    P2PNode(
        uint16_t main_server_port = MAIN_SERVER_PORT,
        uint16_t server_port = SERVER_PORT,
        uint16_t file_mgr_port = FILE_MANAGER_PORT
        ) :
        client(main_server_port, server_port, file_mgr_port), server(server_port) {}
    virtual ~P2PNode();
    virtual std::vector<PeerInfo> get_peers();

    virtual void start(const std::string& server_ip);
    virtual void stop();

    virtual void setPorts(uint16_t ms_port, uint16_t server_port, uint16_t file_mgr_port);

    void startServer();
    void stopServer();
private:
    P2PClient client;
    P2PServer server;
    std::thread server_thread;
    bool server_flag;
};


#endif // P2PCLIENT_H_INCLUDED
