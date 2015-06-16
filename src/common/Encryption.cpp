#include "Encryption.h"
#include <cstdlib>
#include <time.h>
#include <iostream>
#include <random>

Encryption::Encryption(size_t keyLength) : key(generateRandomKey(keyLength)), current_file_index(0) {
}

Encryption::Encryption(const std::vector<char> & key) : key(key), current_file_index(0) {
}

Encryption::~Encryption() {

}

void Encryption::encryptDecrypt(char* message, size_t length) {
	for (int i = 0; i < length; i++){
		current_file_index = (current_file_index + 1) % key.size();
		message[i] ^= key[current_file_index];
	}
}

const std::vector<char> & Encryption::getKey() const {
	return key;
}

std::vector<char> Encryption::generateRandomKey(size_t keyLength) {
    std::vector<char> newKey(keyLength);

	std::mt19937 randomGenerator;
	std::uniform_int_distribution<int> distribution('a', 'z');

	for (size_t i = 0; i < keyLength; ++i) {
		newKey[i] = distribution(randomGenerator);
	}

	return std::move(newKey);
}
