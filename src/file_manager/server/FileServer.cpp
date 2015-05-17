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
}

bool FileServer::receive(unsigned short host_port) {

    Socket socket;
    socket.bindTo(host_port);

    listen(socket, MAX_LENGTH_OF_QUEUE_PANDING);


    int  newsockfd = accept( socket, nullptr,nullptr);

    if (newsockfd < 0) {
        std::cerr << "ERROR on accept\n";
        exit(1);  // TODO remove it
    }

    bzero(buffer.get(), SIZE_BUFFER);

    int readed_bytes = -1;
    uint64_t size_of_file;

    read(newsockfd, &size_of_file, sizeof(uint64_t));

    printf("size_of_file: %ld\n", size_of_file);

    while(readed_bytes = read(newsockfd, buffer.get(), SIZE_BUFFER - 1)) {

        printf("Here is the message: %s\n", buffer.get());
        scanf("%s", buffer.get());
        readed_bytes = write(newsockfd, buffer.get(),SIZE_BUFFER);
        printf("bytes %d\n", readed_bytes);
     }

     if (readed_bytes < 0) {
        std::cerr << "ERROR writing to socket";
     }

    return true/*TODO*/;
}

