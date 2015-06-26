#include "socket.h"
#include "socket.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
//#include <netinet/in.h>

#ifdef C_WIN_SOCK
    #include <winsock2.h>
    #include <WS2tcpip.h>
#else
    #define SOCKET_ERROR -1
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/fcntl.h>
#endif




Socket::Socket() : fd(INVALID_SOCKFD)
{
#ifdef C_WIN_SOCK
    // init MS lib
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    this->fd = socket(AF_INET, SOCK_STREAM, PROTOCOL);

    if (this->fd == INVALID_SOCKFD) {
        std::cerr << "Cannot create socket !\n";
        exit(1);
    }
    this->clearBlockFlag();
}

Socket::Socket(int sockFd): fd(sockFd)
{
    this->clearBlockFlag();
#ifdef C_WIN_SOCK
    // init MS lib
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}


Socket::Socket(Socket&& other)
{
    this->fd = other.fd;
    other.fd = INVALID_SOCKFD;
    this->clearBlockFlag();
}

Socket & Socket::operator=(Socket && other)
{
    if (this == &other) {
        return *this;
    }

    if (this->fd != INVALID_SOCKFD) {
        this->close();
    }

    this->fd = other.fd;
    other.fd = INVALID_SOCKFD;

    return *this;
}


Socket::~Socket()
{
    this->close();
}


uint32_t Socket::getPeerAddress()
{
    this->clearBlockFlag();
    if (this->fd == INVALID_SOCKFD) {
        return 0;
    }
    return getSockAddr().sin_addr.s_addr;
}

std::string Socket::getPeerName()
{
    this->clearBlockFlag();
    if (this->fd == INVALID_SOCKFD)
        return "";

    return std::string(inet_ntoa(getSockAddr().sin_addr));
}

void Socket::bindTo(unsigned short port)
{
    this->clearBlockFlag();
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(addr.sin_addr.s_addr == -1){
        std::cerr << "Socket::bind_to : error converting address to int!\n";
        exit(1);
    }

    int yes = 1;
    if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&yes), sizeof(int)) == -1) {
        std::cerr << "Failed to setsockop SO_REUSEADDR\n";
        exit(1);
    }


    if(bind(this->fd, (sockaddr *) &addr, sizeof(addr)) != 0){
        std::cerr << "Unsuccessful binding!\n";
        exit(1);
    }
}


int Socket::connectTo(const std::string& ip, uint16_t port)
{
    this->clearBlockFlag();
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if(server_addr.sin_addr.s_addr == (-1)) {
        return -1;
    }

    return connect(this->fd, (sockaddr*)&server_addr, sizeof(server_addr));
}

int Socket::connectTo(const sockaddr_in* server_addr)
{
    this->clearBlockFlag();
    return connect(this->fd, (sockaddr*)server_addr, sizeof(*server_addr));
}

int Socket::recv(void * buf, int len, int flags)
{
    this->clearBlockFlag();
    int result = ::recv(this->fd, reinterpret_cast<char *>(buf), len, flags);
    if (result == -1) {
        this->setBlockFlag();
    }
    return result;
}

int Socket::send(const void * buf, int len, int flags)
{
    this->clearBlockFlag();
    int result = ::send(this->fd, reinterpret_cast<const char *>(buf), len, flags);
    if (result == -1) {
        this->setBlockFlag();
    }
    return result;
}

void Socket::addOption(int option)
{
    int opt_val;
    setsockopt(this->fd, SOL_SOCKET, option, reinterpret_cast<const char*>(&opt_val), sizeof(int));
}

bool Socket::makeNonblocking()
{

#ifdef C_WIN_SOCK
    u_long mode = 1;
    if (ioctlsocket(this->fd, FIONBIO, &mode) != NO_ERROR) {
#else
    if(fcntl(this->fd, F_SETFL, O_NONBLOCK) == -1) {
#endif
        return false;
    }
    return true;
}


bool Socket::listen()
{
    return (::listen(this->fd, BACKLOG) == 0);
}


ClientInfo Socket::accept()
{
    this->clearBlockFlag();

    ClientInfo info;
    socklen_t addr_size = sizeof(info.addr);
    info.sock_fd = ::accept(this->fd, (sockaddr *) &info.addr, &addr_size);
    if (info.sock_fd == -1) {
        this->setBlockFlag();
    }

    return info;
}

Socket Socket::acceptSocket()
{
    this->clearBlockFlag();
    Socket s(::accept(this->fd, nullptr, nullptr));
    if (s == -1) {
        this->setBlockFlag();
    }
    return s;
}

bool Socket::wouldHaveBlocked()
{
    return wasBlocking;
}

sockaddr_in Socket::getSockAddr()
{
    sockaddr_in info;
    socklen_t size = sizeof(sockaddr_in);
    getpeername(this->fd, reinterpret_cast<sockaddr*>(&info), &size);
    return info;
}

void Socket::close()
{
    if (this->fd != INVALID_SOCKFD) {
#ifdef C_WIN_SOCK
        closesocket(this->fd);
#else
        close(this->fd);
#endif
        this->fd = INVALID_SOCKFD;
    }

}

void Socket::setBlockFlag()
{
#ifdef C_WIN_SOCK
    int err = WSAGetLastError();
    wasBlocking = err && err == WSAEWOULDBLOCK;
#else
    wasBlocking = errno && (errno == EAGAIN || errno == EWOULDBLOCK);
#endif
}

void Socket::clearBlockFlag()
{
    this->wasBlocking = false;
}
