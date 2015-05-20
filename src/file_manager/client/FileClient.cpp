#include "FileClient.h"
#include "../util.h"
#include <iostream>
#include <cstring>
#include <stdint.h>
#include <memory>
#include <sstream>

#define MAX_SIZE 100

FileClient::FileClient() {

}

FileClient::~FileClient() {

}

uint64_t FileClient::send(
    const std::string & host,
    std::string & file_path,
    uint64_t from,
    uint64_t to)
{
    uint64_t data_length = to - from;

    if(data_length <= 0) {
        std::cerr << "Length is <= 0" << std::endl;
        return 0;
    }

    std::string ip = getHost(host);
    unsigned short host_port = getPort(host);

    Socket host_socket;
    host_socket.connectTo(ip, host_port);

    if(!sendLength(host_socket, data_length)) {
        std::cerr << "Failed to send file of length - " << data_length << std::endl;
    }

    FILE* file_to_send;
    file_to_send = fopen(file_path.c_str(), "r");

    if(!file_to_send) {
        std::cerr << "Failed to open file to send" << std::endl;
        fclose(file_to_send);
        return 0;
    }

    if(fseek(file_to_send, from, SEEK_SET)) {
        std::cerr << "Failed to seek to position " << from << std::endl;
        fclose(file_to_send);
        return 0;
    }


    std::unique_ptr<char[]> buffer(new char[MAX_SIZE + 1]);
//    std::unique_ptr<char> buffer(new char);

    while(data_length) {
        fread(buffer.get(), sizeof(char), MAX_SIZE, file_to_send);
        data_length -= MAX_SIZE;

        if(ferror(file_to_send)) {
            std::cerr << "Failed to read file content" << std::endl;
            fclose(file_to_send);
            return 0;
        }

//        std::cout << buffer.get();

        int bytes_sent = ::send(host_socket.getFd(), buffer.get(), strlen(buffer.get()), 0);

        if(bytes_sent == -1) {
            std::cerr << "Failed to write to socket" << std::endl;
            fclose(file_to_send);
            return 0;
        }

        if(feof(file_to_send)) {
            break;
        }
    }

    uint64_t fileID = getFileID(host_socket);
//    std::cout << "FileID: " << fileID << std::endl;

    fclose(file_to_send);
    return fileID;
}

bool FileClient::sendLength(const Socket& host_socket, uint64_t length) {

    int byte_sent = ::send(host_socket.getFd(), reinterpret_cast<const char *>(&length), sizeof(uint64_t), 0);

    if(byte_sent == -1) {
        return false;
    }
    return true;
}

uint64_t FileClient::getFileID(const Socket& host_socket) {
    uint64_t file_id;
    int file_read = ::recv(host_socket.getFd(), reinterpret_cast<char *>(&file_id), sizeof(uint64_t), 0);

    if(file_read == -1) {
        std::cerr << "Failed to read from socket" << std::endl;
        return 0;
    }

    return file_id;
}


