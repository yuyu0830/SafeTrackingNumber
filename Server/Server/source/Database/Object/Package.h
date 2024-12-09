#pragma once

#include "DeliveryMan.h"
#include "User.h"
#include "Params.h"

enum PackageState {
	Idle = 0,
	Ready,
	OnDelivery,
	Arrived,
	Checked
};


class Package
{
public:
	Package();

	static int Init();
	static int GetUniqueNumber();


private:
	
public:

private:
	static int UniqueNumber;
	static Package* Packages;

	DeliveryMan* CurrentDeliveryMan;

	User* Sender;
	User* Receiver;

	PackageState State;

};