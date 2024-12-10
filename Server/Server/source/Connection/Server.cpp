#include "Server.h"

Server::Server()
{

}

int Server::RunServer()
{
	if (SocketSetting()) {
		return -1;
	};


}

void Server::CloseServer()
{
	Terminate();
}

void Server::Connect()
{

}

void Server::ReceiveProcess() {
	/*char buf[PACKET_SIZE] = { 0, };

	while (!WSAGetLastError()) {
		ZeroMemory(buf, PACKET_SIZE);
		recv(clientSocket, buf, PACKET_SIZE, 0);
		cout << "> " << buf << endl;
	}

	char msg[PACKET_SIZE] = { 0 };

	while (true) {
		if (WSAGetLastError()) {
			cout << "!" << endl;
			break;
		}

		ZeroMemory(&msg, PACKET_SIZE);
		cin >> msg;
		send(clientSocket, msg, strlen(msg) + 1, 0);
	}*/
}

int Server::SocketSetting()
{
	WSADATA wsData;

	if (WSAStartup(MAKEWORD(2, 2), &wsData)) {
		cerr << "Can't start Winsock" << endl;
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

	clientSocket = accept(serverSocket, (SOCKADDR*)&ClientInfo, &clientSize);

	if (clientSocket == INVALID_SOCKET) {
		cerr << "Can't accept a socket! Quitting" << endl;
		return -1;
	}

	char host[NI_MAXHOST];
	char service[NI_MAXHOST];
	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);

	if (!getnameinfo((sockaddr*)&ClientInfo, sizeof(ClientInfo), host, NI_MAXHOST, service, NI_MAXSERV, 0)) {
		cout << host << " connected ON port " << service << endl;
	}
	else {
		inet_ntop(AF_INET, &ClientInfo.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(ClientInfo.sin_port) << endl;
	}

	return 0;
}

void Server::Terminate()
{
	closesocket(clientSocket);
	closesocket(serverSocket);
	WSACleanup();
}
