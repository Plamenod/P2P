#ifndef ENCRYPT_H_
#define ENCRYPT_H_

#include <memory>
#include <cstring>
#include <string>
#include <array>

class Encryption {
public:
	const static int SIZE = 32;
	typedef std::array<char, Encryption::SIZE> KeyType;

	struct KeySave {
		KeyType key;
		uint64_t id;
	};

	/**
	* @brief Create 'empty' encryption object
	*/
	Encryption();

	/**
	* @brief Encrypt or decrypt the message in-place
	* @param message the message to encrypt or decrypt
	* @param length length of the message
	*/
	void encryptDecrypt(char * message, size_t length);

	/**
	* @brief Generate random key and create object for it
	*/
	static Encryption getRandomEncryption();

	/**
	* @brief Get KeySave with own key and provided id;
	*/
	KeySave getSave(uint64_t id) const;

	/**
	* @brief Create Encyption from file save
	*/
	static Encryption fromSave(const KeySave & save);

	operator bool() const;

private:

    KeyType key;
	int rotationIndex;
};

#endif
