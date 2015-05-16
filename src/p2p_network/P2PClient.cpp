#include "P2PClient.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

P2PClient::~P2PClient()
{

}

void P2PClient::connectTo(const std::string& ip) const
{
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    if(server_addr.sin_addr.s_addr == (-1)) {
        std::cerr << "Cannot connect to server !" << std::endl;
        exit(1);
    }

    int return_value = connect(socket.getFd(), (sockaddr*)&server_addr, sizeof(server_addr));
    if(return_value) {
        std::cerr << "Cannot connect to server !" << std::endl;
        exit(1);
    }
}

void P2PClient::start() const
{
    std::string ip;
    std::cout << "Enter main server ip: " << std::endl;
    std::cin >> ip;
    connectTo(ip);
}
