//Клиент сетевого чата
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

	Chat chat; //объект чат создан
	Settings settings;

	settings.loadSettingsFromFile("settings.txt"); //загрузка настроек из файла
	std::string portStr = settings.getSetting("PORT");
	std::string IP = settings.getSetting("IP");
	int PORT = std::stoi(portStr);

	NetworkClient client(IP, PORT); //объект клиент создан

	chat.start(); //запуск чата

	client.startClient(); //запуск сетевых функций

	while (chat.chatWork()) //основной цикл программы
	{
		chat.showMainMenu(client, settings); //Главное меню
		while (chat.getUserOnline())
		{
			if (chat.getCurrentUserName() == "Администратор") //если вошел админ, откроется его меню
			{
				
				chat.showAdminMenu(client);
			}
			else //меню простых пользователей
			{

				chat.showChatMenu(client);
			}
		}
	}

	client.stopClient(); //закрытие соединеия

	return 0;
}