#include "p2pserver.h"

const unsigned int BUFFERSIZE = 1024;

void P2PServer::start() const
{
	this->socket.bindTo(port);

}

void P2PServer::run() const
{
    while(true) {

    }
}
