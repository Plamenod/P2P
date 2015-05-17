#ifndef P2P_SERVER_H_
#define P2PSERVER_H_

#include "socket.h"


class P2PServer{

public:
	P2PServer() {};

	P2PServer(const P2PServer& other) = delete;
	P2PServer& operator =(const P2PServer& other) = delete;

	~P2PServer() {};

	void start() const;

private:
	Socket socket;
};

#endif // P2P_SERVER_H_
