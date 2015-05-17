#include "FileClient.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <memory>
#include <sstream>

FileClient::FileClient() {

}

FileClient::~FileClient() {

}

uint64_t FileClient::send(
    const std::string & host,
    const unsigned short host_port,
    std::string & file_path,
    uint64_t from,
    uint64_t to)
{
    uint64_t data_length = to - from;

    if(data_length <= 0) {
        std::cerr << "Length is <= 0" << std::endl;
        return 0;
    }

    Socket host_socket;
    host_socket.connectTo(host, host_port);

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


//    std::unique_ptr<char[]> buffer(new char[data_length]);
    std::unique_ptr<char> buffer(new char);

    while(fread(buffer.get(), sizeof(char), 1, file_to_send)) {
        if(ferror(file_to_send)) {
            std::cerr << "Failed to read file" << std::endl;
            fclose(file_to_send);
            return 0;
        }

//        std::cout << buffer.get();

        int bytes_sent = write(host_socket.getFd(), buffer.get(), strlen(buffer.get()));

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

    fclose(file_to_send);
    return fileID;
}

bool FileClient::sendLength(const Socket& host_socket, uint64_t length) {

    int byte_sent = write(host_socket.getFd(), &length, sizeof(uint64_t));

    if(byte_sent == -1) {
        return false;
    }
    return true;
}

uint64_t FileClient::getFileID(const Socket& host_socket) {
    ClientInfo server_info;
    host_socket.listen();
    server_info = host_socket.accept();

    uint64_t file_id;
    int file_read = read(server_info.sock_fd, &file_id, sizeof(uint64_t));

    if(file_read == -1) {
        std::cerr << "Failed to read from socket" << std::endl;
        return 0;
    }

    return file_id;
}

std::pair<std::string, unsigned short> FileClient::getHostAndPort(const std::string& host) {
    std::vector<std::string> hostAndPort = split(host, ":");

    std::pair<std::string, unsigned short> host_port;

    host_port.first = hostAndPort[0];
    host_port.second = strtoul(hostAndPort[1].c_str(), nullptr, 10);

    return host_port;
}

std::vector<std::string> FileClient::split(std::string str, const char* delimiter) {
    std::vector<std::string> internal;
    std::stringstream ss(str);
    std::string tok;

    while(getline(ss, tok, delimiter[0])) {
        internal.push_back(tok);
    }

    return internal;
}

