#pragma once

#include "CryptoInterface.h"
#include "Crypto/kem.h"
#include "Params.h"

class User
{
public:
	User(const char _Name[MAX_NAME_LEN], const char _Phone[MAX_PHONE_LEN], const char _Address[MAX_ADDR_LEN], uint8_t* SecretKey);

	uint8_t* GetPublicKey() { return PublicKey; }

	char* GetName() { return Name; }
	char* GetPhone() { return Phone; }
	char* GetAddress() { return Address; }

	static int GetUniqueNumber() { return UniqueNumber++; }

private:
	

public:
	
private:
	char Name[MAX_NAME_LEN];
	char Phone[MAX_PHONE_LEN];
	char Address[MAX_ADDR_LEN];

	static int UniqueNumber;

	uint8_t PublicKey[PUBLIC_KEY_SIZE];
};