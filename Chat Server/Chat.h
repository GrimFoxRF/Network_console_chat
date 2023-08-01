//���� ���� � �������
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

	void serverStart(NetworkServer& server); //������ ������� �������

	void showMainMenu(DataBase& db); // ������� ����
	void showLoginMenu(DataBase& db); // ���� ����� ������������
	void showServerMenu(DataBase& db, Settings& settings); //���� ����, � ������� ��������
	void showSettingsMenu(Settings& settings);//���� �������� �������

	bool getAdminOnline() const { return _adminOnline; }

};


