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
	Settings settings;

	settings.loadSettingsFromFile("settings.txt"); //�������� �������� �� �����
	std::string portStr = settings.getSetting("PORT");
	std::string IP = settings.getSetting("IP");
	int PORT = std::stoi(portStr);

	NetworkClient client(IP, PORT); //������ ������ ������

	chat.start(); //������ ����

	client.startClient(); //������ ������� �������

	while (chat.chatWork()) //�������� ���� ���������
	{
		chat.showMainMenu(client, settings); //������� ����
		while (chat.getUserOnline())
		{
			if (chat.getCurrentUserName() == "�������������") //���� ����� �����, ��������� ��� ����
			{
				
				chat.showAdminMenu(client);
			}
			else //���� ������� �������������
			{

				chat.showChatMenu(client);
			}
		}
	}

	client.stopClient(); //�������� ���������

	return 0;
}