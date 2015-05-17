#ifndef P2PCLIENT_H_INCLUDED
#define P2PCLIENT_H_INCLUDED

#include "socket.h"
#include "commondefines.h"
#include <string>
#include <vector>

#define PORT 23456

class P2PClient
{
public:
    P2PClient(){}
    ~P2PClient();
    P2PClient(const P2PClient&) = delete;
    P2PClient& operator=(const P2PClient&) = delete;

    void setServerIP(const std::string& ip);

    void connectToServer(const std::string& ip, unsigned short port = PORT);
    void getPeersInfo(std::vector<PeerInfo>& result) const;

private:
    size_t send(const void* buf, size_t size, int flags = 0) const;
    size_t recv(void* buf, size_t size, int flags = 0) const;
    void receivePeersInfo(std::vector<PeerInfo>& result) const;

    Socket socket;
    std::string server_ip;
};

#endif // P2PCLIENT_H_INCLUDED
