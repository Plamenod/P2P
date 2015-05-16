#ifndef SOCKET_H_
#define SOCKET_H_

#include <string>


#define PROTOCOL 0
#define INVALID_SOCKFD -1


class Socket{

public:

	Socket();
	//Socket(int domain, int socktype);

	Socket(const Socket& other) = delete;
	Socket& operator =(const Socket& other) = delete;

	~Socket();

	operator int() const { return fd; };
	int getFd() const { return fd; };

	void bindTo(unsigned short port) const;

	void connectTo(const std::string& ip, unsigned short port) const;

private:
	int fd;
};

#endif // SOCKET
