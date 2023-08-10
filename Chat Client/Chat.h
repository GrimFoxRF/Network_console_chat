//Основной функционал чата
#pragma once
#include <string>
#include <vector>
#include <memory>
#include "NetworkClient.h"

class Chat
{
protected:
	bool _chatWork = false;
	bool _userOnline = false;
	std::string _currentUserLogin;
	std::string _currentUserName;

public:

	void start();
	bool chatWork() const;
	
	void showMainMenu(NetworkClient& client, Settings& settings); // Главное меню
	void showLoginMenu(NetworkClient& client); // Меню входа пользователя
	void showRegistrationMenu(NetworkClient& client); // Меню регистрации нового пользователя

	void setUserOnline(bool value) { _userOnline = value; }//установить что пользователь находится в чате
	bool getUserOnline() { return _userOnline; }//показывает  находится ли пользователь в чате

	void setCurrentUserLogin(std::string login) { _currentUserLogin = login; }//устанавливает значение текущего пользователя по логину
	std::string getCurrentUserLogin() { return _currentUserLogin; }//выдает логин текущего пользователя

	void setCurrentUser(std::string name) { _currentUserName = name; }//устанавливает значение текущего пользователя по имени
	std::string getCurrentUserName() { return _currentUserName; }//выдает имя текущего пользователя

	void showAdminMenu(NetworkClient& client);//меню с функционалом администратора
	void showChatMenu(NetworkClient& client);//меню пользователя

	void showGeneralChat(NetworkClient& client);//Общий чат
	void showUserChat(NetworkClient& client);//Личный чат
	void showAllUsers(NetworkClient& client);//Показать список пользователей
	void addMessage(NetworkClient& client);//Написать сообщение
	//функции Админа
	void showUsersInfo(NetworkClient& client);// информация для администратора о пользователях
	void setUserBanned(NetworkClient& client, bool bannStatus);//блокировка пользователя

	void showSettingsMenu(Settings& settings);//меню настроек клиента
};


