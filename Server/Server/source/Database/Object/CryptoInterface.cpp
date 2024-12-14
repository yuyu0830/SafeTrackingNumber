#include "CryptoInterface.h"
#include "Params.h"

#include "Crypto/kem.h"
#include <string.h>
#include <iostream>

int CryptoInterface::CreateNewKey(uint8_t pk[PUBLIC_KEY_SIZE], uint8_t sk[SECRET_KEY_SIZE])
{
	int result = crypto_kem_keypair(pk, sk);

	return 0;
}

int CryptoInterface::Encryption(uint8_t* ct, uint8_t* m, const uint8_t* pk)
{
	int result = crypto_kem_enc(ct, m, pk);

	return result;
}

int CryptoInterface::Decryption(uint8_t* m, const uint8_t* ct, const uint8_t* sk)
{
	int result = crypto_kem_dec(m, ct, sk);

	return result;
}
