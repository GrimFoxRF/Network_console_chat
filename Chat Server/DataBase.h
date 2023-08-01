#pragma once
#include <iostream>
#include <mysql.h>

class DataBase {
private:
	MYSQL mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;

	void createDataBase();//�������� ����� ���� ������, ���� �� ������� ������
	void createTables();//�������� ������, ���� ��� �����������
	void createAdmin();//�������� ������������ � ������� ��������������, ���� ������ ���

public:
	void dataBaseConnect(); // ����������� � ���� ������
	void dataBaseDisconnect(); // ���������� �� ���� ������

	bool checkLogin(const std::string& login); //�������� ������ ��� ����� �� ������
	bool checkPassword(const std::string& password);//�������� ������ ��� ����� �� ������

	bool checkUserLogin(const std::string& login);//�������� ������ ������������
	bool checkUserPassword(const std::string& password);//�������� ������ ������������
	bool isBanned(const std::string& login);//�������� ������� �� ������������

	std::string takeUserName(const std::string& login);//���������� ��� ������������ �� ������

	bool checkLoginExistsInDB(const std::string& login);//�������� �� ����� �� �����
	bool checkNameExistsInDB(const std::string& name);//�������� �� ������ �� ���

	bool addNewUser(const std::string& login, const std::string& password, const std::string& name);//��������� ������ ������������ � ��
	std::string takeAllUsersNameStatus();//���������� ������ ���� ������������� � �� ������

	std::string takeAllUsersNameStatusIsBann();//���������� ��� ������
	void setBannStatus(const std::string& name, bool bann);//�������� ������ ���� ������������

	void setUserStatus(const std::string& login, bool online);//������������� ������ ������������ online/offline
	void resetAllUsersStatus();//������������� ������� ���� ������������� = 0

	void addMessageToAll(const std::string& from, const std::string& to, const std::string& text, bool toAll);//���������� ������ ��������� ��� ���� � ���� ������
	void addMessageToDB(const std::string& from, const std::string& to, const std::string& text);//���������� ������ ��������� ��� ������������ � ���� ������
	
	std::string loadMessagesToAll();//��������� ��� ���� �������������
	std::string loadMessagesToUser(const std::string& to);//��������� ��� �������� ������������
	
	//////////////////////////////////�������� �����////////////////////////////
	//������� ������ ������������� �� ��
	void showAllUsers();
};
