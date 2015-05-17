#include "p2p_main_server.h"

#include <cstring>
#include <iostream>
#include <thread>
#include <chrono>

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
	    std::this_thread::sleep_for(std::chrono::milliseconds(1));
		ClientInfo current_client = this->socket.accept();
		handleClientConnect(current_client);
		serveConnectedClients(buffer);
	}
}

void P2PMainServer::handleClientConnect(ClientInfo& client)
{
    if(client.sock_fd != INVALID_SOCKFD){
        client.connected = true;
        client.server_port = 0;
        this->clients.push_back(client);
    }
}

void P2PMainServer::serveConnectedClients(char* in_buffer)
{
    size_t clients_count = this->clients.size();
    for(size_t i = 0; i < clients_count; ++i){
        ssize_t received_len = recv(clients[i].sock_fd, in_buffer, BUFFER_SIZE, 0);
        if(received_len == 1){
            switch(in_buffer[0]){
                case '1': break;
            }
        }
    }
}
