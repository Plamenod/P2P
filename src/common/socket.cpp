#include "socket.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>


struct client_info{
	sockaddr_in addr;
	int sock_fd;
};


Socket::Socket(int fdesc) : fd(fdesc)
{
	this->fd = socket(AF_INET, SOCK_STREAM, PROTOCOL);

	if(this->fd < 0){
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
	if(addr.sin_addr.s_addr == -1){
		std::cerr << "Socket::bind_to : error converting address to int!\n";
		exit(1);
	}


    int yes = 1;
    if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        close(this->fd);
        std::cerr << "Failed to setsockop SO_REUSEADDR\n";
        exit(1);
    }


	if(bind(this->fd, (sockaddr *) &addr, sizeof(addr)) != 0){
		std::cerr << "Unsuccessful binding!\n";
		exit(1);
	}
}


void Socket::connectTo(const std::string& ip, unsigned short port) const
{
	sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
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


void Socket::addOption(int option)
{
	int opt_val;
	setsockopt(this->fd, SOL_SOCKET, option, &opt_val, sizeof(int));
}


bool Socket::listen_() const
{
	return (listen(this->fd, BACKLOG) == 0);
}


client_info Socket::accept_() const
{
	client_info info;
	info.sock_fd = accept(this->fd, (sockaddr *) &info.addr, NULL);

	if(info.sock_fd != INVALID_SOCKFD){
        std::cerr << "Error accepting connection request!\n";
        exit(1);
	}

	return info;
}
