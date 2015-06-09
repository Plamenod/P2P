#ifndef ENCRYPT_H_
#define ENCRYPT_H_

#include <memory>
#include <cstring>

class Encryption {
public:
	Encryption(size_t keyLength);
	~Encryption();

	/** Encrypt the message
	*   if the message is already encrypted then it will be decrypted
	*/
	void encryptDecrypt(char* message, size_t length);

	static std::unique_ptr<char[]> generateRandomKey(size_t keyLength);

private:
	std::unique_ptr<char[]> key;
};

#endif