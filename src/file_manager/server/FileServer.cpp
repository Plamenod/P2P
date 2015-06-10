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
FileServer::FileServer(int port) : buffer(unique_ptr<char[]>(new char[SIZE_BUFFER]))
{

    fd = fopen("test.txt", "ab+");

    if(fd == NULL)
    {
        cerr << "can't open the file" << endl;
        exit(1);
    }

    isBind = false;
    this->port = port;
    fseek(fd, 0, SEEK_END);
    fileSize = ftell(fd);
    memset(buffer.get(), 0, SIZE_BUFFER);
    recoverServer();
}

FileServer::~FileServer()
{
    fclose(fd);
}


int FileServer::listen(int hostPort, Socket& socket)
{
  //  socket.makeNonblocking();
    socket.bindTo(hostPort);
    ::listen(socket, MAX_LENGTH_OF_QUEUE_PANDING);

    return accept(socket, nullptr, nullptr);
}


bool FileServer::receive()
{
    Socket socket;
    connection = listen(port, socket);
    cout << "listening\n" << connection;


    if (connection < 0)
    {
        cerr << "ERROR on accept\n";
        return false;
        exit(1);  // TODO remove it
    }

    if(eventType(connection) != 1)
    {
        initialAppend(connection);
        appendToFile(connection);
    }

    else
    {
        sendFileToClient(connection);
    }

#ifdef C_WIN_SOCK
	closesocket(socket);
#else
	close(socket);
#endif
    return true/*TODO*/;
}

bool FileServer::recieveSizeOfFile(int connection)
{

    uint64_t receivedBytes = ::recv(
        connection,
        reinterpret_cast<char *>(&info.sizeOfFile),
        sizeof(uint64_t),
        0);

    if(receivedBytes)
    {
        info.id = nextFreeId++;
        all_ids.push_back(nextFreeId);
    }

    return receivedBytes;
}

uint64_t FileServer::initialAppend(int connection)
{

    uint64_t writtenBytes = 0;
    if (recieveSizeOfFile(connection))
    {
        writtenBytes = fwrite(&info, sizeof(InfoData), 1, fd);
    }

    return writtenBytes;
}

int FileServer::appendToFile(int connection)
{
    int readBytes = -1;
    uint64_t sizeOfFile = info.sizeOfFile;

    while(sizeOfFile)
    {
        readBytes = ::recv(connection, buffer.get(), SIZE_BUFFER, 0);

        fwrite(buffer.get(), sizeof(char), readBytes, fd);

        sizeOfFile -= readBytes;
        cout << sizeOfFile << endl;

    }

    fileSize += info.sizeOfFile;

    ::send(connection, reinterpret_cast<const char *>(&info.id), sizeof(uint64_t), 0);

    if (readBytes < 0)
    {
        std::cerr << "ERROR writing to socket";
    }
    return false;
}

void FileServer::sendInfoFileToClient(int newfd)
{
    ::send(
        newfd,
        reinterpret_cast<const char *>(&info.sizeOfFile),
        sizeof(uint64_t),
        0);
}

uint64_t FileServer::getIdByClient(int connection)
{
    uint64_t id;

    ::recv(connection, reinterpret_cast<char*>(&id), sizeof(uint64_t), 0);

    return id;
}

void FileServer::sendFileToClient(int newfd)
{

    uint64_t id = getIdByClient(newfd);

    uint64_t bytesToTransfer = seek2File(id);

    if(bytesToTransfer == 0)
    {
        cout << "ERROR: you are trying to get file with wrong id!!!\n";
    }

    ::send(newfd, reinterpret_cast<const char*> (&bytesToTransfer), sizeof(uint64_t), 0);
    printf(" will send %d bytes!!!\n", static_cast<int>(bytesToTransfer));

    while(bytesToTransfer > 0)
    {

        int k = fread(buffer.get(), sizeof(char), 55, this->fd);

        if ( ferror(fd) )
        {
            cout << "ERROR";
        }

        else if ( feof(fd) )
        {
            cout << "EOF" << endl;
            break;
        }

        bytesToTransfer -= k;

        int s = ::send(newfd, buffer.get(), k, 0);

        if(k != s)
        {
            cout << "Wrong k and s \n";
        }

        if(bytesToTransfer > 0 && bytesToTransfer < 55)
        {
            fread(buffer.get(), sizeof(char), bytesToTransfer, this->fd);
            s =  ::send(newfd, buffer.get(), bytesToTransfer, 0);
            cout << k << ' ' << s << ' ' << bytesToTransfer << endl;
            break;
        }
        cout << k << ' ' << s << ' ' << bytesToTransfer << endl;
    }
}

uint64_t FileServer::seek2File(uint64_t id)
{
    InfoData data;
    uint64_t sz = fileSize;

    fseek(fd, 0, SEEK_SET);

    while(sz > 0)
    {
        int b = fread(reinterpret_cast<char*>(&data), sizeof(InfoData), 1, fd);

        if(id == data.id)
        {
            return data.sizeOfFile;
        }

        fseek(fd, data.sizeOfFile /*+ sizeof(InfoData)*/, SEEK_CUR);

        sz -= data.sizeOfFile + sizeof(InfoData);
    }

    return 0;
}

uint64_t FileServer::eventType(int connection)
{
    uint64_t type;

    ::recv( connection, reinterpret_cast<char *>(&type), sizeof(uint64_t), 0);

    return type;
}

bool FileServer::doesFileExist()
{
    return fileSize;
}

std::vector<uint64_t> FileServer::getAllIds()
{
    return all_ids;
}

void FileServer::run()
{
    isRun = true;

    if(doesFileExist())
    {
        recoverServer();
    }

    while(isRun)
    {
        receive();
    }

}

int FileServer::getPort()
{
    return port;
}

void FileServer::setPort(int newPort)
{
    port = newPort;
}

void FileServer::recoverServer()
{
    uint64_t file_sz = fileSize;
    uint64_t readBytes;
    InfoData data;

    fseek(fd, 0, SEEK_SET);

    while(file_sz)
    {

        readBytes = fread(reinterpret_cast<char*>(&data), sizeof(InfoData), 1, fd);

        int t = sizeof(InfoData);
        if(!readBytes)
        {
            cout << "Can't read InfoData structure !!!\n";
            exit(1);
        }

        fseek(fd, data.sizeOfFile /*+ sizeof(InfoData)*/, SEEK_CUR);

        file_sz -= data.sizeOfFile + sizeof(InfoData);

        all_ids.push_back(data.id);

        cout << " ----------- " <<file_sz << " \n";
    }
    set_nextFreeId();
}

void FileServer::set_nextFreeId()
{
    if(!all_ids.size())
    {
        nextFreeId = 0;
        return;
    }

    uint64_t max_id = all_ids[0];

    for(auto& iter : all_ids)
    {
        if(max_id < iter)
        {
            max_id = iter;
        }
    }
    nextFreeId = ++max_id;
}
void FileServer::stop()
{
    isRun = false;
}

void FileServer::openFile()
{

}

void FileServer::closeFile()
{

}

