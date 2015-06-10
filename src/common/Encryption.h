#ifndef ENCRYPT_H_
#define ENCRYPT_H_

#include <memory>
#include <cstring>
#include <string>

class Encryption {
public:
	/**
	* @brief construct with random generated key
	* @param keyLength length of random generated key
	*/
	Encryption(size_t keyLength);

	/**
	* @brief construct with specified key
	*/
	Encryption(std::string key);
	~Encryption();

	/** 
	* @brief Encrypt or decrypt the message in-place
	* @param message the message to encrypt or decrypt
	* @param length length of the message
	*/
	void encryptDecrypt(char* message, size_t length);

	static std::unique_ptr<char[]> generateRandomKey(size_t keyLength);

	char* getKey() const;

private:
	std::unique_ptr<char[]> key;
};

#endif
