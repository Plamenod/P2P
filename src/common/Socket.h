#ifndef SOCKET_H_
#define SOCKET_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PROTOCOL 0
#define INVALID_SOCKFD -1


class Socket{

public:

	Socket();
	//Socket(int domain, int socktype);

	Socket(const Socket& other) = delete;
	Socket& operator =(const Socket& other) = delete;

	~Socket() { close(fd); };

	operator int() const { return fd; };

	int bind(short port);

private:
	int fd;
};

#endif // SOCKET
