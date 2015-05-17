/*
 * FileServer.cpp
 *
 *  Created on: May 16, 2015
 *      Author: plamen
 */

#include "FileServer.h"
#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>


#define MAX_LENGTH_OF_QUEUE_PANDING 5

using namespace std;
FileServer::FileServer() : buffer(unique_ptr<char[]>( new char[SIZE_BUFFER])) {

    info.size_of_file = 0;
    fd = fopen("tmp.dat", "ab+");
    if(fd == NULL)
    {
        std::cerr << "can't open the file" << std::endl;
        exit(1);
    }
    bzero(buffer.get(), SIZE_BUFFER);
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


    int readed_bytes = -1;
    while(readed_bytes = read(newsockfd, buffer.get(), SIZE_BUFFER - 1)) {

        printf("Here is the message: %s\n", buffer.get());


        scanf("%s", buffer.get());
        readed_bytes = write(newsockfd, buffer.get(),SIZE_BUFFER);
        printf("bytes %d\n", readed_bytes);

        bzero(buffer.get(), SIZE_BUFFER); // TODO remove to append_to_file, after appended it
     }

     if (readed_bytes < 0) {
        std::cerr << "ERROR writing to socket";
     }
    return true/*TODO*/;
}

/*you have to have the id before use it*/
bool FileServer::initial_append(int newsockfd) {
    // get size of the file from the client
    read(newsockfd, &info.size_of_file, sizeof(uint64_t));
    printf("size_of_file: %ld\n", info.size_of_file);

    return fwrite(&info, sizeof(InfoData), 1, fd);
}

bool FileServer::append_to_file() {
    return false;
}
