#pragma once

#include "Database/Object/CryptoInterface.h"
#include "Object/DeliveryMan.h"
#include "Object/Package.h"
#include "Object/User.h"

class Database {
public:
	Database();
	~Database();

	int CreateDeliveryMan(const char _Name[MAX_NAME_LEN], const char _Phone[MAX_PHONE_LEN], const char _Address[MAX_ADDR_LEN], uint8_t* SecretKey);
	int CreateUser(const char _Name[MAX_NAME_LEN], const char _Phone[MAX_PHONE_LEN], const char _Address[MAX_ADDR_LEN], uint8_t* SecretKey);
	int CreatePackage(int _SenderIdx, int _ReceiverIdx, char* _Name, uint8_t* ct);

	DeliveryMan*	GetDeliveryMan(int DeliveryManIdx)	{ return DeliveryManArray[DeliveryManIdx]; }
	User*			GetUser(int UserIdx)				{ return UserArray[UserIdx]; };
	Package*		GetPackage(int PackageIdx)			{ return PackageArray[PackageIdx]; };

	void DeliveryTakePackage(int PackageIdx, int DeliveryManIdx, uint8_t rct[KYBER_CIPHERTEXTBYTES], uint8_t dct[KYBER_CIPHERTEXTBYTES]);
	int VerifyUser(int UserIdx, uint8_t ct[KYBER_CIPHERTEXTBYTES]);
	int VerifyDeliveryMan(int DeliveryManIdx, uint8_t ct[KYBER_CIPHERTEXTBYTES]);

private:
	void IdxToMessage(int Idx, uint8_t m[32]);
	
public:

private:
	DeliveryMan* DeliveryManArray[MAX_USER_NUM];
	Package* PackageArray[MAX_USER_NUM];
	User* UserArray[MAX_USER_NUM];


};