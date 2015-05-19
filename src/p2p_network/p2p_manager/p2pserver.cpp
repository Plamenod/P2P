#include "p2pserver.h"
#include <iostream>

const unsigned int BUFFERSIZE = 1024;

void P2PServer::start() const
{
	this->socket.bindTo(port);
	std::cout << "P2P server listening at port " << port << std::endl;
}

void P2PServer::run() const
{
    while(true) {
        ClientInfo client = socket.accept();
        char* ip_addr = inet_ntoa(client.addr.sin_addr);
        std::cout << "Accepted a client with ip " << ip_addr << std::endl;
        close(client.sock_fd);
    }
}
