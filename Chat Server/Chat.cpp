#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>
#include <string>
#include "Chat.h"
#include "Utility.h"
#include "NetworkServer.h"

#if defined(max)
#undef max
#endif

void Chat::start()
{
	_serverWork = true;
}

bool Chat::serverWork() const
{
	return _serverWork;
}

std::string Chat::setCurrentUser(const std::string& login)
{
	return _currentUser = login;
}
//������ ������� �������
void Chat::serverStart(NetworkServer& server)
{
	server.startServer();
}
//������� ���� � ������� �����, ����������� ��� ������ �� ���������, ���������� ������� ����
void Chat::showMainMenu(DataBase& db)
{
	char choice;

	_adminOnline = false;

	std::cout << "\n\n\t������� ���� �������\n" << std::endl;
	std::cout << "[" << getCurrentTime() << "]" << std::endl; //���� � ����� � ������� ����
	while(!_adminOnline && _serverWork)
	{ 
		std::cout << "\n1 - ���� �� ������\n2 - �����\n" << std::endl;
		std::cin >> choice;
		switch (choice) 
		{
		case('1'):
			showLoginMenu(db);
			break;
		case('2'):
			std::cout << "\n\t�����\n" << std::endl;
			_serverWork = false;
			system("pause");
			break;
		default:
			std::cout << "������� 1 ��� 2" << std::endl;
			break;
		}
		
	}
}
//������� �������� ������ � ������ ��� ����� � ���
void Chat::showLoginMenu(DataBase& db)
{
	bool menu = true;
	char choice;
	std::string login;
	std::string password;

	while (menu)
	{
		std::cout << "\t������� �����:\n" << std::endl;
		std::cin >> login;

		if (login != "admin") {
			std::cout << "\n����������� ������ �����\n" << std::endl;
			menu = false;
			_adminOnline = false;
			break;
		}

		if (!db.checkLogin(login))
		{
			std::cout << "\n����������� ������ �����\n" << std::endl;
			std::cout << "������� ����� ������ ��� ���������� ����� ��� \"0\" ��� ������\n" << std::endl;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cin >> choice;

			if (choice == '0')
			{
				menu = false;
				_adminOnline = false;
				break;
			}
			_adminOnline = false;
		}
		else
		{
			std::cout << "\t������� ������:\n" << std::endl;
			std::cin >> password;

			if (!db.checkPassword(password))
			{
				std::cout << "\n����������� ������ ������\n" << std::endl;
				std::cout << "������� ����� ������ ��� ���������� ����� ��� \"0\" ��� ������\n" << std::endl;
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				std::cin >> choice;

				if (choice == '0')
				{
					menu = false;
					_adminOnline = false;
					break;
				}
				_adminOnline = false;
				break;
			}
			setCurrentUser(login);
			db.setUserStatus(login, true);
			_adminOnline = true;
			break;
		}
	}
}
//���� �������
void Chat::showServerMenu(DataBase& db, Settings& settings)
{
	char choice;

	std::cout << "\n������������: �������������" << std::endl;

	NetworkServer server;

	while (_adminOnline)
	{
		std::cout << "\n1 - ��������� ������\n2 - ��������� �������\n3 - ��������� ������\n" << std::endl;

		std::cin >> choice;

		switch (choice)
		{
		case ('1'):
			serverStart(server);
			break;
		case ('2'):
			showSettingsMenu(settings);
			break;
		case ('3'):
			std::cout << "\t�����\n" << std::endl;
			db.setUserStatus(getCurrentUser(), false);
			_adminOnline = false;
			break;
		default:

			break;
		}
		
	}
}

void Chat::showSettingsMenu(Settings& settings)
{
	char choice;
	std::string value;
	std::cout << "\n1 - ����� ����� ����\n2 - �����\n" << std::endl;

	std::cin >> choice;
	
	switch (choice)
	{
	case ('1'):
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
	case('2'):

		break;
	default:

		break;
	}
}
