#pragma once

#include "CryptoInterface.h"

class DeliveryMan : public CryptoInterface
{
public:
	DeliveryMan();

	static int Init();
	static int GetUniqueNumber();

private:
	static int UniqueNumber;


public:



private:
	char Name[10];
	char Phone[15];
	char Address[255];



};