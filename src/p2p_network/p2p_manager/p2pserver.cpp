#include "p2pserver.h"

#define PORT 6666

void P2PServer::start() const
{
	this->socket.bindTo(PORT);
}

