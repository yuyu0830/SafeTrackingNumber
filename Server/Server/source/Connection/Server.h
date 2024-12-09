#pragma once

#include <thread>

class Server {
public:
	Server();

	void RunServer();
	void CloseServer();

private:
	void Connect();as

	void ReceiveProcess();
	int SocketSetting();
	void Terminate();


public:


private:
	WSADATA wsData;
	SOCKET clientSocket, serverSocket;

};