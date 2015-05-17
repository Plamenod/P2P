#include "p2p_main_server.h"

#include <cstring>
#include <iostream>


void P2PMainServer::start()
{
	this->socket.bindTo(MSPORT);
	if(!this->socket.listen()){
		std::cerr << "Unsuccessful listening request!\n";
		exit(1);
	}

	this->socket.makeNonblocking();

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));

	while(true){
		ClientInfo current_client = this->socket.accept();
		if(current_client.sock_fd != INVALID_SOCKFD){
			this->clients.push_back(current_client);

			size_t clients_count = this->clients.size();
			for(size_t i = 0; i < clients_count; ++i){
				ssize_t received_len = recv(clients[i].sock_fd, buffer, BUFFER_SIZE, 0);

				if(received_len == 1){
					switch(buffer[0]){
						case '1': break;
					}
				}
			}
		}

	}
}
