#pragma once

#include "CryptoInterface.h"

class User : public CryptoInterface
{
public:
	User();

	static int Init();
	static int GetUniqueNumber();

private:
	

public:


private:
	static int UniqueNumber;

	char Name[10];
	char Phone[15];
	char Address[255];


};