#ifndef P2PCLIENT_H_INCLUDED
#define P2PCLIENT_H_INCLUDED

#include "Socket.h"
#include <string>

const unsigned short port = 7777;

class P2PClient
{
public:
    P2PClient(){}
    virtual ~P2PClient();

    void start() const;
    void connectTo(const std::string& server_address) const;

    P2PClient(const P2PClient&) = delete;
    P2PClient& operator=(const P2PClient&) = delete;

private:
    Socket socket;
};

#endif // P2PCLIENT_H_INCLUDED
