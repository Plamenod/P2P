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
FileServer::FileServer(int port) : buffer(unique_ptr<char[]>(new char[SIZE_BUFFER])) {

    fd = fopen("test.txt", "ab+");

    if(fd == NULL)
    {
        std::cerr << "can't open the file" << std::endl;
        exit(1);
    }

    isBind = false;
    this->port = port;
    file_size = fseek(fd, 0, SEEK_END);
    memset(buffer.get(), 0, SIZE_BUFFER);
}

FileServer::~FileServer() {
    fclose(fd);
}


int FileServer::listen(int host_port) {

    socket.makeNonblocking();
    socket.bindTo(host_port);
    ::listen(socket, MAX_LENGTH_OF_QUEUE_PANDING);

    return accept(socket, nullptr, nullptr);
}


bool FileServer::receive() {

    if(!isBind) {

        connection_fd = listen(port);
        isBind = true;
    }

    if (connection_fd < 0) {
        cerr << "ERROR on accept\n";
        return false;
        //exit(1);  // TODO remove it
    }

    if(event_type(connection_fd) != 1) {

        if(initial_append(connection_fd)) {
            cout << "Size of the file is zero!!!\n";
            /*TODO need to try to receive it again*/
            exit(1);
        }
        append_to_file(connection_fd);

    } else {

        send_file_to_client(connection_fd);
    }
    return true/*TODO*/;
}

bool FileServer::recieve_size_of_file(int connection_fd) {

    uint64_t received_bytes = ::recv(
        connection_fd,
        reinterpret_cast<char *>(&info.size_of_file),
        sizeof(uint64_t),
        0);

    if(received_bytes) {
        info.id = next_free_id++;
        all_ids.push_back(next_free_id);
    }

    return received_bytes;
}

uint64_t FileServer::initial_append(int connection_fd) {

    uint64_t written_bytes = 0;
    if (recieve_size_of_file(connection_fd)) {

        written_bytes = fwrite(&info, sizeof(InfoData), 1, fd);
    }

    return written_bytes;
}

int FileServer::append_to_file(int connection_fd) {


    int read_bytes = -1;
    uint64_t size_of_file = info.size_of_file;
    while(size_of_file) {

        read_bytes = ::recv(connection_fd, buffer.get(), SIZE_BUFFER, 0);

        fwrite(buffer.get(), sizeof(char), read_bytes, fd);

        size_of_file -= read_bytes;
        cout << size_of_file << endl;

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

uint64_t FileServer::get_id_by_client(int connection_fd) {
    uint64_t id;
    ::recv(connection_fd, reinterpret_cast<char*>(&id), sizeof(uint64_t), 0);
    return id;
}

void FileServer::send_file_to_client(int newfd) {

    uint64_t id = get_id_by_client(newfd);

    uint64_t bytes_to_transfer = seek_2_file(id);

    if(bytes_to_transfer == 0) {
        cout << "ERROR: you are trying to get file with wrong id!!!\n";
    }

    ::send(newfd, reinterpret_cast<const char*> (&bytes_to_transfer), sizeof(uint64_t), 0);
    printf(" will send %d bytes!!!\n", static_cast<int>(bytes_to_transfer));

    while(bytes_to_transfer > 0) {

        int k = fread(buffer.get(), sizeof(char), 55, this->fd);
        if ( ferror(fd) ) {
                cout << "ERROR";
            }
            else if ( feof(fd) ) {
                cout << "EOF" << endl;
            }
        bytes_to_transfer -= k;
        int s = ::send(newfd, buffer.get(), k, 0);
        if(k != s) {
            cout << "Wrong k and s \n";
        }

        cout << k << ' ' << s << ' ' << bytes_to_transfer << endl;
    }
}

uint64_t FileServer::seek_2_file(uint64_t id) {

    if(!all_ids.size()) {
        return 0;
    }

    InfoData data;
    uint64_t sz = file_size;
    fseek(fd, 0, SEEK_SET);

    while(sz > 0) {

        fread(reinterpret_cast<char*>(&data), sizeof(InfoData), 1, fd);
        if(id == data.id) {
            fseek(fd, sizeof(InfoData), SEEK_CUR);
            return data.size_of_file;
        }
        fseek(fd, data.size_of_file + sizeof(InfoData), SEEK_CUR);
        sz -= data.size_of_file + sizeof(InfoData);
    }

    return 0;
}

uint64_t FileServer::event_type(int connection_fd) {
    uint64_t type;
    ::recv( connection_fd, reinterpret_cast<char *>(&type), sizeof(uint64_t), 0);
    return type;
}

bool FileServer::doesFileExist() {

    return file_size;
}

std::vector<uint64_t> FileServer::get_all_ids() {

    return all_ids;
}

void FileServer::run() {
    isRun = true;

    if(doesFileExist()) {
        recover_server();
    }

    while(isRun) {
        receive();
    }

}

int FileServer::getPort() {

    return port;
}
void FileServer::setPort(int new_port) {
    port = new_port;
}

void FileServer::recover_server() {

    uint64_t file_sz = file_size;
    uint64_t read_bytes;
    InfoData data;

    fseek(fd, 0, SEEK_SET);

    while(file_sz) {

        read_bytes = fread(reinterpret_cast<char*>(&data), sizeof(InfoData), 1, fd);

        if(read_bytes != sizeof(InfoData)) {
            cout << "Can't read InfoData structure !!!\n";
            exit(1);
        }

        fseek(fd, data.size_of_file + sizeof(InfoData), SEEK_CUR);

        file_size -= data.size_of_file + sizeof(InfoData);

        all_ids.push_back(data.id);
    }
    set_next_free_id();
}

void FileServer::set_next_free_id() {

    uint64_t max_id = all_ids[0];

    for(auto& iter : all_ids) {
        if(max_id < iter) {
            max_id = iter;
        }
    }
    next_free_id = ++max_id;
}
void FileServer::stop()
{
    isRun = false;
}


