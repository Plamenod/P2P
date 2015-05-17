#ifndef P2PCLIENT_H_INCLUDED
#define P2PCLIENT_H_INCLUDED

#include "socket.h"
#include "commondefines.h"
#include <string>
#include <vector>

const unsigned short port = 2332;

enum command: char {
    GET_PEERS = '1',
};

class P2PClient
{
public:
    P2PClient(){}
    ~P2PClient();

    void setServerIP(const std::string& ip);

    void connectToServer(const std::string& ip);
    void acquirePeersInfo() const;

    P2PClient(const P2PClient&) = delete;
    P2PClient& operator=(const P2PClient&) = delete;

private:
    size_t send(const void* buf, size_t size, int flags = 0) const;


    Socket socket;
    std::string server_ip;
};

#endif // P2PCLIENT_H_INCLUDED
