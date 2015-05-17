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
FileServer::FileServer() {
	// TODO Auto-generated constructor stub

}

bool FileServer::receive(unsigned short host_port){

   // char buffer [256]; // TODO remove it as a member

    Socket socket;
    socket.bindTo(host_port);

    listen(socket, MAX_LENGTH_OF_QUEUE_PANDING);

    socklen_t clilen;
    sockaddr_in cli_addr;

    int  newsockfd = accept( socket, (struct sockaddr *) &cli_addr, &clilen);

    if (newsockfd < 0) {
        std::cerr << "ERROR on accept\n";
        exit(1);  // TODO remove it
    }
    bzero(buffer, SIZE);
    int n = -1;
     while(n = read(newsockfd,buffer,SIZE - 1))
     {
        std::cout << "Super Lubiiii "<< n << std::endl;
        printf("Here is the message: %s\n",buffer);
        scanf("%s", buffer);
        n = write(newsockfd,buffer,18);
        printf("bytes %d\n", n);
        if (n < 0) std::cerr << "ERROR writing to socket";
     }
    std::cout << "Lubiiii "<< n << std::endl;
    return false/*TODO*/;
}

