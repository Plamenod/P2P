#include "FileServer.h"
#include "socket.h"

#ifndef C_WIN_SOCK
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>


#define MAX_LENGTH_OF_QUEUE_PANDING 5

using namespace std;
FileServer::FileServer() : buffer(unique_ptr<char[]>(new char[SIZE_BUFFER])) {

    fd = fopen("/home/plamen/workspace/server_project/bin/Debug/data_file.dat", "rb+");

    if(fd == NULL)
    {
        std::cerr << "can't open the file" << std::endl;
        exit(1);
    }

    memset(buffer.get(), 0, SIZE_BUFFER);
    info.id = 66; // fake id, it's only for testing
    info.size_of_file = 3048;
}

FileServer::~FileServer() {
    fclose(fd);
}


int FileServer::listen(int host_port) {

    socket.bindTo(host_port);
    ::listen(socket, MAX_LENGTH_OF_QUEUE_PANDING);

    return accept(socket, nullptr, nullptr);
}


bool FileServer::receive(unsigned short host_port) {

    int  connection_fd = listen(host_port);

    if (connection_fd < 0) {
        cerr << "ERROR on accept\n";
        exit(1);  // TODO remove it
    }

    if(event_type(connection_fd) != 1) {

        initial_append(connection_fd);
        append_to_file(connection_fd);

    } else {

        send_file_to_client(connection_fd);
    }
    return true/*TODO*/;
}

bool FileServer::recieve_size_of_file(int connection_fd) {

    return ::recv(connection_fd, reinterpret_cast<char *>(&info.size_of_file), sizeof(uint64_t), 0);
}

int FileServer::initial_append(int connection_fd) {

    recieve_size_of_file(connection_fd);

    return fwrite(&info, sizeof(InfoData), 1, fd);
}

int FileServer::append_to_file(int connection_fd) {


    int read_bytes = -1;
    uint64_t size_of_file = info.size_of_file;
    while(size_of_file) {

        read_bytes = ::recv(connection_fd, buffer.get(), SIZE_BUFFER, 0);

        fwrite(buffer.get(), sizeof(char), read_bytes, fd);

        size_of_file -= read_bytes;

    }
    ::send(connection_fd, reinterpret_cast<const char *>(&info.id), sizeof(uint64_t), 0);

    if (read_bytes < 0) {
        std::cerr << "ERROR writing to socket";
    }
    return false;
}

void FileServer::send_info_file_to_client(int newfd) {

    ::send(
        newfd,
        reinterpret_cast<const char *>(&info.size_of_file),
        sizeof(uint64_t),
        0);
}

void FileServer::send_file_to_client(int newfd) {

    send_info_file_to_client(newfd);

    uint64_t bytes_to_transfer = info.size_of_file;
    cout << bytes_to_transfer << endl;

    printf(" will send %d bytes!!!\n", bytes_to_transfer);
    fseek(fd, 0, SEEK_SET);
    while(bytes_to_transfer > 0) {

        int k = fread(buffer.get(), sizeof(char), 10, this->fd);
        bytes_to_transfer -= k;
        int s = ::send(newfd, buffer.get(), k, 0);

        cout << k << ' ' << s << ' ' << bytes_to_transfer << endl;
    }
}

uint64_t FileServer::event_type(int connection_fd) {
    uint64_t type;
    ::recv( connection_fd, reinterpret_cast<char *>(&type), sizeof(uint64_t), 0);
    return type;
}

