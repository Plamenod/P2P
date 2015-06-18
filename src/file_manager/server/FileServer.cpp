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
#include <chrono>
#include <thread>


#define MAX_LENGTH_OF_QUEUE_PANDING 5

using namespace std;
FileServer::FileServer(int port, string dbFilePath): dbFilePath(move(dbFilePath)), buffer(unique_ptr<char[]>(new char[SIZE_BUFFER]))
{

    fd = fopen(this->dbFilePath.c_str(), "ab+");

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
    /////////

    socket.makeNonblocking();
    //cout << "line 40\n";
    socket.bindTo(port);
    //cout << "line 42\n";
    ::listen(socket, MAX_LENGTH_OF_QUEUE_PANDING);
}

FileServer::~FileServer()
{
    fclose(fd);
}


int FileServer::listen() // TODO change the name
{
    return accept(socket, nullptr, nullptr);
}


bool FileServer::receive()
{
    connection = listen();

    if (connection < 0)
    {
        return false; // TODO not bool
    }

    if(eventType(connection) != 1)
    {
        if (initialAppend(connection)) {
            appendToFile(connection);
        }
    }

    else
    {
        sendFileToClient(connection);
    }


#ifdef C_WIN_SOCK
	closesocket(connection);
#else
	close(connection);
#endif

    return true/*TODO*/;
}

bool FileServer::recieveSizeOfFile(int connection)
{
    uint64_t receivedBytes;
    while(1)
    {
        //cout << "line: 95 - recieveSizeOfFile\n";
        receivedBytes = ::recv(
            connection,
            reinterpret_cast<char *>(&info.sizeOfFile),
            sizeof(uint64_t),
            0);
            if(receivedBytes != -1) break;
    }

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
        fclose(fd);

		fd = fopen(dbFilePath.c_str(), "ab+");
        if (!fd) {
            return 0;
        }

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

		if (readBytes == -1)
		{
		    //cout << "line 136\n";
			continue;
		}

        fwrite(buffer.get(), sizeof(char), readBytes, fd);

        sizeOfFile -= readBytes;
    }

    fileSize += info.sizeOfFile;
    while(1)
    {
        if(-1 != ::send(connection, reinterpret_cast<const char *>(&info.id), sizeof(uint64_t), 0))
        {
            break;
        }
        //cout << "Send line: 152\n";
    }

    if (readBytes < 0)
    {
        std::cerr << "ERROR writing to socket";
    }
    return false;
}

void FileServer::sendInfoFileToClient(int newfd)
{
    while(1)
    {
        if( -1 != ::send(
                        newfd,
                        reinterpret_cast<const char *>(&info.sizeOfFile),
                        sizeof(uint64_t),
                        0) )
        {
            break;
        }
        //cout << "Send line: 174\n";
    }
}

uint64_t FileServer::getIdByClient(int connection)
{
    uint64_t id;

	while(-1 == ::recv(connection, reinterpret_cast<char*>(&id), sizeof(uint64_t), 0))
    {
        //cout << "Send line: 184\n";
    }


    return id;
}

void FileServer::sendFileToClient(int newfd)
{

    uint64_t id = getIdByClient(newfd);

    uint64_t bytesToTransfer = seek2File(id);

    if(bytesToTransfer == 0)
    {
        cout << "ERROR: you are trying to get file with wrong id!!!\n";
        return;
    }

    while(-1 == ::send(newfd, reinterpret_cast<const char*>(&bytesToTransfer), sizeof(uint64_t), 0))
    {
        //cout << "Send line: 205\n";
    }



    while(bytesToTransfer > 0)
    {

        uint64_t readBytes = fread(buffer.get(), sizeof(char), SIZE_BUFFER, this->fd);

        if ( ferror(fd) )
        {
            cerr << "ERROR";
        }

        else if ( feof(fd) )
        {
            break;
        }

        else if(-1 == readBytes) continue;

        bytesToTransfer -= readBytes;
        while(readBytes > 0)
        {
            int sentBytes = ::send(newfd, buffer.get(), readBytes, 0);
            if(sentBytes == -1) continue;

            readBytes -= sentBytes;
        }

        if(bytesToTransfer > 0 && bytesToTransfer < SIZE_BUFFER) // TODO
        {
            fread(buffer.get(), sizeof(char), bytesToTransfer, fd);
            ::send(newfd, buffer.get(), bytesToTransfer, 0);
            break;
        }
    }
}

uint64_t FileServer::seek2File(uint64_t id)
{
    InfoData data;
    uint64_t currentSize = fileSize;

    fclose(fd);
	fd = fopen(dbFilePath.c_str(), "ab+");
    fseek(fd, 0, SEEK_SET);

    while(currentSize > 0)
    {
        auto read = fread(reinterpret_cast<char*>(&data), sizeof(InfoData), 1, fd);
        if (read != 1) {
            perror("Failed to read InfoData");
            return 0;
        }

        if(id == data.id)
        {
            return data.sizeOfFile;
        }

        auto seek = fseek(fd, data.sizeOfFile, SEEK_CUR);

        currentSize -= data.sizeOfFile + sizeof(InfoData);
    }

    return 0;
}

uint64_t FileServer::eventType(int connection)
{
    uint64_t type;

    while(-1 == ::recv( connection, reinterpret_cast<char *>(&type), sizeof(uint64_t), 0))
    {
        //cout << "Send line: 275\n";
    }

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
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
        if(!readBytes)
        {
            cout << "Can't read InfoData structure !!!\n";
            exit(1);
        }

        fseek(fd, data.sizeOfFile, SEEK_CUR);

        file_sz -= data.sizeOfFile + sizeof(InfoData);

        all_ids.push_back(data.id);
    }
    setNextFreeId();
}

void FileServer::setNextFreeId()
{
    if(!all_ids.size())
    {
        nextFreeId = 1;
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

