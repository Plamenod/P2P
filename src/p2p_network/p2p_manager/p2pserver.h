#ifndef P2P_SERVER_H_
#define P2PSERVER_H_

#include "socket.h"
#include "commondefines.h"
#include <vector>

#define PORT 23457

class P2PServer{

public:
	P2PServer(unsigned short port = PORT): port(port) {};

	P2PServer(const P2PServer& other) = delete;
	P2PServer& operator =(const P2PServer& other) = delete;

	~P2PServer() {};

	void start() const;

private:
    void run()const;

	Socket socket;
	unsigned short port;
};

#endif // P2P_SERVER_H_
