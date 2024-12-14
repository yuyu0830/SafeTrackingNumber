#pragma once

#include <stdint.h>

#include "Params.h"

class CryptoInterface {
public:
	static int CreateNewKey(uint8_t pk[PUBLIC_KEY_SIZE], uint8_t sk[SECRET_KEY_SIZE]);
	static int Encryption(uint8_t* ct, uint8_t* m, const uint8_t* pk);
	static int Decryption(uint8_t* m, const uint8_t* ct, const uint8_t* sk);

private:
	

public:



private:
};