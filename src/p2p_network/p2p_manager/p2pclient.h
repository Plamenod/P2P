#ifndef P2PCLIENT_H_INCLUDED
#define P2PCLIENT_H_INCLUDED

#include "socket.h"
#include "commondefines.h"
#include <string>
#include <vector>

class P2PClient
{
public:
    P2PClient(uint16_t main_server_port, uint16_t server_port, uint16_t file_mgr_port):
        main_server_port(main_server_port),
        server_port(server_port) ,
        file_mgr_port(file_mgr_port) {}


    P2PClient(const P2PClient&) = delete;
    P2PClient& operator=(const P2PClient&) = delete;

    void setServerIP(const std::string& ip);
    void setPorts(uint16_t ms_port, uint16_t server_port, uint16_t file_mgr_port);

    void connectToServer(const std::string& ip);
    void getPeersInfo(std::vector<PeerInfo>& result);

private:
    int send(const void* buf, size_t size, int flags = 0);
    int recv(void* buf, size_t size, int flags = 0);

    void receivePeersInfo(std::vector<PeerInfo>& result);
    void sendPortsToMainServer();

    Socket socket;
    uint16_t main_server_port;
    uint16_t server_port;
    uint16_t file_mgr_port;
    std::string server_ip;
};

#endif // P2PCLIENT_H_INCLUDED
