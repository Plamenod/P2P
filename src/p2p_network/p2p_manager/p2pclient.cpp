#include "p2pclient.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>
#include <thread>
#include <cstring>
#include <sstream>

const int retries = 10;
const unsigned int BUFFERSIZE_W = 16384; /// default send size

P2PClient::~P2PClient()
{

}

void P2PClient::setServerIP(const std::string& ip)
{
    server_ip = ip;
}

void P2PClient::connectToServer(const std::string& ip, unsigned short port)
{
    setServerIP(ip);
    socket.connectTo(ip, port);
}

size_t P2PClient::send(const void* buf, size_t size, int flags) const
{
    return ::send(socket.getFd(), buf, size, flags);
}

size_t P2PClient::recv(void* buf, size_t size, int flags) const
{
    return ::recv(socket.getFd(), buf, size, flags);
}

void P2PClient::getPeersInfo(std::vector<PeerInfo>& result) const
{
    char cmd = GET_PEERS;
    size_t sent;
    for(int i = 0; i < retries; ++i) {
        sent = send(&cmd, sizeof(cmd));
        if(sent > 0) break;
    }

    if(sent <= 0) {
        std::cerr << "No connection to main server(sending get peers cmd)" << std::endl;
        exit(1);
    }

    receivePeersInfo(result);
}

void P2PClient::receivePeersInfo(std::vector<PeerInfo>& result) const
{
    /** sleep in order to give main server time to answer */
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    size_t received;
    char buffer[BUFFERSIZE_W], *buff_ptr;
    received = recv(buffer, BUFFERSIZE_W);
    if(received <= 0) {
        std::cerr << "No connection to main server(receiving peers info)" << std::endl;
        exit(1);
    }
    int number_of_peers = (int)(*buff_ptr);

    unsigned char ip_addr_part;
    unsigned int ip_addr;
    unsigned short port;

    int ip_addr_size = sizeof(ip_addr);
    int numb_of_peers_size = sizeof(number_of_peers);

    PeerInfo peer_info_tmp;

    for(int i = 0; i < number_of_peers; ++i) {
        std::stringstream strstream;
        int offset = numb_of_peers_size + i;
        ip_addr = (unsigned int)(*(buff_ptr + offset));
        for(int i = 0; i < 4; ++i) {
            unsigned char* ip_addr_ptr = (unsigned char*)&ip_addr;
            ip_addr_part = (unsigned char)(*(ip_addr_ptr + i));
            strstream << ip_addr_part;
            if(i < 3) strstream << ".";
        }
        offset += ip_addr_size;
        port = (unsigned short)(*(buff_ptr + offset));
        strstream << "/" << port;

        peer_info_tmp.address = strstream.str();
        peer_info_tmp.connected = true;
        result.push_back(peer_info_tmp);
    }
}
