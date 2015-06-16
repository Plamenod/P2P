#include "Encryption.h"
#include <cstdlib>
#include <time.h>
#include <iostream>

Encryption::Encryption(size_t keyLength) : key(generateRandomKey(keyLength)), current_file_index(0) {
}

Encryption::Encryption(const std::vector<char> & key) : key(key), current_file_index(0) {
}

Encryption::~Encryption() {

}

void Encryption::encryptDecrypt(char* message, size_t length) {
	for (int i = 0; i < length; i++){
		message[i] ^= key[current_file_index++ % (key.size() / sizeof(char))];
	}
}

const std::vector<char> & Encryption::getKey() const {
	return key;
}

std::vector<char> Encryption::generateRandomKey(size_t keyLength) {
    std::vector<char> newKey(keyLength);

	srand(time(NULL));

	for (size_t i = 0; i < keyLength; ++i) {
		newKey[i] = rand() % 100 + 1;
	}

	return std::move(newKey);
}
