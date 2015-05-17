#include "p2p_main_server.h"

#include <iostream>


void P2PMainServer::start()
{
	this->socket.bindTo(MSPORT);
	if(!this->socket.listen()){
		std::cerr << "Unsuccessful listening request!\n";
		exit(1);
	}

	this->socket.makeNonblocking();

	while(true){
		ClientInfo current_client = this->socket.accept();
		if(current_client.sock_fd != INVALID_SOCKFD){
			this->clients.push_back(current_client);

			size_t clients_count = this->clients.size();
			for(int i  = 0; i < clients_count; ++i)
			{

			}
		}

	}
}
