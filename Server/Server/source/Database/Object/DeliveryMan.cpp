#include "DeliveryMan.h"
#include <cstring>

DeliveryMan::DeliveryMan(const char _Name[MAX_NAME_LEN], const char _Phone[MAX_PHONE_LEN], const char _Address[MAX_ADDR_LEN], uint8_t* SecretKey) {
	strcpy_s(Name, _Name);
	strcpy_s(Phone, _Phone);
	strcpy_s(Address, _Address);

	CryptoInterface::CreateNewKey(PublicKey, SecretKey);
}