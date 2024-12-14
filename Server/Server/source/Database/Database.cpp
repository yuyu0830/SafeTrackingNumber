#include "Database.h"
#include <iostream>

Database::Database()
{
	for (int i = 0; i < MAX_USER_NUM; i++) {
		DeliveryManArray[i] = nullptr;
		UserArray[i] = nullptr;
		PackageArray[i] = nullptr;
	}
}

Database::~Database() 
{
	for (int i = 0; i < MAX_USER_NUM; i++) {
		if (DeliveryManArray[i] == nullptr) break;
		delete DeliveryManArray[i];
	}

	for (int i = 0; i < MAX_USER_NUM; i++) {
		if (UserArray[i] == nullptr) break;
		delete UserArray[i];
	}

	for (int i = 0; i < MAX_USER_NUM; i++) {
		if (PackageArray[i] == nullptr) break;
		delete PackageArray[i];
	}
}

int Database::CreateDeliveryMan(const char _Name[MAX_NAME_LEN], const char _Phone[MAX_PHONE_LEN], const char _Address[MAX_ADDR_LEN], uint8_t* SecretKey)
{
	DeliveryMan* NewDeliveryMan = new DeliveryMan(_Name, _Phone, _Address, SecretKey);

	int DeliveryManIdx = DeliveryMan::GetUniqueNumber();
	DeliveryManArray[DeliveryManIdx] = NewDeliveryMan;

	return DeliveryManIdx;
}

int Database::CreateUser(const char _Name[MAX_NAME_LEN], const char _Phone[MAX_PHONE_LEN], const char _Address[MAX_ADDR_LEN], uint8_t* SecretKey)
{
	User* NewUser = new User(_Name, _Phone, _Address, SecretKey);

	int UserIdx = User::GetUniqueNumber();
	UserArray[UserIdx] = NewUser;

	return UserIdx;
}

int Database::CreatePackage(int _SenderIdx, int _ReceiverIdx, char* _Name, uint8_t* ct)
{
	Package* NewPackage = new Package(_SenderIdx, _ReceiverIdx, _Name);

	int PackageIdx = Package::GetUniqueNumber();
	PackageArray[PackageIdx] = NewPackage;

	uint8_t m[32];

	IdxToMessage(PackageIdx, m);

	CryptoInterface::Encryption(ct, m, DeliveryPublicKey);

	return PackageIdx;
}

void Database::DeliveryTakePackage(int PackageIdx, int DeliveryManIdx, uint8_t rct[KYBER_CIPHERTEXTBYTES], uint8_t dct[KYBER_CIPHERTEXTBYTES])
{
	Package* Pack = GetPackage(PackageIdx);

	Pack->TakePackage(DeliveryManIdx);

	int ReceiverIdx = Pack->GetReceiverIdx();

	uint8_t rm[32];
	uint8_t dm[32];

	IdxToMessage(ReceiverIdx, rm);
	IdxToMessage(DeliveryManIdx, dm);
	
	CryptoInterface::Encryption(rct, rm, GetUser(ReceiverIdx)->GetPublicKey());
	CryptoInterface::Encryption(dct, dm, GetDeliveryMan(DeliveryManIdx)->GetPublicKey());
}

int Database::VerifyUser(int UserIdx, uint8_t ct[KYBER_CIPHERTEXTBYTES])
{
	int RandNum = rand() % 255;
	uint8_t m[32];

	IdxToMessage(RandNum, m);

	CryptoInterface::Encryption(ct, m, GetUser(UserIdx)->GetPublicKey());

	return RandNum;
}

int Database::VerifyDeliveryMan(int DeliveryManIdx, uint8_t ct[(KYBER_INDCPA_BYTES)])
{
	int RandNum = rand() % 255;
	uint8_t m[32];

	IdxToMessage(RandNum, m);

	CryptoInterface::Encryption(ct, m, GetDeliveryMan(DeliveryManIdx)->GetPublicKey());

	return RandNum;
}

void Database::IdxToMessage(int Idx, uint8_t m[32])
{
	for (int i = 1; i < 32; i++) {
		m[i] = 255;
	}

	m[0] = (uint8_t)SIGNAL_CHECK_DELIVERY;
	m[31] = (uint8_t)Idx;
}
