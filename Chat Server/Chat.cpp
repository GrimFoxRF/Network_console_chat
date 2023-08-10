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
//запуск сетевых функций
void Chat::serverStart(NetworkServer& server)
{
	server.startServer();
}
//Главное меню с выбором входа, регистрации или выхода из программы, отображает текущую дату
void Chat::showMainMenu(DataBase& db)
{
	char choice;

	_adminOnline = false;

	std::cout << "\n\n\tГлавное Меню Сервера\n" << std::endl;
	std::cout << "[" << getCurrentTime() << "]" << std::endl; //Дата и время в главном меню
	while(!_adminOnline && _serverWork)
	{ 
		std::cout << "\n1 - Вход на Сервер\n2 - Выход\n" << std::endl;
		std::cin >> choice;
		switch (choice) 
		{
		case('1'):
			showLoginMenu(db);
			break;
		case('2'):
			std::cout << "\n\tВыход\n" << std::endl;
			_serverWork = false;
			system("pause");
			break;
		default:
			std::cout << "Введите 1 или 2" << std::endl;
			break;
		}
		
	}
}
//Функция проверки логина и пароля при входе в чат
void Chat::showLoginMenu(DataBase& db)
{
	bool menu = true;
	char choice;
	std::string login;
	std::string password;

	while (menu)
	{
		std::cout << "\tВведите логин:\n" << std::endl;
		std::cin >> login;

		if (login != "admin") {
			std::cout << "\nНеправильно введен логин\n" << std::endl;
			menu = false;
			_adminOnline = false;
			break;
		}

		if (!db.checkLogin(login))
		{
			std::cout << "\nНеправильно введен логин\n" << std::endl;
			std::cout << "Нажмите любую кнопку для повторного ввода или \"0\" для выхода\n" << std::endl;
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
			std::cout << "\tВведите пароль:\n" << std::endl;
			std::cin >> password;

			if (!db.checkPassword(password))
			{
				std::cout << "\nНеправильно введен пароль\n" << std::endl;
				std::cout << "Нажмите любую кнопку для повторного ввода или \"0\" для выхода\n" << std::endl;
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
//Меню сервера
void Chat::showServerMenu(DataBase& db, Settings& settings)
{
	char choice;

	std::cout << "\nПользователь: Администратор" << std::endl;

	NetworkServer server;

	while (_adminOnline)
	{
		std::cout << "\n1 - Запустить Сервер\n2 - Настройки сервера\n3 - Отключить сервер\n" << std::endl;

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
			std::cout << "\tВыход\n" << std::endl;
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
	std::cout << "\n1 - Вести новый порт\n2 - Назад\n" << std::endl;

	std::cin >> choice;
	
	switch (choice)
	{
	case ('1'):
		std::cout << "\nВведите новый номер порта: ";
		std::cin.clear();
		
		std::cin >> value;
		if (std::stoi(value) >= 7777 && std::stoi(value) <= 17777) 
		{
			settings.setSetting("PORT", value);
			settings.saveSettingsToFile("settings.txt");
		}
		else 
		{
			std::cout << "\nНедопустимый порт" << std::endl;
		}
		break;
	case('2'):

		break;
	default:

		break;
	}
}
