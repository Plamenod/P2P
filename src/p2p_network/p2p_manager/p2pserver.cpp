#include "p2pserver.h"
#include <iostream>
#include <thread>
#include <chrono>
const unsigned int BUFFERSIZE = 1024;

void P2PServer::setPorts(uint16_t port)
{
    this->port = port;
}

void P2PServer::start(bool& flag) const
{
    this->socket.bindTo(port);
	this->socket.makeNonblocking();
    std::cout << "P2P server listening at port " << port << std::endl;
    this->run(flag);
}

void P2PServer::run(bool& flag) const
{
    while(flag) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        Socket client = socket.acceptSocket(); // just accept connections from main server
    }
}
