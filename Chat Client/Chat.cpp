#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>
#include <string>
#include <sstream>
#include "Chat.h"
#include "Utility.h"
#include "NetworkClient.h"


#if defined(max)
#undef max
#endif

void Chat::start()
{
	_chatWork = true;
}

bool Chat::chatWork() const
{
	return _chatWork;
}
//������� ����
void Chat::showMainMenu(NetworkClient& client, Settings& settings)
{
	char choice;

	setUserOnline(false);

	std::cout << "\n\n\t������� ����\n" << std::endl;
	std::cout << "[" << getCurrentTime() << "]" << std::endl; //���� � ����� � ������� ����
	while (!_userOnline && _chatWork)
	{
		std::cout << "\n1 - ���� � ���\n2 - ����������� ������������\n3 - ���������\n4 - �����\n" << std::endl;
		std::cin >> choice;
		switch (choice)
		{
		case('1'):
			showLoginMenu(client);
			break;
		case('2'):
			showRegistrationMenu(client);
			break;
		case('3'):
			showSettingsMenu(settings);
			break;
		case('4'):
			std::cout << "\n\t�����\n" << std::endl;
			_chatWork = false;
			system("pause");
			break;
		default:
			std::cout << "������� ����� �� 1 �� 4" << std::endl;
			break;
		}
	}
}
//���� ����� � ���
void Chat::showLoginMenu(NetworkClient& client)
{
	bool menu = true;
	std::string login;
	std::string password;

	while (menu)
	{
		std::cout << "\t������� �����:\n" << std::endl;
		std::cin >> login;
		std::cout << "\t������� ������:\n" << std::endl;
		std::cin >> password;
		std::string responce = client.loginToServer(login, password);
		if (responce == "log|success")
		{
			std::string name = client.askUserName(login);
			client.setUserStatus(login, 1);
			setCurrentUserLogin(login);
			setCurrentUser(name);
			setUserOnline(true);
			menu = false;
			break;
		}
		else 
		{
			std::cout << "\n������: ������ �� �����" << std::endl;
			menu = false;
			break;
		}
		
	}
}
//���� ����������� ������������
void Chat::showRegistrationMenu(NetworkClient& client)
{
	bool menu = true;
	std::string login;
	std::string password;
	std::string name;

	std::cout << "\t����������� ������ ������������:\n" << std::endl;

	while (menu)
	{
		std::cout << "���������� �����:\n" << std::endl;
		std::cin >> login;

		std::cout << "���������� ������: \n" << std::endl;
		std::cin >> password;

		std::cout << "���������� ��� ������������: \n" << std::endl;
		std::cin >> name;

		std::string response = client.registrationNewUser(login, password, name);
		if (response == "reg|success")
		{
			std::cout << "\n������������ ������� ���������������" << std::endl;
			menu = false;
			break;
		}
		else if(response == "reg|LoginFail")
		{
			std::cout << "\n����� �����" << std::endl;
			menu = false;
			break;
		}
		else if (response == "reg|NameFail")
		{
			std::cout << "\n��� ������" << std::endl;
			menu = false;
			break;
		}
		else 
		{
			std::cout << "\n������: ������������ �� ���������������" << std::endl;
			menu = false;
			break;
		}
	}
}
//���� ��������������
void Chat::showAdminMenu(NetworkClient& client)
{
	char choice;
	std::cout << "\n\t----������� ������������----" << std::endl;
	std::cout << "\n1 - ����� � ����� ���\n2 - ����� � ������ ���\n3 - �������� ���������\n4 - �������� ���� �������������\n5 - ����� �� ����\n" << std::endl;
	std::cout << "\t----������� ��������������----" << std::endl;
	std::cout << "\n6 - �������� ���������� � �������������\n7 - ������������� ������������\n8 - �������������� ������������\n" << std::endl;
	std::cin >> choice;

	switch (choice)
	{
	case ('1'):
		showGeneralChat(client);
		break;
	case ('2'):
		showUserChat(client);
		break;
	case ('3'):
		addMessage(client);
		break;
	case ('4'):
		showAllUsers(client);
		break;
	case ('5'):
		std::cout << "\t�����\n" << std::endl;
		client.setUserStatus(getCurrentUserLogin(), false);
		setUserOnline(false);
		break;
	case ('6'):
		showUsersInfo(client);
		break;
	case('7'):
		setUserBanned(client, true);
		break;
	case ('8'):
		setUserBanned(client, false);
		break;
	}
}
//���� ������������
void Chat::showChatMenu(NetworkClient& client)
{
	char choice;

	std::cout << "\t\n----������� ������������----" << std::endl;
	std::cout << "\n1 - ����� � ����� ���\n2 - ����� � ������ ���\n3 - �������� ���������\n4 - �������� ���� �������������\n5 - ����� �� ����\n" << std::endl;

	std::cin >> choice;

	switch (choice)
	{
	case ('1'):
		showGeneralChat(client);
		break;
	case ('2'):
		showUserChat(client);
		break;
	case ('3'):
		addMessage(client);
		break;
	case ('4'):
		showAllUsers(client);
		break;
	case ('5'):
		std::cout << "\t�����\n" << std::endl;
		client.setUserStatus(getCurrentUserLogin(), false);
		setUserOnline(false);
		break;
	}

}
//����� ���
void Chat::showGeneralChat(NetworkClient& client)
{
	std::vector<std::string> messages;

	messages = client.loadMessagesToAll();

 	std::cout << "\n\t����� ���: \n" << std::endl;
	for (const std::string& message : messages)
	{
		// ���� ������� ������������ '|'
		size_t firstDelimiterPos = message.find('|');
		size_t secondDelimiterPos = message.find('|', firstDelimiterPos + 1);
		size_t thirdDelimiterPos = message.find('|', secondDelimiterPos + 1);

		if (firstDelimiterPos != std::string::npos && secondDelimiterPos != std::string::npos && thirdDelimiterPos != std::string::npos)
		{
			// ��������� time, from � text �� ������
			std::string time = message.substr(0, firstDelimiterPos);
			std::string from = message.substr(firstDelimiterPos + 1, secondDelimiterPos - firstDelimiterPos - 1);
			std::string text = message.substr(secondDelimiterPos + 1, thirdDelimiterPos - secondDelimiterPos - 1);

			// ������� time, from � text �� �����
			std::cout << "[" << time << "] " << from << ": \n\t-----> " << text << std::endl;
		}
	}
}
//������ ���
void Chat::showUserChat(NetworkClient& client)
{
	std::vector<std::string> messages;

	messages = client.loadMessagesToUser(getCurrentUserName());

	std::cout << "\n\t��� ������������: " << getCurrentUserName() << "\n" << std::endl;
	for (const std::string& message : messages)
	{
		// ���� ������� ������������ '|'
		size_t firstDelimiterPos = message.find('|');
		size_t secondDelimiterPos = message.find('|', firstDelimiterPos + 1);
		size_t thirdDelimiterPos = message.find('|', secondDelimiterPos + 1);
		size_t fourthDelimiterPos = message.find('|', thirdDelimiterPos + 1);

		if (firstDelimiterPos != std::string::npos && 
			secondDelimiterPos != std::string::npos && 
			thirdDelimiterPos != std::string::npos &&
			fourthDelimiterPos != std::string::npos)
		{
			// ��������� time, from � text �� ������
			std::string time = message.substr(0, firstDelimiterPos);
			std::string from = message.substr(firstDelimiterPos + 1, secondDelimiterPos - firstDelimiterPos - 1);
			std::string to = message.substr(secondDelimiterPos + 1, thirdDelimiterPos - secondDelimiterPos - 1);
			std::string text = message.substr(thirdDelimiterPos + 1, fourthDelimiterPos - thirdDelimiterPos - 1);

			// ������� time, from, to � text �� �����
			std::cout << "[" << time << "] " << "�� " << from << " ��� " << to <<": \n\t-----> " << text << std::endl;
		}
	}
}
//�������� ������ �������������
void Chat::showAllUsers(NetworkClient& client)
{
	std::vector<std::string> users;

	users = client.loadAllUsersFromDB();

	std::cout << "\n\t������ �������������:\n" << std::endl;
	for (const std::string& user : users)
	{
		// ���� ������� ����������� '|'
		size_t delimiterPos = user.find('|');

		if (delimiterPos != std::string::npos)
		{
			// ��������� ��� ������������ � ������ �� ������
			std::string name = user.substr(0, delimiterPos);
			std::string status = user.substr(delimiterPos + 1);

			// ������� ��� ������������ � ��� ������ �� �����
			std::cout << name << " [" << (status == "1" ? "online" : "offline") << "]" << std::endl;
		}
	}
}
//����������� ������ �������������, ������� ���������������
void Chat::showUsersInfo(NetworkClient& client)
{
	std::vector<std::string> users;

	users = client.loadAllUsersInfo();
	
	std::cout << "\n\t������ �������������:\n" << std::endl;
	for (const std::string& user : users)
	{
		// ���� ������� ������������ '|'
		size_t firstDelimiterPos = user.find('|');
		size_t secondDelimiterPos = user.find('|', firstDelimiterPos + 1);
		size_t thirdDelimiterPos = user.find('|', secondDelimiterPos + 1);

		if (firstDelimiterPos != std::string::npos &&
			secondDelimiterPos != std::string::npos &&
			thirdDelimiterPos != std::string::npos)
		{
			// ��������� ��� ������������, ������ � ���������� � ���� �� ������
			std::string name = user.substr(0, firstDelimiterPos);
			std::string status = user.substr(firstDelimiterPos + 1, secondDelimiterPos - firstDelimiterPos - 1);
			std::string bann = user.substr(secondDelimiterPos + 1, thirdDelimiterPos - secondDelimiterPos - 1);

			// ������� ��� ������������ � ��� ������ �� �����
			std::cout << name << " [" << (status == "1" ? "online" : "offline") << "] " << " [" << (bann == "1" ? "banned" : "not banned") << "] " << std::endl;
		}
	}
}
//���������� ������������
void Chat::setUserBanned(NetworkClient& client, bool bannStatus)
{
	std::string nameForBann;
	std::cout << "\n������� ��� ������������:" << std::endl;
	std::cin >> nameForBann;

	if ((client.checkUserExists(nameForBann) == "ue|Exists") && bannStatus) 
	{
		client.setBannStatus(nameForBann, bannStatus);
		std::cout << "\n������������ " << nameForBann << " ������������" << std::endl;
	}
	else if ((client.checkUserExists(nameForBann) == "ue|Exists") && !bannStatus)
	{
		client.setBannStatus(nameForBann, bannStatus);
		std::cout << "\n������������ " << nameForBann << " �������������" << std::endl;
	}
	else {
		std::cout<< "\n������������ " << nameForBann << " �� ������ � ���� ������" << std::endl;
	}
}
//�������� ���������
void Chat::addMessage(NetworkClient& client) 
{
	std::string from = getCurrentUserName();
	std::string to;
	std::string text;

	std::cout << "\n������� ��� ������������ ��� all - �������� ��������� ����:\n" << std::endl;
	std::cin >> to;
	if (client.checkUserExists(to) == "ue|Exists") 
	{
		std::cout << "\n�������� ����� ���������: \n" << std::endl;
		std::cin.ignore();
		getline(std::cin, text);

		if (to == "all" || to == "All" || to == "ALL")
		{
			client.sendMessageToALL(from, to, text); //��� ����
		}
		else
		{
			client.sendMessageToDB(from, to, text); //��� ������������
		}
	}
	else 
	{
		std::cout << "\n����� ������������ �� ������" << std::endl;
	}
	
}
void Chat::showSettingsMenu(Settings& settings)
{
	char choice;
	std::string value;
	std::cout << "\n1 - �������� IP-�����\n2 - �������� ����� �����\n3 - �����\n" << std::endl;

	std::cin >> choice;

	switch (choice)
	{
	case ('1'):
		std::cout << "\n������� ����� IP-�����: ";
		std::cin.clear();
		std::cin >> value;

		settings.setSetting("IP", value);
		settings.saveSettingsToFile("settings.txt");

		break;
	case ('2'):
		std::cout << "\n������� ����� ����� �����: ";
		std::cin.clear();

		std::cin >> value;
		if (std::stoi(value) >= 7777 && std::stoi(value) <= 17777)
		{
			settings.setSetting("PORT", value);
			settings.saveSettingsToFile("settings.txt");
		}
		else
		{
			std::cout << "\n������������ ����" << std::endl;
		}
		break;
	case('3'):

		break;
	default:

		break;
	}
}