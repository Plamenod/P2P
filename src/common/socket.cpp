#include "socket.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


Socket::Socket() : fd(INVALID_SOCKFD)
{
	this->fd = socket(AF_INET, SOCK_STREAM, PROTOCOL);

	if(this->fd < 0)
	{
		std::cerr << "Cannot create socket !\n";
		exit(1);
	}
}


Socket::~Socket()
{
	close(this->fd);
}


void Socket::bindTo(unsigned short port) const
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	if(addr.sin_addr.s_addr == -1)
	{
		std::cerr << "Socket::bind_to : error converting address to int!\n";
		exit(1);
	}

	if(bind(this->fd, (sockaddr *) &addr, sizeof(addr)) != 0)
	{
		std::cerr << "Unsuccessful binding!\n";
		exit(1);
	}
}


void Socket::connectTo(const std::string& ip, unsigned short port) const
{
	sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    if(server_addr.sin_addr.s_addr == (-1)) {
        std::cerr << "Cannot connect to server !" << std::endl;
        exit(1);
    }

    if(connect(this->fd, (sockaddr*)&server_addr, sizeof(server_addr))) {
        std::cerr << "Cannot connect to server !" << std::endl;
        exit(1);
    }
}

