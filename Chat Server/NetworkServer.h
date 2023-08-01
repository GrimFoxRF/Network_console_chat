//������� ��� �������
#pragma once
#include <string>
#include <winsock2.h>
#include "Utility.h"
#include "DataBase.h"

#pragma comment(lib, "ws2_32.lib") 

#define MESSAGE_LENGTH 1024 // ������������ ������ ������ ��� ������
//#define PORT 7777 // ����� �������������� ���� ����� �����

class NetworkServer {
private:
	int port_;// = PORT;
	WSADATA wsaData;
	SOCKET serverSocket_ = INVALID_SOCKET;
	SOCKET clientSocket_ = INVALID_SOCKET;

	bool createSocket(); //�������� ������
	bool bindSocket(); //���� ������
	bool listenForConnections(); //������������� �����
	bool acceptClientConnection(); //����� ���������� �� �������
	void closeServerSocket(); //�������� ������

public:
	NetworkServer(int port); //����������� ������ NetworkServer
	NetworkServer() {} // ������������� ������� NetworkServer �� ���������
	~NetworkServer();

	void startServer(); //������ ������� �������
	void handleClient(SOCKET clientSocket, DataBase& db); //����� ��������� �� �������
	void stopServer(); //���������� ������� �������

	bool parseMessage(const std::string& message, std::vector<std::string>& arguments);//����� ��� ��������� ���������

};