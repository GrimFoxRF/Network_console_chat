//�������� ���������� ����
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
	
	void showMainMenu(NetworkClient& client, Settings& settings); // ������� ����
	void showLoginMenu(NetworkClient& client); // ���� ����� ������������
	void showRegistrationMenu(NetworkClient& client); // ���� ����������� ������ ������������

	void setUserOnline(bool value) { _userOnline = value; }//���������� ��� ������������ ��������� � ����
	bool getUserOnline() { return _userOnline; }//����������  ��������� �� ������������ � ����

	void setCurrentUserLogin(std::string login) { _currentUserLogin = login; }//������������� �������� �������� ������������ �� ������
	std::string getCurrentUserLogin() { return _currentUserLogin; }//������ ����� �������� ������������

	void setCurrentUser(std::string name) { _currentUserName = name; }//������������� �������� �������� ������������ �� �����
	std::string getCurrentUserName() { return _currentUserName; }//������ ��� �������� ������������

	void showAdminMenu(NetworkClient& client);//���� � ������������ ��������������
	void showChatMenu(NetworkClient& client);//���� ������������

	void showGeneralChat(NetworkClient& client);//����� ���
	void showUserChat(NetworkClient& client);//������ ���
	void showAllUsers(NetworkClient& client);//�������� ������ �������������
	void addMessage(NetworkClient& client);//�������� ���������
	//������� ������
	void showUsersInfo(NetworkClient& client);// ���������� ��� �������������� � �������������
	void setUserBanned(NetworkClient& client, bool bannStatus);//���������� ������������

	void showSettingsMenu(Settings& settings);//���� �������� �������
};


