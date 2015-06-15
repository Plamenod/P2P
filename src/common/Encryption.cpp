#include "Encryption.h"
#include <cstdlib>
#include <time.h>
#include <iostream>

Encryption::Encryption(size_t keyLength) : key(generateRandomKey(keyLength)), current_file_index(0) , key_length(keyLength) {
}

Encryption::Encryption(std::string key) : key(new char[key.length() + 1]), current_file_index(0) , key_length(key.length()) {
	memcpy(this->key.get(), key.c_str(), key.length() + 1);
}

Encryption::~Encryption() {

}

void Encryption::encryptDecrypt(char* message, size_t length) {
	for (int i = 0; i < length; i++){
		message[i] ^= key.get()[current_file_index++ % (key_length / sizeof(char))];
	}
}

char* Encryption::getKey() const {
	return key.get();
}

std::unique_ptr<char[]> Encryption::generateRandomKey(size_t keyLength) {
	std::unique_ptr<char[]> newKey(new char[keyLength + 1]);

	srand(time(NULL));

	for (size_t i = 0; i < keyLength; ++i) {
		newKey.get()[i] = rand() % 255 + 1;
	}
	newKey[keyLength] = '\0';

	return std::move(newKey);
}
