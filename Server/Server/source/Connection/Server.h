#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <vector>

#include "Params.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

struct ClientInfo { // 클라이언트 정보 필드
	SOCKET socket;
	sockaddr_in clientAddress;
};

vector<HANDLE> clientThreads;
vector<ClientInfo> clientPool;

SOCKET serverSocket;

int RunServer();
void CloseServer();

void ClientAccept();

unsigned int __stdcall HandleClient(void* data);

void Terminate();