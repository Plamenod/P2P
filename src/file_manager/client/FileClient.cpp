#include "FileClient.h"
#include "../util.h"
#include <iostream>
#include <cstring>
#include <stdint.h>
#include <sstream>
#include <algorithm>
//#include "Encryption.h"

#define MAX_SIZE 4000
#define BYTE_TO_RECEIVE 100
#define KEY_LENGTH 5

FileClient::FileClient() : connected(false){

}

FileClient::~FileClient() {

}

uint64_t FileClient::send(
    const std::string & host,
    const std::string & file_path,
    uint64_t from,
    uint64_t to)
{
    uint64_t data_length = to - from;
	Socket host_socket;

    if (data_length <= 0) {
        std::cerr << "Length is <= 0" << std::endl;
        return 0;
    }

    std::string ip = getHost(host);
    unsigned short host_port = getPort(host);

    //if (!connected) {
        host_socket.connectTo(ip, host_port);
        connected = true;
    //}

    uint64_t send_file_event = 0;
    if (!sendNumber(host_socket, send_file_event)) {
        std::cerr << "Failed to send event" << std::endl;
        return 0;
    }

    if (!sendNumber(host_socket, data_length)) {
        std::cerr << "Failed to send file of length - " << data_length << std::endl;
        return 0;
    }

    FILE* file_to_send;
    file_to_send = fopen(file_path.c_str(), "rb");

    if (!file_to_send) {
        std::cerr << "Failed to open file to send" << std::endl;
        fclose(file_to_send);
        return 0;
    }

    if (fseek(file_to_send, from, SEEK_SET)) {
        std::cerr << "Failed to seek to position " << from << std::endl;
        fclose(file_to_send);
        return 0;
    }

	Encryption cryptor = Encryption::getRandomEncryption();
	if (!cryptor) {
		std::cerr << "Failed to create encryption key" << std::endl;
		return 0;
	}

    std::unique_ptr<char[]> buffer(new char[MAX_SIZE + 1]);

    while (data_length) {
        int byte_read = fread(buffer.get(), sizeof(char), std::min<int>(MAX_SIZE, data_length), file_to_send);
        data_length -= byte_read;

        if (ferror(file_to_send)) {
            std::cerr << "Failed to read file content" << std::endl;
            fclose(file_to_send);
            return 0;
        }

		cryptor.encryptDecrypt(buffer.get(), byte_read);

		int bytes_sent = -1;
		while (bytes_sent == -1) {
			bytes_sent = ::send(host_socket.getFd(), buffer.get(), byte_read, 0);
		}

        if (bytes_sent == -1) {
            std::cerr << "Failed to write to socket" << std::endl;
            fclose(file_to_send);
            return 0;
        }
    }

	uint64_t fileID = getFileID(host_socket);

	if (!writeKeyToFile(cryptor.getSave(fileID))) {
		std::cerr << "Failed to save encrytion key for file" << std::endl;
		return 0;
	}

    fclose(file_to_send);
    return fileID;
}

std::unique_ptr<char[]> FileClient::getFile(const std::string& host, uint64_t id) {
    std::string ip = getHost(host);
    unsigned short port = getPort(host);

	Socket host_socket;

    //if (!connected) {
        host_socket.connectTo(ip, port);
    //}

    uint64_t request_file_event = 1;
    if (!sendNumber(host_socket, request_file_event)) {
        std::cerr << "Failed to send event" << std::endl;
        return nullptr;
    }

    if (!sendNumber(host_socket, id)) {
        std::cerr << "Failed to send file ID" << std::endl;
        return nullptr;
    }

    uint64_t file_size;
    if (::recv(host_socket.getFd(), reinterpret_cast<char*>(&file_size), sizeof(uint64_t), 0) != sizeof(uint64_t)) {
        std::cerr << "Failed to receive file size" << std::endl;
        return nullptr;
    }

    auto encrypt = getKeyFromId(id);

	if (!encrypt) {
    	std::cerr << "Failed to get key to encrypt" << std::endl;
		return nullptr;
    }
	std::unique_ptr<char[]> file_content(new char[file_size]);

    int offset = 0;
    while (file_size > 0) {

		int byte_read = -1;
		while (byte_read == -1) {
			byte_read = ::recv(
				host_socket.getFd(),
				reinterpret_cast<char*>(file_content.get() + offset),
				file_size,
				0);

            if (byte_read == 0) {
                std::cerr << "Remote socket closed during receiveing of file" << std::endl;
                return nullptr;
            }
		}
		encrypt.encryptDecrypt(file_content.get() + offset, byte_read);
        //printf("byte read: %d \n Message: %s\nfile size: %lu\n", byte_read, file_content.get(), file_size);

        file_size -= byte_read;
        offset += byte_read;
    }

    return std::move(file_content);
}

bool FileClient::sendNumber(const Socket& host_socket, uint64_t number) {

    int byte_sent = ::send(
        host_socket.getFd(),
        reinterpret_cast<const char *>(&number),
        sizeof(number),
        0);

    if (byte_sent != sizeof(number)) {
        return false;
    }
    return true;
}

uint64_t FileClient::getFileID(const Socket& host_socket) {
	uint64_t file_id = 0;
    //int file_read = ::recv(host_socket.getFd(), reinterpret_cast<char *>(&file_id), sizeof(uint64_t), 0);
	int file_read = -1;
	while (file_read == -1) {
		file_read = ::recv(host_socket.getFd(), reinterpret_cast<char *>(&file_id), sizeof(uint64_t), 0);
	}

    //if (file_read != sizeof(uint64_t)) {
    //    std::cerr << "Failed to read from socket" << std::endl;
    //    return 0;
    //}

    return file_id;
}

Encryption FileClient::getKeyFromId(uint64_t id) {
	FILE* file = fopen("key_map.bin", "rb");
	Encryption::KeySave save;

	while(!feof(file)) {
		if (fread(reinterpret_cast<char*>(&save), sizeof(save), 1, file) != 1) {
			return Encryption();
		}

		if(id == save.id) {
			fclose(file);
            return Encryption::fromSave(save);
		}
	}

	fclose(file);
	return Encryption();
}

bool FileClient::writeKeyToFile(const Encryption::KeySave & save) {
	FILE* file = fopen("key_map.bin", "ab+");

	if(!file) {
		std::cerr << "Failed to open file to write key" << std::endl;
		return false;
	}

	if (fwrite(&save, sizeof(save), 1, file) != 1) {
		return false;
	}

	fclose(file);
	return true;
}


