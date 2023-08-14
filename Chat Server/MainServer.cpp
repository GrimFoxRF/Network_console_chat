//Сервер сетевого чата
#include <iostream>
#include <winsock2.h>
#include <Windows.h>
#include "Chat.h"
#include "Utility.h"
#include "Logger.h"

int main() 
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "Russian");

	Chat chat; //объект чат создан
	Logger logger("log.txt");
	DataBase db; //объект база данных создана
	Settings settings;

	settings.loadSettingsFromFile("settings.txt"); //загрузка настроек
	std::string portStr = settings.getSetting("PORT");
	int port = std::stoi(portStr);

	NetworkServer server(port); //объект сервер создан

	chat.start(); //запуск чата
	db.dataBaseConnect();//подключение к базе данных
	db.resetAllUsersStatus();//устанавливаем статус всех пользователей = offline

	while (chat.serverWork()) //основной цикл программы
	{
		chat.showMainMenu(db); //Главное меню сервера
		while (chat.getAdminOnline())
		{
			chat.showServerMenu(db, settings, port, logger); //меню управления сервером
		}
	}

	server.stopServer(); //закрытие сервера
	db.dataBaseDisconnect();//закрытие базы данных

	return 0;
}