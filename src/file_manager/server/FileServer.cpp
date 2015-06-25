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
    this->socket.listen();
}

FileServer::~FileServer()
{
    fclose(fd);
}


Socket FileServer::acceptClient()
{
    return this->socket.acceptSocket();
}


bool FileServer::receive()
{
    Socket connection = acceptClient();

    if (connection < 0)
    {
        return true; // no client - all is ok
    }

    bool result = false;
    auto etype = eventType(connection);
    switch (etype)
    {
    case 0:
        if (initialAppend(connection)) {
            result = appendToFile(connection);
        }
        break;
    case 1:
        result = sendFileToClient(connection);
        break;
    }

    return result;
}

bool FileServer::recieveSizeOfFile(Socket & connection)
{
    int receivedBytes = -1;
    int retries = 100;
    while(retries && receivedBytes <= 0)
    {
        //cout << "line: 95 - recieveSizeOfFile\n";
        receivedBytes = connection.recv(&info.sizeOfFile, sizeof(uint64_t));

        if (receivedBytes == 0)
        {
            cout << "Connection to remote client closed" << endl;
            return false;
        }
        else if (receivedBytes == -1)
        {
            this_thread::sleep_for(chrono::milliseconds(1)); // give client time to receive size
            --retries;
        }
    }

    if (!retries) {
        cout << "Failed to receive size of file" << endl;
        return false;
    }

    if(receivedBytes)
    {
        info.id = nextFreeId++;
        all_ids.push_back(nextFreeId);
    }

    return receivedBytes;
}

bool FileServer::initialAppend(Socket & connection)
{

    int writtenBytes = 0;
    if (recieveSizeOfFile(connection))
    {
        fclose(fd);

		fd = fopen(dbFilePath.c_str(), "ab+");
        if (!fd) {
            return 0;
        }

        writtenBytes = fwrite(&info, sizeof(InfoData), 1, fd);
    }

    return writtenBytes == 1;
}

bool FileServer::appendToFile(Socket & connection)
{
    int readBytes = -1;
    uint64_t sizeOfFile = info.sizeOfFile;
    int retries = 100;
    while(sizeOfFile && retries)
    {
        readBytes = connection.recv(buffer.get(), SIZE_BUFFER);

		if (readBytes == -1)
		{
            --retries;
            this_thread::sleep_for(chrono::milliseconds(1)); // give it time to receive data
		    //cout << "line 136\n";
			continue;
        } else if (readBytes == 0)
        {
            cout << "Connection to client closed while receiveing file" << endl;
            return false;
        }

        fwrite(buffer.get(), sizeof(char), readBytes, fd);
        if (sizeOfFile < readBytes) {
            cout << "Failed to append to file - wrong file size - would inf loop" << endl;
            return false;
        }
        sizeOfFile -= readBytes;
    }

    fileSize += info.sizeOfFile;
    retries = 100;
    while(--retries)
    {
        if(sizeof(uint64_t) == connection.send(&info.id, sizeof(uint64_t)))
        {
            break;
        }
        //cout << "Send line: 152\n";
        this_thread::sleep_for(chrono::milliseconds(1)); // give client time to receive id
    }

    if (!retries) {
        std::cerr << "Failed to send file ID to client" << endl;
        return false;
    }

    if (readBytes < 0)
    {
        std::cerr << "ERROR writing to socket";
        return false;
    }
    return true;
}

bool FileServer::sendInfoFileToClient(Socket & connection)
{
    int retries = 100;
    while(--retries)
    {
        if (sizeof(uint64_t) == connection.send(&info.sizeOfFile, sizeof(uint64_t)))
        {
            return true;
        }
        this_thread::sleep_for(chrono::milliseconds(1)); // give client time to receive size
        //cout << "Send line: 174\n";
    }
    return false;
}

uint64_t FileServer::getIdByClient(Socket & connection)
{
    uint64_t id = 0;

    int retries = 100;
	while(--retries && sizeof(uint64_t) != connection.recv(&id, sizeof(uint64_t)))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    return id;
}

bool FileServer::sendFileToClient(Socket & connection)
{

    uint64_t id = getIdByClient(connection);

    uint64_t bytesToTransfer = seek2File(id);

    if(bytesToTransfer == 0)
    {
        cout << "ERROR: you are trying to get file with wrong id!!!\n";
        return false;
    }

    int retries = 100;
    while (--retries && sizeof(uint64_t) != connection.send(&bytesToTransfer, sizeof(uint64_t)))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        //cout << "Send line: 205\n";
    }

    if (!retries) {
        return false;
    }



    while(bytesToTransfer > 0)
    {

        int readBytes = fread(buffer.get(), sizeof(char), SIZE_BUFFER, this->fd);

        if ( ferror(fd) || readBytes < 0 )
        {
            cout << "Failed to read from db file" << endl;
            return false;
        }
        else if ( feof(fd) )
        {
            break;
        }
        else if (-1 == readBytes)
        {
            continue;
        }

        if (bytesToTransfer < readBytes) {
            cout << "Wrong sent size - would go into inf loop" << endl;
            return false;
        }
        bytesToTransfer -= readBytes;


        while(readBytes > 0)
        {
            int sentBytes = connection.send(buffer.get(), readBytes);
            if (sentBytes == -1)
            {
                continue;
            }

            if (readBytes < sentBytes)
            {
                cout << "Failed to send bytes from db to client" << endl;
                return false;
            }
            readBytes -= sentBytes;
        }

        if(bytesToTransfer > 0 && bytesToTransfer < SIZE_BUFFER) // TODO
        {
            fread(buffer.get(), sizeof(char), bytesToTransfer, fd);
            connection.send(buffer.get(), bytesToTransfer);
            break;
        }
    }

    return true;
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

        currentSize -= sizeof(InfoData);

        if(id == data.id)
        {
            return data.sizeOfFile;
        }

        auto seek = fseek(fd, data.sizeOfFile, SEEK_CUR);

        if (currentSize < data.sizeOfFile) {
            cout << "Failed to read data structure from db" << endl;
            return 0;
        }

        currentSize -= data.sizeOfFile;
    }

    return 0;
}

uint64_t FileServer::eventType(Socket & connection)
{
    uint64_t type = -1;
    int retries = 100;
    while(--retries && sizeof(uint64_t) != connection.recv(&type, sizeof(uint64_t)))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // give the client time to send type
        //cout << "Send line: 275\n";
    }

    if (!retries) {
        cout << "Failed to receive event type from client" << endl;
        return -1;
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
        if (!receive()) {
            cout << "Failed to server user action" << endl;
        }
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
        if(readBytes != 1)
        {
            cout << "Can't read InfoData structure - failed read" << endl;
            exit(1);
        }

        file_sz -= sizeof(InfoData);

        fseek(fd, data.sizeOfFile, SEEK_CUR);

        if (file_sz < data.sizeOfFile) {
            cout << "Can't read InfoData structure - broken file" << endl;
            exit(1);
        }

        file_sz -= data.sizeOfFile;
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

