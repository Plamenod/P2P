#include "p2pclient.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

P2PClient::~P2PClient()
{

}

void P2PClient::start() const
{
    std::string ip;
    std::cout << "Enter main server ip: " << std::endl;
    std::cin >> ip;
    socket.connectTo(ip, port);
}
