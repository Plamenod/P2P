#ifndef ENCRYPT_H_
#define ENCRYPT_H_

#include <memory>
#include <cstring>
#include <string>
#include <vector>

class Encryption {
public:
	/**
	* @brief construct with length of generated key
	* @param keyLength length of random generated key
	*/
	Encryption(size_t keyLength);

	/**
	* @brief construct with specified key
	*/
	Encryption(const std::vector<char> & key);
	~Encryption();

	/**
	* @brief Encrypt or decrypt the message in-place
	* @param message the message to encrypt or decrypt
	* @param length length of the message
	*/
	void encryptDecrypt(char* message, size_t length);

	static std::vector<char> generateRandomKey(size_t keyLength);

    const std::vector<char> & getKey() const;

private:
    std::vector<char> key;
	int current_file_index;
};

#endif
