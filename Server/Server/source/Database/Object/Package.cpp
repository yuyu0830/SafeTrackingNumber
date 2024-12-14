#include "Package.h"

#include <string.h>

Package::Package(int _SenderIdx, int _ReceiverIdx, char* _Name)
{
	DeliveryManIdx = -1;
	SenderIdx = _SenderIdx;
	ReceiverIdx = _ReceiverIdx;

	memcpy(Name, _Name, MAX_NAME_LEN);

	State = PackageState::Idle;
}

void Package::TakePackage(int _DeliveryManIdx)
{
	DeliveryManIdx = _DeliveryManIdx;
	TsuginoStep(PackageState::OnDelivery);
}

void Package::TsuginoStep(PackageState _State)
{
	State = _State;
}
