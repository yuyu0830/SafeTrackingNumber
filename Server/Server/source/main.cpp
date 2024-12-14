#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <thread>
#include <stdlib.h>
#include <winsock2.h>
#include <time.h>

#include "QRGenerator/QRGenerator.h"
#include "Database/Database.h"

#include "Params.h"

#pragma comment(lib,"ws2_32.lib") 

using namespace std;

struct ClientInfo { // Ŭ���̾�Ʈ ���� �ʵ�
	SOCKET socket;
	sockaddr_in clientAddress;
};

int DeliveryMan::UniqueNumber = 0;
int User::UniqueNumber = 0;
int Package::UniqueNumber = 0;

vector<HANDLE> clientThreads;
vector<ClientInfo> clientPool;

SOCKET serverSocket;
Database* DB;
QRGenerator* Qrgen;

int RunServer();
void CloseServer();
void ClientAccept();
unsigned int __stdcall HandleClient(void* data);

int GetIdxFromReceiveMessage(char* ReceiveMessage);
int UserVerify(SOCKET soc, char* ReceiveMessage);
int DeliveryManVerify(SOCKET soc, char* ReceiveMessage);

int main() {
	srand(time(NULL));

	if (RunServer()) {
		cout << "Run Server Fail.." << endl;
		CloseServer();
		return -1;
	}

	cout << "Server Running.." << endl;

	DB = new Database();
	Qrgen = new QRGenerator();

	// Server
	thread ClientListen(ClientAccept);

	char msg[PACKET_SIZE] = { 0 }; // �޼��� �Է¹��� ���� ����

	while (1) {
		ZeroMemory(&msg, PACKET_SIZE); // ���� �ʱ�ȭ�ϰ�, �Է¹��� �޼����� Ŭ���̾�Ʈ���� send
		cin >> msg;
		if ((string)msg == "list") { // "list" �Է� �� �������� ����� Ŭ���̾�Ʈ ��� ���
			cout << "����� Client IP Address ���" << endl;
			for (ClientInfo c : clientPool) {
				cout << inet_ntoa(c.clientAddress.sin_addr) << ":" << ntohs(c.clientAddress.sin_port) << endl;
			}
		}
		else {
			for (ClientInfo c : clientPool) {
				send(c.socket, msg, strlen(msg), 0);
			}
		}
	}

	// ������ ���� ���
	ClientListen.join();

	for (HANDLE hThread : clientThreads) {
		WaitForSingleObject(hThread, INFINITE); // �����尡 ����� �� ���� �����, INFINITE�� ���� ���
		CloseHandle(hThread);
	}

	delete DB;
	delete Qrgen;

	closesocket(serverSocket);

	WSACleanup(); // ���ҽ� ����

	return 0;
}



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
	closesocket(serverSocket);
	WSACleanup();
}

void ClientAccept()
{
	while (1) {
		SOCKADDR_IN client = {};
		int client_size = sizeof(client);

		// Ŭ���̾�Ʈ���� ������ �����ϱ� ���� ClientSocket�̶�� �ӽ� ���� ��ü ����
		SOCKET client_socket = accept(serverSocket, (SOCKADDR*)&client, &client_size);
		if (client_socket == INVALID_SOCKET) {
			std::cerr << "Ŭ���̾�Ʈ ���� ����: " << WSAGetLastError() << std::endl;
			closesocket(client_socket);
		}

		// ���� �� Ŭ���̾�Ʈ ���� ���
		if (!WSAGetLastError()) {
			std::cout << "���� �Ϸ�" << endl;
			std::cout << "Client IP: " << inet_ntoa(client.sin_addr) << endl;
			std::cout << "Port: " << ntohs(client.sin_port) << endl;
		}

		ClientInfo* clientData = new ClientInfo;
		clientData->socket = client_socket;
		clientData->clientAddress = client;

		// Ŭ���̾�Ʈ ó���� ���� ������ ����
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClient, clientData, 0, NULL);
		// _beginthreadex�� ������ �ڵ��� ��ȯ�Ͽ� �������� ���¸� �����ϰ� ������ �� ����
		clientThreads.push_back(hThread);

		ClientInfo newClient;
		newClient.socket = client_socket;
		newClient.clientAddress = client; // �ʿ��� Ŭ���̾�Ʈ ���� ����

		clientPool.push_back(newClient);
	}
}

unsigned int __stdcall HandleClient(void* data)
{
	ClientInfo* clientData = static_cast<ClientInfo*>(data);
	SOCKET clientSocket = clientData->socket;
	sockaddr_in clientAddr = clientData->clientAddress;

	delete clientData;

	char buffer[PACKET_SIZE];
	int recvSize;

	// Ŭ���̾�Ʈ �޽��� ���� �� ����
	do {
		ZeroMemory(&buffer, PACKET_SIZE); // ���� �ʱ�ȭ
		recvSize = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (recvSize > 0) {
			if (!strncmp(buffer, SIGNAL_CREATE_USER, SIGNAL_SIZE)) { // Create User
				cout << "Create User" << endl;
				char Name[MAX_NAME_LEN];
				char Phone[MAX_PHONE_LEN];
				char Addr[MAX_ADDR_LEN];
				uint8_t SecretKey[SECRET_KEY_SIZE];

				// GetName
				ZeroMemory(&buffer, PACKET_SIZE); // ���� �ʱ�ȭ
				recv(clientSocket, buffer, PACKET_SIZE, 0);
				memcpy(Name, buffer, MAX_NAME_LEN);
				cout << "User Name : " << buffer << endl;

				// Get Phone
				ZeroMemory(&buffer, PACKET_SIZE); // ���� �ʱ�ȭ
				recv(clientSocket, buffer, PACKET_SIZE, 0);
				memcpy(Phone, buffer, MAX_PHONE_LEN);
				cout << "User Phone : " << buffer << endl;

				// Get Addr
				ZeroMemory(&buffer, PACKET_SIZE); // ���� �ʱ�ȭ
				recv(clientSocket, buffer, PACKET_SIZE, 0);
				memcpy(Addr, buffer, MAX_ADDR_LEN);
				cout << "User Addr : " << buffer << endl;

				int UserIdx = DB->CreateUser(Name, Phone, Addr, SecretKey);
				send(clientSocket, (const char*)SecretKey, SECRET_KEY_SIZE, 0);
				cout << "Create User Complete!" << endl;

				printf("pk : ");
				for (int i = 0; i < PUBLIC_KEY_SIZE; i++) {
					printf("%02X", DB->GetUser(UserIdx)->GetPublicKey()[i]);
				}
				printf("\n");

				printf("sk : ");
				for (int i = 0; i < SECRET_KEY_SIZE; i++) {
					printf("%02X", SecretKey[i]);
				}
				printf("\n");
			}

			else if (!strncmp(buffer, SIGNAL_CREATE_DELIVERY, SIGNAL_SIZE)) { // Create Delivery Man
				cout << "Create Delivery Man" << endl;

				char Name[MAX_NAME_LEN];
				char Phone[MAX_PHONE_LEN];
				char Addr[MAX_ADDR_LEN];
				uint8_t SecretKey[SECRET_KEY_SIZE];

				// GetName
				ZeroMemory(&buffer, PACKET_SIZE);
				recv(clientSocket, buffer, PACKET_SIZE, 0);
				memcpy(Name, buffer, MAX_NAME_LEN);
				cout << "Delivery Man Name : " << buffer << endl;

				// Get Phone
				ZeroMemory(&buffer, PACKET_SIZE);
				recv(clientSocket, buffer, PACKET_SIZE, 0);
				memcpy(Phone, buffer, MAX_PHONE_LEN);
				cout << "Delivery Man Phone : " << buffer << endl;

				// Get Addr
				ZeroMemory(&buffer, PACKET_SIZE);
				recv(clientSocket, buffer, PACKET_SIZE, 0);
				memcpy(Addr, buffer, MAX_ADDR_LEN);
				cout << "Delivery Man Addr : " << buffer << endl;

				int DeliveryManIdx = DB->CreateDeliveryMan(Name, Phone, Addr, SecretKey);
				send(clientSocket, (const char*)SecretKey, SECRET_KEY_SIZE, 0);
				cout << "Create Delivery Man Complete!" << endl;

				printf("pk : ");
				for (int i = 0; i < PUBLIC_KEY_SIZE; i++) {
					printf("%02X", DB->GetDeliveryMan(DeliveryManIdx)->GetPublicKey()[i]);
				}
				printf("\n");

				printf("sk : ");
				for (int i = 0; i < SECRET_KEY_SIZE; i++) {
					printf("%02X", SecretKey[i]);
				}
				printf("\n");
			}

			else if (!strncmp(buffer, SIGNAL_CREATE_PACKAGE, SIGNAL_SIZE)) { // Create Package
				cout << "Create Package" << endl;

				// Get Send User
				ZeroMemory(&buffer, PACKET_SIZE);
				recv(clientSocket, buffer, PACKET_SIZE, 0);
				int SendUserIdx = UserVerify(clientSocket, buffer);

				if (SendUserIdx == INVALID_VALUE) {
					cout << "Cannot Find User!" << endl;
					send(clientSocket, (const char*) SignalUnverified, 2, 0);
					break;
				}

				send(clientSocket, (const char*)SignalVerified, 2, 0);

				cout << "Send User : " << SendUserIdx << endl;
				cout << "Send User Name : " << DB->GetUser(SendUserIdx)->GetName() << endl;
				
				// Get Receive User
				ZeroMemory(&buffer, PACKET_SIZE);
				recv(clientSocket, buffer, PACKET_SIZE, 0);
				int ReceiveUserIdx = GetIdxFromReceiveMessage(buffer);
				cout << "Receive User : " << ReceiveUserIdx << endl;
				cout << "Receive User Name : " << DB->GetUser(ReceiveUserIdx)->GetName() << endl;

				// Get Name of Package
				ZeroMemory(&buffer, PACKET_SIZE);
				recv(clientSocket, buffer, PACKET_SIZE, 0);
				cout << "Package Name : " << buffer << endl;

				uint8_t ct[KYBER_CIPHERTEXTBYTES];
				int PackageIdx = DB->CreatePackage(SendUserIdx, ReceiveUserIdx, buffer, ct);

				cout << "Package : ";
				for (int i = 0; i < KYBER_CIPHERTEXTBYTES; i++) {
					printf("%02X", ct[i]);
				}

				printf("\n");
				Qrgen->MakeQRCode((const char*)ct, KYBER_CIPHERTEXTBYTES);
			}

			else if (!strncmp(buffer, SIGNAL_TAKE_PACKAGE, SIGNAL_SIZE)) { // Take Package
				cout << "Delivery Man Try to Take Package" << endl;

				// Verify Delivery Man
				ZeroMemory(&buffer, PACKET_SIZE);
				recv(clientSocket, buffer, PACKET_SIZE, 0);
				int DeliveryManIdx = DeliveryManVerify(clientSocket, buffer);

				if (DeliveryManIdx == INVALID_VALUE) {
					cout << "Cannot Find Delivery Man!" << endl;
					send(clientSocket, (const char*)SignalUnverified, 2, 0);
					break;
				}
				cout << "Valid Delivery Man" << endl;
				cout << "Delivery Man Name : " << DB->GetDeliveryMan(DeliveryManIdx)->GetName() << endl;

				send(clientSocket, (const char*)SignalVerified, 2, 0);

				// Verify Package
				ZeroMemory(&buffer, PACKET_SIZE);
				recv(clientSocket, buffer, PACKET_SIZE, 0);

				if (buffer[0] != SIGNAL_CHECK_DELIVERY || (unsigned char) buffer[1] != 255) {
					cout << "Unvalid Package" << endl;
					send(clientSocket, (const char*)SignalUnverified, 2, 0);
					break;
				}
				cout << "Valid Package" << endl;

				send(clientSocket, (const char*)SignalVerified, 2, 0);

				int PackageIdx = buffer[31];
				cout << "Package Idx : " << PackageIdx << endl;

				Package* Pack = DB->GetPackage(PackageIdx);

				User* Receiver = DB->GetUser(Pack->GetReceiverIdx());

				cout << "Sender Name : " << DB->GetUser(Pack->GetSenderIdx())->GetName() << endl;
				cout << "Receiver Name : " << DB->GetUser(Pack->GetReceiverIdx())->GetName() << endl;

				uint8_t rct[KYBER_CIPHERTEXTBYTES];
				uint8_t dct[KYBER_CIPHERTEXTBYTES];

				DB->DeliveryTakePackage(PackageIdx, DeliveryManIdx, rct, dct);

				cout << "Receiver's QR Code" << endl;
				for (int i = 0; i < KYBER_CIPHERTEXTBYTES; i++) {
					printf("%02X", rct[i]);
				}
				Qrgen->MakeQRCode((const char*)rct, KYBER_CIPHERTEXTBYTES);

				cout << "DeliveryMan's QR Code" << endl;
				for (int i = 0; i < KYBER_CIPHERTEXTBYTES; i++) {
					printf("%02X", dct[i]);
				}
				Qrgen->MakeQRCode((const char*)dct, KYBER_CIPHERTEXTBYTES);
				
			}
			/*cout << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << "�κ��� ���� �޼���: [" << buffer << "]�� echo�մϴ�." << endl;
			send(clientSocket, buffer, recvSize, 0);*/
		}
	} while (recvSize > 0);

	//std::cout << "Ŭ���̾�Ʈ ���� ����" << std::endl;
	for (size_t i = 0; i < clientPool.size(); ++i) {
		if (clientPool[i].socket == clientSocket) {
			// Ŭ���̾�Ʈ ���� ����
			cout << "Ŭ���̾�Ʈ�� ������ ����Ǿ����ϴ�: " << inet_ntoa(clientPool[i].clientAddress.sin_addr) << ":" << ntohs(clientPool[i].clientAddress.sin_port) << endl;
			clientPool.erase(clientPool.begin() + i);
			break;
		}
	}
	closesocket(clientSocket);
	_endthreadex(0);
	return 0;
}

int GetIdxFromReceiveMessage(char* ReceiveMessage)
{
	try {
		return std::stoi(ReceiveMessage); 
	}
	catch (const std::invalid_argument& e) {
		return -1;
	}
	catch (const std::out_of_range& e) {
		return -2;
	}
}

int UserVerify(SOCKET soc, char* ReceiveMessage)
{
	char buffer[PACKET_SIZE];
	
	// Make Random Number and Cipher Text
	int UserIdx = GetIdxFromReceiveMessage(ReceiveMessage);
	uint8_t ct[KYBER_CIPHERTEXTBYTES];

	int RandNum = DB->VerifyUser(UserIdx, ct);
	send(soc, (const char*)ct, KYBER_CIPHERTEXTBYTES, 0);

	// Verify
	ZeroMemory(&buffer, PACKET_SIZE);
	recv(soc, buffer, PACKET_SIZE, 0);

	if (buffer[0] == SIGNAL_CHECK_DELIVERY && RandNum == buffer[31])
		return UserIdx;
	
	else
		return INVALID_VALUE;
}

int DeliveryManVerify(SOCKET soc, char* ReceiveMessage)
{
	char buffer[PACKET_SIZE];

	// Make Random Number and Cipher Text
	int DeliveryManIdx = GetIdxFromReceiveMessage(ReceiveMessage);
	uint8_t ct[KYBER_CIPHERTEXTBYTES];

	int RandNum = DB->VerifyDeliveryMan(DeliveryManIdx, ct);
	send(soc, (const char*)ct, KYBER_CIPHERTEXTBYTES, 0);

	// Verify
	ZeroMemory(&buffer, PACKET_SIZE);
	recv(soc, buffer, PACKET_SIZE, 0);

	if (buffer[0] == SIGNAL_CHECK_DELIVERY && RandNum == buffer[31])
		return DeliveryManIdx;

	else
		return INVALID_VALUE;
}
