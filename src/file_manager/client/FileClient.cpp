#include "FileClient.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <memory>

FileClient::FileClient() {

}

FileClient::~FileClient() {

}

bool FileClient::send(
    const std::string & host,
    const unsigned short host_port,
    std::string & file_path,
    uint64_t from,
    uint64_t to)
{
    uint64_t data_length = to - from;

    if(data_length <= 0) {
        std::cerr << "Length is <= 0" << std::endl;
        return false;
    }

    Socket host_socket;
    host_socket.connectTo(host, host_port);

    sendLength(host_socket, data_length);

    FILE* file_to_send;
    file_to_send = fopen(file_path.c_str(), "r");

    if(!file_to_send) {
        std::cerr << "Failed to open file to send" << std::endl;
        fclose(file_to_send);
        return false;
    }

    if(fseek(file_to_send, from, SEEK_SET)) {
        std::cerr << "Failed to seek to position " << from << std::endl;
        fclose(file_to_send);
        return false;
    }

    std::unique_ptr<char[]> buffer(new char[data_length]);

    fread(buffer.get(), sizeof(char), data_length, file_to_send);

    if(ferror(file_to_send)) {
        std::cerr << "Failed to read file" << std::endl;
        fclose(file_to_send);
        return false;
    }

    int bytes_sent = write(host_socket.getFd(), buffer.get(), strlen(buffer.get()));

    if(bytes_sent == -1) {
        std::cerr << "Failed to write to socket" << std::endl;
        fclose(file_to_send);
        return false;
    }

    fclose(file_to_send);
    return true;
}

bool FileClient::sendLength(const Socket& host_socket, uint64_t length) {

    int byte_sent = write(host_socket.getFd(), &length, sizeof(uint64_t));

    if(byte_sent == -1) {
        std::cerr << "Failed to send file of length - " << length << std::endl;
        return false;
    }
    return true;
}
