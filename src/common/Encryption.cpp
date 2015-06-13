#include "Encryption.h"
#include <cstdlib>
#include <time.h>
#include <iostream>

Encryption::Encryption(size_t keyLength) : key(generateRandomKey(keyLength)), current_file_index(0) {
}

Encryption::Encryption(std::string key) : key(new char[key.length() + 1]), current_file_index(0) {
	memcpy(this->key.get(), key.c_str(), key.length() + 1);
}

Encryption::~Encryption() {

}

void Encryption::encryptDecrypt(char* message, size_t length) {
	for (int i = 0; i < length; i++){
		message[i] ^= key.get()[current_file_index++ % (strlen(key.get()) / sizeof(char))];
	}
}

char* Encryption::getKey() const {
	return key.get();
}

std::unique_ptr<char[]> Encryption::generateRandomKey(size_t keyLength) {
	std::unique_ptr<char[]> newKey(new char[keyLength + 1]);

	srand(time(NULL));

	for (size_t i = 0; i < keyLength; ++i) {
		newKey.get()[i] = rand() % 255;
	}
	newKey[keyLength] = '\0';

	return std::move(newKey);
}
