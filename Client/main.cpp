#define _WINSOCK_DEPRECATED_NO_WARNINGS // winsock c4996 처리

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <string>
#include <thread>

#define PACKET_SIZE 1024
#define SERVER_PORT 54000
#define SERVER_IP_ADDR "127.0.0.1"

using namespace std;

#pragma comment(lib,"ws2_32.lib") // ws2_32.lib 라이브러리를 링크

WSADATA wsData;
SOCKET soc;

int SocketSetting();
void Terminate();

int main()
{
	if (SocketSetting()) {
		cout << "Can't Create Socket.." << endl;
		Terminate();
		return -1;
	}

	// Client 
	char msg[PACKET_SIZE] = { 0 };

	while (!WSAGetLastError()) {
		ZeroMemory(&msg, PACKET_SIZE);
		cin >> msg;
		send(soc, msg, strlen(msg) + 1, 0);

		ZeroMemory(msg, PACKET_SIZE);
		recv(soc, msg, PACKET_SIZE, 0);
		cout << "> " << msg << endl;
	}

	Terminate();

	return 0;
}



int SocketSetting() {
	if (WSAStartup(MAKEWORD(2, 2), &wsData)) {
		cerr << "Can't start Winsock.." << endl;
		return -1;
	}

	soc = socket(AF_INET, SOCK_STREAM, 0);

	if (soc == INVALID_SOCKET) {
		std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
		return -1;
	}

	SOCKADDR_IN hint{};
	hint.sin_family = AF_INET;
	hint.sin_port = htons(SERVER_PORT);
	hint.sin_addr.s_addr = inet_addr(SERVER_IP_ADDR);

	int connResult = connect(soc, (SOCKADDR*)&hint, sizeof(hint));

	for (int i = 0; i < 3; i++) {
		if (!connResult) break;
		else {
			cout << "Can't connect to server, Retry.. (" << i + 1 << "/3)" << endl;
			connResult = connect(soc, (SOCKADDR*)&hint, sizeof(hint));
		}
	}

	if (connResult == SOCKET_ERROR)
	{
		std::cerr << "Can't connect to server, Err #" << WSAGetLastError() << std::endl;
		Terminate();
		return -1;
	}

	return 0;
}


void Terminate() {
	closesocket(soc);
	WSACleanup();
}