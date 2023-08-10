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
//главное меню
void Chat::showMainMenu(NetworkClient& client, Settings& settings)
{
	char choice;

	setUserOnline(false);

	std::cout << "\n\n\tГлавное Меню\n" << std::endl;
	std::cout << "[" << getCurrentTime() << "]" << std::endl; //Дата и время в главном меню
	while (!_userOnline && _chatWork)
	{
		std::cout << "\n1 - Вход в Чат\n2 - Регистрация пользователя\n3 - Настройки\n4 - Выход\n" << std::endl;
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
			std::cout << "\n\tВыход\n" << std::endl;
			_chatWork = false;
			system("pause");
			break;
		default:
			std::cout << "Введите число от 1 до 4" << std::endl;
			break;
		}
	}
}
//меню входа в чат
void Chat::showLoginMenu(NetworkClient& client)
{
	bool menu = true;
	std::string login;
	std::string password;

	while (menu)
	{
		std::cout << "\tВведите логин:\n" << std::endl;
		std::cin >> login;
		std::cout << "\tВведите пароль:\n" << std::endl;
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
			std::cout << "\nОШИБКА: данные не верны" << std::endl;
			menu = false;
			break;
		}
		
	}
}
//меню регистрации пользователя
void Chat::showRegistrationMenu(NetworkClient& client)
{
	bool menu = true;
	std::string login;
	std::string password;
	std::string name;

	std::cout << "\tРегистрация нового пользователя:\n" << std::endl;

	while (menu)
	{
		std::cout << "Придумайте логин:\n" << std::endl;
		std::cin >> login;

		std::cout << "Придумайте пароль: \n" << std::endl;
		std::cin >> password;

		std::cout << "Придумайте имя пользователя: \n" << std::endl;
		std::cin >> name;

		std::string response = client.registrationNewUser(login, password, name);
		if (response == "reg|success")
		{
			std::cout << "\nПользователь успешно зарегистрирован" << std::endl;
			menu = false;
			break;
		}
		else if(response == "reg|LoginFail")
		{
			std::cout << "\nЛогин занят" << std::endl;
			menu = false;
			break;
		}
		else if (response == "reg|NameFail")
		{
			std::cout << "\nИмя занято" << std::endl;
			menu = false;
			break;
		}
		else 
		{
			std::cout << "\nОШИБКА: пользователь не зарегистрирован" << std::endl;
			menu = false;
			break;
		}
	}
}
//меню администратора
void Chat::showAdminMenu(NetworkClient& client)
{
	char choice;
	std::cout << "\n\t----Функции пользователя----" << std::endl;
	std::cout << "\n1 - Войти в общий чат\n2 - Войти в личный чат\n3 - Написать сообщение\n4 - Показать всех пользователей\n5 - Выйти из чата\n" << std::endl;
	std::cout << "\t----Функции администратора----" << std::endl;
	std::cout << "\n6 - Показать информацию о пользователях\n7 - Заблокировать пользователя\n8 - Разблокировать пользователя\n" << std::endl;
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
		std::cout << "\tВыход\n" << std::endl;
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
//меню пользователя
void Chat::showChatMenu(NetworkClient& client)
{
	char choice;

	std::cout << "\t\n----Функции пользователя----" << std::endl;
	std::cout << "\n1 - Войти в общий чат\n2 - Войти в личный чат\n3 - Написать сообщение\n4 - Показать всех пользователей\n5 - Выйти из чата\n" << std::endl;

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
		std::cout << "\tВыход\n" << std::endl;
		client.setUserStatus(getCurrentUserLogin(), false);
		setUserOnline(false);
		break;
	}

}
//Общий чат
void Chat::showGeneralChat(NetworkClient& client)
{
	std::vector<std::string> messages;

	messages = client.loadMessagesToAll();

 	std::cout << "\n\tОбщий чат: \n" << std::endl;
	for (const std::string& message : messages)
	{
		// Ищем позиции разделителей '|'
		size_t firstDelimiterPos = message.find('|');
		size_t secondDelimiterPos = message.find('|', firstDelimiterPos + 1);
		size_t thirdDelimiterPos = message.find('|', secondDelimiterPos + 1);

		if (firstDelimiterPos != std::string::npos && secondDelimiterPos != std::string::npos && thirdDelimiterPos != std::string::npos)
		{
			// Извлекаем time, from и text из строки
			std::string time = message.substr(0, firstDelimiterPos);
			std::string from = message.substr(firstDelimiterPos + 1, secondDelimiterPos - firstDelimiterPos - 1);
			std::string text = message.substr(secondDelimiterPos + 1, thirdDelimiterPos - secondDelimiterPos - 1);

			// Выводим time, from и text на экран
			std::cout << "[" << time << "] " << from << ": \n\t-----> " << text << std::endl;
		}
	}
}
//Личный чат
void Chat::showUserChat(NetworkClient& client)
{
	std::vector<std::string> messages;

	messages = client.loadMessagesToUser(getCurrentUserName());

	std::cout << "\n\tЧат пользователя: " << getCurrentUserName() << "\n" << std::endl;
	for (const std::string& message : messages)
	{
		// Ищем позиции разделителей '|'
		size_t firstDelimiterPos = message.find('|');
		size_t secondDelimiterPos = message.find('|', firstDelimiterPos + 1);
		size_t thirdDelimiterPos = message.find('|', secondDelimiterPos + 1);
		size_t fourthDelimiterPos = message.find('|', thirdDelimiterPos + 1);

		if (firstDelimiterPos != std::string::npos && 
			secondDelimiterPos != std::string::npos && 
			thirdDelimiterPos != std::string::npos &&
			fourthDelimiterPos != std::string::npos)
		{
			// Извлекаем time, from и text из строки
			std::string time = message.substr(0, firstDelimiterPos);
			std::string from = message.substr(firstDelimiterPos + 1, secondDelimiterPos - firstDelimiterPos - 1);
			std::string to = message.substr(secondDelimiterPos + 1, thirdDelimiterPos - secondDelimiterPos - 1);
			std::string text = message.substr(thirdDelimiterPos + 1, fourthDelimiterPos - thirdDelimiterPos - 1);

			// Выводим time, from, to и text на экран
			std::cout << "[" << time << "] " << "От " << from << " для " << to <<": \n\t-----> " << text << std::endl;
		}
	}
}
//Показать список пользователей
void Chat::showAllUsers(NetworkClient& client)
{
	std::vector<std::string> users;

	users = client.loadAllUsersFromDB();

	std::cout << "\n\tСписок пользователей:\n" << std::endl;
	for (const std::string& user : users)
	{
		// Ищем позицию разделителя '|'
		size_t delimiterPos = user.find('|');

		if (delimiterPos != std::string::npos)
		{
			// Извлекаем имя пользователя и статус из строки
			std::string name = user.substr(0, delimiterPos);
			std::string status = user.substr(delimiterPos + 1);

			// Выводим имя пользователя и его статус на экран
			std::cout << name << " [" << (status == "1" ? "online" : "offline") << "]" << std::endl;
		}
	}
}
//расширенный список пользователей, включая заблокированных
void Chat::showUsersInfo(NetworkClient& client)
{
	std::vector<std::string> users;

	users = client.loadAllUsersInfo();
	
	std::cout << "\n\tСписок пользователей:\n" << std::endl;
	for (const std::string& user : users)
	{
		// Ищем позиции разделителей '|'
		size_t firstDelimiterPos = user.find('|');
		size_t secondDelimiterPos = user.find('|', firstDelimiterPos + 1);
		size_t thirdDelimiterPos = user.find('|', secondDelimiterPos + 1);

		if (firstDelimiterPos != std::string::npos &&
			secondDelimiterPos != std::string::npos &&
			thirdDelimiterPos != std::string::npos)
		{
			// Извлекаем имя пользователя, статус и информацию о бане из строки
			std::string name = user.substr(0, firstDelimiterPos);
			std::string status = user.substr(firstDelimiterPos + 1, secondDelimiterPos - firstDelimiterPos - 1);
			std::string bann = user.substr(secondDelimiterPos + 1, thirdDelimiterPos - secondDelimiterPos - 1);

			// Выводим имя пользователя и его статус на экран
			std::cout << name << " [" << (status == "1" ? "online" : "offline") << "] " << " [" << (bann == "1" ? "banned" : "not banned") << "] " << std::endl;
		}
	}
}
//блокировка пользователя
void Chat::setUserBanned(NetworkClient& client, bool bannStatus)
{
	std::string nameForBann;
	std::cout << "\nВведите имя пользователя:" << std::endl;
	std::cin >> nameForBann;

	if ((client.checkUserExists(nameForBann) == "ue|Exists") && bannStatus) 
	{
		client.setBannStatus(nameForBann, bannStatus);
		std::cout << "\nПользователь " << nameForBann << " заблокирован" << std::endl;
	}
	else if ((client.checkUserExists(nameForBann) == "ue|Exists") && !bannStatus)
	{
		client.setBannStatus(nameForBann, bannStatus);
		std::cout << "\nПользователь " << nameForBann << " разблокирован" << std::endl;
	}
	else {
		std::cout<< "\nПользователь " << nameForBann << " не найден в базе данных" << std::endl;
	}
}
//Написать сообщение
void Chat::addMessage(NetworkClient& client) 
{
	std::string from = getCurrentUserName();
	std::string to;
	std::string text;

	std::cout << "\nВведите имя пользователя или all - отпрвить сообщение Всем:\n" << std::endl;
	std::cin >> to;
	if (client.checkUserExists(to) == "ue|Exists") 
	{
		std::cout << "\nНапишите текст сообщения: \n" << std::endl;
		std::cin.ignore();
		getline(std::cin, text);

		if (to == "all" || to == "All" || to == "ALL")
		{
			client.sendMessageToALL(from, to, text); //для всех
		}
		else
		{
			client.sendMessageToDB(from, to, text); //для пользователя
		}
	}
	else 
	{
		std::cout << "\nТакой пользователь не найден" << std::endl;
	}
	
}
void Chat::showSettingsMenu(Settings& settings)
{
	char choice;
	std::string value;
	std::cout << "\n1 - Изменить IP-адрес\n2 - Изменить номер порта\n3 - Назад\n" << std::endl;

	std::cin >> choice;

	switch (choice)
	{
	case ('1'):
		std::cout << "\nВведите новый IP-адрес: ";
		std::cin.clear();
		std::cin >> value;

		settings.setSetting("IP", value);
		settings.saveSettingsToFile("settings.txt");

		break;
	case ('2'):
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
	case('3'):

		break;
	default:

		break;
	}
}