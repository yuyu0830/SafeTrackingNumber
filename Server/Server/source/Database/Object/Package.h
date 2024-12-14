#pragma once

#include "DeliveryMan.h"
#include "User.h"
#include "Params.h"

enum PackageState {
	Idle = 0,
	OnDelivery,
	Arrived,
	Checked
};

class Package
{
public:
	Package(int _SenderIdx, int _ReceiverIdx, char* _Name);

	static int GetUniqueNumber() { return UniqueNumber++; }

	void TakePackage(int _DeliveryManIdx);
	void PackageDelivered();

	int GetDeliverManIdx() { return DeliveryManIdx; }
	int GetSenderIdx() { return SenderIdx; }
	int GetReceiverIdx() { return ReceiverIdx; }

	PackageState GetCurrentState() { return State; }

private:
	void TsuginoStep(PackageState _State);

public:

private:
	char Name[MAX_NAME_LEN];

	static int UniqueNumber;

	int DeliveryManIdx;

	int SenderIdx;
	int ReceiverIdx;

	PackageState State;

};