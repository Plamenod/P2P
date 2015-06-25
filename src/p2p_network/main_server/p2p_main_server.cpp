#include "p2p_main_server.h"
#include "commondefines.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <thread>
#include <chrono>

#ifndef C_WIN_SOCK
#include <arpa/inet.h>
#include <errno.h>
#endif

#define BUFFER_SIZE 16384

void P2PMainServer::start(int port)
{
    this->socket.makeNonblocking();

    this->socket.bindTo(port);
    if(!this->socket.listen()){
        std::cerr << "Unsuccessful listening request!\n";
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    std::cout << "Server waiting for connections at port " << port << std::endl;
	isRunning = true;
    while(isRunning) {
        serveConnectedClients(buffer);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        Socket client = this->socket.acceptSocket();
        if(client == -1) {
            continue;
        }
        handleClientConnect(client);
    }
}

void P2PMainServer::stop() {
	isRunning = false;
}

void P2PMainServer::handleClientConnect(Socket & client)
{

    if (client != -1) {
        if (!client.makeNonblocking()) {
            std::cerr << "Failed to set client to nonblocking !\n";
            exit(1);
        }

        this->clients.emplace_back(ClientDescriptor{ std::move(client), 0, 0 });
    }
}

void P2PMainServer::serveConnectedClients(char* in_buffer)
{
    size_t clients_count = this->clients.size();
    Command cmd;

	std::vector<int> dcClient;

    for(size_t i = 0; i < clients_count; ++i) {
        int received_len = clients[i].sock.recv(&cmd, sizeof(cmd));
		if (received_len == 0) {
			// save all indecies of disconnected clients
			dcClient.push_back(i);
		} else if (received_len == -1) {
			bool wouldBlock = false;

#ifdef C_WIN_SOCK
			int err = WSAGetLastError();
			wouldBlock =  err && err == WSAEWOULDBLOCK;
#else
			wouldBlock = errno && (errno == EAGAIN || errno == EWOULDBLOCK);
#endif

			if (!wouldBlock) {
				dcClient.push_back(i);
			}

		} else if (received_len == sizeof(Command)) {
            switch(cmd) {
                case Command::GET_PEERS:
                    sendPeersInfo(i);
                    break;
                case Command::LISTENING_PORT:
                    acceptClientListeningPort(in_buffer, i);
                    break;
                default:
                    break;
            }
        }
    }

	if (dcClient.size() > 0) {
		int clientsCounter = 0;
		// go trough the clients and remove those with saved indecies
		clients.erase(std::remove_if(clients.begin(), clients.end(), [&](const ClientDescriptor &) {
			return find(dcClient.begin(), dcClient.end(), clientsCounter++) != dcClient.end();
		}), clients.end());
	}
}

void P2PMainServer::sendPeersInfo(int out_peer_index)
{
    std::vector<ServerInfo> connected_peers;

    checkPeers(connected_peers, out_peer_index);

    int count = connected_peers.size();
    char buffer[BUFFER_SIZE];
    memcpy(buffer, &count, sizeof(count));
    int offset = sizeof(count);

    int port_size = sizeof(decltype(ServerInfo::server_port));
    int addr_size = sizeof(decltype(ServerInfo::ip_addr));

    for(int i = 0; i < connected_peers.size(); ++i) {
        memcpy(buffer + offset, &connected_peers[i].ip_addr, addr_size);
        offset += addr_size;
        memcpy(buffer + offset, &connected_peers[i].server_port, port_size);
        offset += port_size;
        memcpy(buffer + offset, &connected_peers[i].file_mgr_port, port_size);
        offset += port_size;
    }
    int msg_size = sizeof(count) + count * (addr_size + 2 * port_size);
    int sent = clients[out_peer_index].sock.send(buffer, msg_size);
    if(sent < msg_size) {
        std::cerr << "Failed sending peers info !" << std::endl;
        exit(1);
    }
    std::cout << "Peer's info sent " << std::endl;
    std::cout << count << " peers connected" << std::endl;
}

void P2PMainServer::checkPeers(std::vector<ServerInfo> & connected_peers, int out_peer_index)
{
    ServerInfo temp;
    for(int i = 0; i < clients.size(); ++i) {
        if(clients[i].server_port == 0 || i == out_peer_index) {
            continue;
        }
        //Socket sock;
        //sockaddr_in addr = clients[i].addr;
        //addr.sin_port = htons(clients[i].server_port);
        /**connect to returns -1 on failure*/
        //if(sock.connectTo(&addr) != -1) {
        //    continue;
        //}

        temp.ip_addr = clients[i].sock.getPeerAddress();
        temp.server_port = clients[i].server_port;
        temp.file_mgr_port = clients[i].file_mgr_port;
        connected_peers.push_back(temp);
    }
}

void P2PMainServer::acceptClientListeningPort(char* in_buffer, int client_index)
{
    int offset = 0;
    int server_port_size = sizeof(clients[client_index].server_port);
    int file_mgr_port_size = sizeof(clients[client_index].file_mgr_port);
    int mesg_size = server_port_size + file_mgr_port_size;

    int received = clients[client_index].sock.recv(in_buffer, mesg_size);
    if(received <= 0) {
        std::cerr << "No server and file manager ports received !" << std::endl;
        exit(1);
    }
    clients[client_index].server_port = *((uint16_t*)(in_buffer + offset));
    offset += sizeof(clients[client_index].server_port);
    clients[client_index].file_mgr_port = *((uint16_t*)(in_buffer + offset));

    std::cout << "Server port: " << clients[client_index].server_port << std::endl;
    std::cout << "File manager port: " << clients[client_index].file_mgr_port << std::endl;
}
