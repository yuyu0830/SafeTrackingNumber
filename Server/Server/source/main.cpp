#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <thread>

#include "Connection/Server.h"
#include "Params.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

WSADATA wsData;
SOCKET clientSocket, serverSocket;

void ReceiveProcess();
int SocketSetting();
void Terminate();

int main() {
	if (SocketSetting()) {
		cout << "Can't Create Socket.." << endl;
		Terminate();
		return -1;
	}

	thread proc1(ReceiveProcess);

	cout << "Ready.." << endl;

	// While loop: 클라이언트의 메세지를 받아서 출력 후 클라이언트에 다시 보냅니다.
	char msg[PACKET_SIZE] = { 0 };

	while (true) {
		if (WSAGetLastError()) {
			cout << "!" << endl;
			break;
		}

		ZeroMemory(&msg, PACKET_SIZE);
		cin >> msg;
		send(clientSocket, msg, strlen(msg) + 1, 0);
	}

	proc1.join();

	Terminate();
}

void ReceiveProcess() {
	char buf[PACKET_SIZE] = { 0, };

	while (!WSAGetLastError()) {
		ZeroMemory(buf, PACKET_SIZE);
		recv(clientSocket, buf, PACKET_SIZE, 0);
		cout << "> " << buf << endl;
	}
}


int SocketSetting() {
	if (WSAStartup(MAKEWORD(2, 2), &wsData)) {
		std::cerr << "Can't start Winsock" << std::endl;
		return -1;
	}

	SOCKADDR_IN ClientInfo, ServerInfo;

	if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		cerr << "Can't create a socket! Quitting" << endl;
		return -1;
	}

	ServerInfo.sin_family = AF_INET;
	ServerInfo.sin_port = htons(SERVER_PORT);
	ServerInfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(serverSocket, (SOCKADDR*) &ServerInfo, sizeof(ServerInfo)) == SOCKET_ERROR) {
		cerr << "Can't bind a socket! Quitting" << endl;
		return -1;
	}

	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		cerr << "Can't listen a socket! Quitting" << endl;
		return -1;
	}

	int clientSize = sizeof(ClientInfo);
	ZeroMemory(&ClientInfo, clientSize);

	// connection queue의 가장 앞에 있는 클라이언트 요청을 accept하고, client 소켓을 반환합니다.
	clientSocket = accept(serverSocket, (SOCKADDR*) &ClientInfo, &clientSize);

	if (clientSocket == INVALID_SOCKET) {
		cerr << "Can't accept a socket! Quitting" << endl;
		return -1;
	}

	char host[NI_MAXHOST];
	char service[NI_MAXHOST];  
	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);

	// clientSockInfo에 저장된 IP 주소를 통해 도메인 정보를 얻습니다. host 이름은 host에, 포트 번호는 service에 저장됩니다.
	// getnameinfo()는 성공 시 0을 반환합니다. 실패 시 0이 아닌 값을 반환합니다.
	if (!getnameinfo((sockaddr*)&ClientInfo, sizeof(ClientInfo), host, NI_MAXHOST, service, NI_MAXSERV, 0)) {
		cout << host << " connected ON port " << service << endl;
	}
	else {
		inet_ntop(AF_INET, &ClientInfo.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(ClientInfo.sin_port) << endl;
	}

	return 0;
}

void Terminate() {
	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();
}