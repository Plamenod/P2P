#include "FileClient.h"
#include "../util.h"
#include <iostream>
#include <cstring>
#include <stdint.h>
#include <sstream>

#define MAX_SIZE 100
#define BYTE_TO_RECEIVE 100

FileClient::FileClient() {

}

FileClient::~FileClient() {

}

uint64_t FileClient::send(
	const std::string & host,
	std::string & file_path,
	uint64_t from,
	uint64_t to)
{
	uint64_t data_length = to - from;


	if (data_length <= 0) {
		std::cerr << "Length is <= 0" << std::endl;
		return 0;
	}

	std::string ip = getHost(host);
	unsigned short host_port = getPort(host);

	Socket host_socket;
	host_socket.connectTo(ip, host_port);

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
	file_to_send = fopen(file_path.c_str(), "r");

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


	std::unique_ptr<char[]> buffer(new char[MAX_SIZE + 1]);

	while (data_length) {
		int byte_read = fread(buffer.get(), sizeof(char), MAX_SIZE, file_to_send);
		data_length -= byte_read;

		if (ferror(file_to_send)) {
			std::cerr << "Failed to read file content" << std::endl;
			fclose(file_to_send);
			return 0;
		}

		int bytes_sent = ::send(host_socket.getFd(), buffer.get(), byte_read, 0);

		if (bytes_sent == -1) {
			std::cerr << "Failed to write to socket" << std::endl;
			fclose(file_to_send);
			return 0;
		}
	}

	uint64_t fileID = getFileID(host_socket);
	//    std::cout << "FileID: " << fileID << std::endl;

	fclose(file_to_send);
	return fileID;
}

std::unique_ptr<char[]> FileClient::getFile(const std::string& host, uint64_t id) {
	std::string ip = getHost(host);
	unsigned short port = getPort(host);

	Socket host_socket;
	host_socket.connectTo(ip, port);

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

	std::cout << "File size: " << file_size << std::endl;

	std::unique_ptr<char[]> file_content(new char[file_size]);

	//TODO: remove
	uint64_t x;
	::recv(host_socket, reinterpret_cast<char*>(&x), sizeof(x), 0);
	std::cout << x << std::endl;
	::recv(host_socket, reinterpret_cast<char*>(&x), sizeof(x), 0);
	std::cout << x << std::endl;
	file_size -= 8 * 2;

	int offset = 0;
	while (file_size > 0) {
		int byte_read = ::recv(
			host_socket.getFd(),
			reinterpret_cast<char*>(file_content.get() + offset),
			file_size,
			0);

		//printf("byte read: %d \n Message: %s\n", byte_read, file_content.get());

		file_size -= byte_read;
		offset += byte_read;
	}
	//printf("byte read: %d \n Message: %s\n", offset, file_content.get());

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
	uint64_t file_id;
	int file_read = ::recv(host_socket.getFd(), reinterpret_cast<char *>(&file_id), sizeof(uint64_t), 0);

	if (file_read != sizeof(uint64_t)) {
		std::cerr << "Failed to read from socket" << std::endl;
		return 0;
	}

	return file_id;
}