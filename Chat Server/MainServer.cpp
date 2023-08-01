//������ �������� ����
#include <iostream>
#include <winsock2.h>
#include <Windows.h>
#include "Chat.h"
#include "Utility.h"

int main() 
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "Russian");

	Chat chat; //������ ��� ������
	DataBase db; //������ ���� ������ �������
	Settings settings;

	settings.loadSettingsFromFile("settings.txt"); //�������� ��������
	std::string portStr = settings.getSetting("PORT");
	int port = std::stoi(portStr);

	NetworkServer server(port); //������ ������ ������

	chat.start(); //������ ����
	db.dataBaseConnect();//����������� � ���� ������
	db.resetAllUsersStatus();//������������� ������ ���� ������������� = offline

	while (chat.serverWork()) //�������� ���� ���������
	{
		chat.showMainMenu(db); //������� ���� �������
		while (chat.getAdminOnline())
		{
			chat.showServerMenu(db, settings); //���� ���������� ��������
		}
	}

	server.stopServer(); //�������� �������
	db.dataBaseDisconnect();//�������� ���� ������

	return 0;
}