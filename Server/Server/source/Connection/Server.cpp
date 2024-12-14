#include "Server.h"

int RunServer()
{
	WSADATA wsData;

	if (WSAStartup(MAKEWORD(2, 2), &wsData)) {
		cerr << "Can't start Winsock" << endl;
		return -1;
	}

	SOCKADDR_IN ServerInfo;

	if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		cerr << "Can't create a socket! Quitting" << endl;
		return -1;
	}

	ServerInfo.sin_family = AF_INET;
	ServerInfo.sin_port = htons(SERVER_PORT);
	ServerInfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(serverSocket, (SOCKADDR*)&ServerInfo, sizeof(ServerInfo)) == SOCKET_ERROR) {
		cerr << "Can't bind a socket! Quitting" << endl;
		return -1;
	}

	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		cerr << "Can't listen a socket! Quitting" << endl;
		return -1;
	}

	return 0;
}

void CloseServer() 
{
	Terminate();
}

void ClientAccept()
{
	while (1) {
		SOCKADDR_IN client = {};
		int client_size = sizeof(client);

		// 클라이언트에서 연결을 수락하기 위해 ClientSocket이라는 임시 소켓 개체 생성
		SOCKET client_socket = accept(serverSocket, (SOCKADDR*)&client, &client_size);
		if (client_socket == INVALID_SOCKET) {
			std::cerr << "클라이언트 연결 실패: " << WSAGetLastError() << std::endl;
			closesocket(client_socket);
		}

		// 연결 시 클라이언트 정보 출력
		if (!WSAGetLastError()) {
			std::cout << "연결 완료" << endl;
			std::cout << "Client IP: " << inet_ntoa(client.sin_addr) << endl;
			std::cout << "Port: " << ntohs(client.sin_port) << endl;
		}

		ClientInfo* clientData = new ClientInfo;
		clientData->socket = client_socket;
		clientData->clientAddress = client;

		// 클라이언트 처리를 위한 스레드 시작
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClient, clientData, 0, NULL);
		// _beginthreadex는 스레드 핸들을 반환하여 스레드의 상태를 추적하고 제어할 수 있음
		clientThreads.push_back(hThread);

		ClientInfo newClient;
		newClient.socket = client_socket;
		newClient.clientAddress = client; // 필요한 클라이언트 정보 설정

		clientPool.push_back(newClient);
	}
}

unsigned int __stdcall HandleClient(void* data)
{
	ClientInfo* clientData = static_cast<ClientInfo*>(data);
	SOCKET clientSocket = clientData->socket;
	sockaddr_in clientAddr = clientData->clientAddress;
	delete clientData;

	char buffer[1024];
	int recvSize;

	// 클라이언트 메시지 수신 및 에코
	do {
		ZeroMemory(&buffer, 1024); // 버퍼 초기화
		recvSize = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (recvSize > 0) {
			cout << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << "로부터 받은 메세지: [" << buffer << "]를 echo합니다." << endl;
			send(clientSocket, buffer, recvSize, 0);
		}
	} while (recvSize > 0);

	//std::cout << "클라이언트 연결 종료" << std::endl;
	for (size_t i = 0; i < clientPool.size(); ++i) {
		if (clientPool[i].socket == clientSocket) {
			// 클라이언트 정보 삭제
			cout << "클라이언트와 연결이 종료되었습니다: " << inet_ntoa(clientPool[i].clientAddress.sin_addr) << ":" << ntohs(clientPool[i].clientAddress.sin_port) << endl;
			clientPool.erase(clientPool.begin() + i);
			break;
		}
	}
	closesocket(clientSocket);
	_endthreadex(0);
	return 0;
}

void ReceiveProcess() {
	/*char msg[PACKET_SIZE] = { 0 };

	ZeroMemory(msg, PACKET_SIZE);
	recv(clientSocket, msg, PACKET_SIZE, 0);
	cout << "> " << msg << endl;
	
	ZeroMemory(&msg, PACKET_SIZE);
	cin >> msg;
	send(clientSocket, msg, strlen(msg) + 1, 0);*/

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

void Terminate()
{
	closesocket(serverSocket);
	WSACleanup();
}
