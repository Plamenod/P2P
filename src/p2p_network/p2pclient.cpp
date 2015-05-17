#include "p2pclient.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const int tries = 10;

P2PClient::~P2PClient()
{

}

void P2PClient::setServerIP(const std::string& ip)
{
    server_ip = ip;
}

void P2PClient::connectToServer(const std::string& ip)
{
    setServerIP(ip);
    socket.connectTo(ip, port);
}

size_t P2PClient::send(const void* buf, size_t size, int flags) const
{
    return ::send(socket.getFd(), buf, size, flags);
}

void P2PClient::acquirePeersInfo() const
{
    char cmd = GET_PEERS;
    size_t sent;
    for(int i = 0; i < tries; ++i) {
        sent = send(&cmd, sizeof(cmd));
        if(sent > 0) break;
    }

    if(sent <= 0) {
        std::cerr << "No connection to main server " << std::endl;
        exit(1);
    }
}
