#include "Socket.h"

#include <cstdlib>
#include <iostream>

Socket::Socket() : fd(INVALID_SOCKFD)
{
	this->fd = socket(AF_INET, SOCK_STREAM, PROTOCOL);

	if(this->fd < 0)
	{
		std::cerr << "Cannot create socket !\n";
		exit(1);
	}
}
