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

    info.size_of_file = 0;
    fd = fopen("data_file.dat", "ab+");
    if(fd == NULL)
    {
        std::cerr << "can't open the file" << std::endl;
        exit(1);
    }
    memset(buffer.get(), 0, SIZE_BUFFER);
    info.id = 100; // fake id, it's only for testing
}

FileServer::~FileServer() {
    fclose(fd);
}

bool FileServer::receive(unsigned short host_port) {

    socket.bindTo(host_port);
    listen(socket, MAX_LENGTH_OF_QUEUE_PANDING);
    int  newsockfd = accept(socket, nullptr, nullptr);

    if (newsockfd < 0) {
        std::cerr << "ERROR on accept\n";
        exit(1);  // TODO remove it
    }

    initial_append(newsockfd);
    append_to_file(newsockfd);

    return true/*TODO*/;
}

/*you have to have the id before use it*/
bool FileServer::initial_append(int newsockfd) {

    // get size of the file from the client
    ::recv(newsockfd, reinterpret_cast<char *>(&info.size_of_file), sizeof(uint64_t), 0);
    printf("size_of_file: %ld\n", info.size_of_file);

    return fwrite(&info, sizeof(InfoData), 1, fd);
}

bool FileServer::append_to_file(int newsockfd) {


    int read_bytes = -1;

    while(info.size_of_file) {

        read_bytes = ::recv(newsockfd, buffer.get(), SIZE_BUFFER - 1, 0);
        printf("Here is the message: %s\n", buffer.get());
        printf("bytes %d\n", read_bytes);

        int k = fwrite(buffer.get(), sizeof(char), read_bytes, fd);

        info.size_of_file -= read_bytes;
        memset(buffer.get(), 0, SIZE_BUFFER);

    }
    ::send(newsockfd, reinterpret_cast<const char *>(&info.id), sizeof(uint64_t), 0);

    if (read_bytes < 0) {
        std::cerr << "ERROR writing to socket";
    }
    return false;
}
