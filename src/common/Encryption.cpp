#include "Encryption.h"
#include <cstdlib>
#include <time.h>
#include <iostream>
#include <random>

Encryption::Encryption() : rotationIndex(0) {
	std::fill(key.begin(), key.end(), 0);
}

void Encryption::encryptDecrypt(char* message, size_t length) {
	for (int i = 0; i < length; i++){
		rotationIndex = (rotationIndex + 1) % key.size();
		message[i] ^= key[rotationIndex];
	}
}

Encryption::KeySave Encryption::getSave(uint64_t id) const {
	return { this->key, id };
}

Encryption::operator bool() const {
	for (const auto & keyPart : key) {
		if (keyPart != 0) {
			return true;
		}
	}
	return false;
}

Encryption Encryption::getRandomEncryption() {
	Encryption enc;

	std::mt19937 randomGenerator;
	std::uniform_int_distribution<int> distribution('a', 'z');

	for (auto & keyPart : enc.key) {
		keyPart = distribution(randomGenerator);
	}

	return enc;
}

Encryption Encryption::fromSave(const Encryption::KeySave & save) {
	Encryption enc;
	memcpy(&enc.key[0], &save.key[0], Encryption::SIZE);
	return enc;
}
