#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <thread>

#include "Connection/Server.h"
#include "Params.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
	Server serv;

	if (serv.RunServer()) {
		cout << "Run Server Fail.." << endl;
		serv.CloseServer();
		return -1;
	}
}


	
