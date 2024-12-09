#pragma once

#include <stdint.h>

#include "Crypto/params.h"
#include "Crypto/kem.h"

class CryptoInterface {
public:
	uint8_t* GetPublicKey() { return PublicKey; }
	uint8_t* GetSecretKey() { return SecretKey; }

	static void CreateNewKey(uint8_t* pk, uint8_t* sk);

	static int Encryption(uint8_t* pk, uint8_t* m, uint8_t* ct);
	static int Decryption(uint8_t* sk, uint8_t* ct, uint8_t* m);

private:


public:



private:
	uint8_t PublicKey[KYBER_PUBLICKEYBYTES];
	uint8_t SecretKey[KYBER_SECRETKEYBYTES];
};