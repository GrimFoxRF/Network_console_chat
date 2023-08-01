//Меню чата и функции
#pragma once
#include <string>
#include <vector>
#include <memory>
#include "NetworkServer.h"
#include "DataBase.h"

class Chat
{
protected:
	bool _serverWork = false;
	bool _adminOnline = false;
	std::string _currentUser;

public:
	void start();
	bool serverWork() const;

	std::string setCurrentUser(const std::string& login);
	std::string getCurrentUser() { return _currentUser; }

	void serverStart(NetworkServer& server); //запуск сетевых функций

	void showMainMenu(DataBase& db); // Главное меню
	void showLoginMenu(DataBase& db); // Меню входа пользователя
	void showServerMenu(DataBase& db, Settings& settings); //Меню чата, с выбором действий
	void showSettingsMenu(Settings& settings);//меню настроек сервера

	bool getAdminOnline() const { return _adminOnline; }

};


