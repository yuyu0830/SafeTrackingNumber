#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <thread>

#include "Params.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

class Server {
public:
	Server();

	int RunServer();
	void CloseServer();

private:
	void Connect();

	void ReceiveProcess();
	int SocketSetting();
	void Terminate();


public:


private:
	SOCKET clientSocket, serverSocket;

};