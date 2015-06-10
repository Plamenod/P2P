#include "Encryption.h"
#include <cstdlib>
#include <time.h>
#include <iostream>

Encryption::Encryption(size_t keyLength) : key(generateRandomKey(keyLength)) {
}

Encryption::~Encryption() {

}

void Encryption::encryptDecrypt(char* message, size_t length) {
	for (int i = 0; i < length; i++){
		message[i] ^= key.get()[i % (strlen(key.get()) / sizeof(char))];
	}
}

char* Encryption::getKey() const {
	return key.get();
}

std::unique_ptr<char[]> Encryption::generateRandomKey(size_t keyLength) {
	std::unique_ptr<char[]> newKey(new char[keyLength + 1]);

	srand(time(NULL));

	for (size_t i = 0; i < keyLength; ++i) {
		newKey.get()[i] = rand() % 255 - 127;				//range -127 - 127
	}
	newKey[keyLength] = '\0';

	return std::move(newKey);
}
