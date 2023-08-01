#include <iostream>
#include <sstream>
#include <mysql.h>
#include "DataBase.h"
#include <filesystem>
#include <string>

//�������� �� ���� �����
void DataBase::createDataBase()
{
	if (mysql_select_db(&mysql, "chat_database") == 0) {
		std::cout << "���� ������ ������ � ������" << std::endl;
		return;
	}

	if (mysql_query(&mysql, "CREATE DATABASE chat_database") == 0) {
		std::cout << "���� ������ �����������. ������� ����� ���� ������" << std::endl;
	}
	else {
		std::cout << "������: �� ������� ������� ���� ������" << mysql_error(&mysql) << std::endl;
	}
}
//�������� ������ ���� �����
void DataBase::createTables()
{
    // �������� ������� ������ users � messages
    const char* checkTablesQuery = "SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES "
                                   "WHERE TABLE_SCHEMA = 'chat_database' "
                                   "AND TABLE_NAME IN ('users', 'messages')";

    if (mysql_query(&mysql, checkTablesQuery) != 0) {
        std::cout << "������: �� ������� ��������� �������" << mysql_error(&mysql) << std::endl;
        return;
    }

    res = mysql_store_result(&mysql);

    if (res == nullptr) {
        std::cout << "������: �� ������� �������� ��������� �������" << mysql_error(&mysql) << std::endl;
        return;
    }

    int numTables = mysql_num_rows(res);

    mysql_free_result(res);

    if (numTables == 2) {
        std::cout << "������� ���������" << std::endl;
        return;
    }
    else 
    {
        // ������� users
        const char* createUsersTable = "CREATE TABLE IF NOT EXISTS users ("
            "id INT AUTO_INCREMENT PRIMARY KEY,"
            "login VARCHAR(255) NOT NULL UNIQUE,"
            "password VARCHAR(255) NOT NULL,"
            "name VARCHAR(255) NOT NULL UNIQUE,"
            "online BOOLEAN DEFAULT FALSE,"
            "isbanned BOOLEAN DEFAULT FALSE"
            ")";
        //������� messages
        const char* createMessagesTable = "CREATE TABLE IF NOT EXISTS messages ("
            "id INT AUTO_INCREMENT PRIMARY KEY,"
            "from_id VARCHAR(255) NOT NULL,"
            "to_id VARCHAR(255) NOT NULL,"
            "to_all BOOLEAN DEFAULT FALSE,"
            "text TEXT NOT NULL,"
            "timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
            ")";

        // ��������, ��� ������� ������� �������
        if (mysql_query(&mysql, createUsersTable) == 0 &&
            mysql_query(&mysql, createMessagesTable) == 0) {
            std::cout << "������� ������� �������" << std::endl;
        }
        else {
            std::cout << "������: �� ������� ������� �������" << mysql_error(&mysql) << std::endl;
        }
    }
}
//�������� ������������ admin �� ���������
void DataBase::createAdmin()
{
    const char* selectAdmin = "SELECT * FROM users WHERE login='admin'";

    if (mysql_query(&mysql, selectAdmin) != 0) {
        std::cout << "������ ��� ���������� ������� SELECT: " << mysql_error(&mysql) << std::endl;
        mysql_close(&mysql);
        return;
    }

    MYSQL_RES* result = mysql_store_result(&mysql);
    if (result != NULL) {
        if (mysql_num_rows(result) > 0) {
            std::cout << "������������� �����" << std::endl;
        }
        else {
            const char* insertAdmin = "INSERT INTO users (login, password, name) VALUES ('admin', 'admin', '�������������')";

            if (mysql_query(&mysql, insertAdmin) == 0) {
                std::cout << "������������� ������" << std::endl;
            }
            else {
                std::cout << "������ ��� ���������� ������� INSERT: " << mysql_error(&mysql) << std::endl;
            }
        }

        mysql_free_result(result);
    }
    else {
        std::cout << "������ ��� ��������� ���������� ������� SELECT: " << mysql_error(&mysql) << std::endl;
    }
}
//���������� � ��
void DataBase::dataBaseConnect()
{
	mysql_init(&mysql);
	if (&mysql == nullptr) 
    {
		std::cout << "������: �� ������� ������� MySQL-����������" << std::endl;
	}

	// ������������ � �������
	if (!mysql_real_connect(&mysql, "localhost", "root", "root", "chat_database", NULL, NULL, 0)) 
    {
		std::cout << "������: �� ������� ������������ � ���� ������ " << mysql_error(&mysql) << std::endl;
	}
	else 
    {
		std::cout << "���������� � ����� ������ �����������" << std::endl;
	}

    mysql_set_character_set(&mysql, "cp1251");
    //������� ���������� �� ��������� �� ������, �� ��������� ��� latin1
    std::cout << "��������� ���������: " << mysql_character_set_name(&mysql) << std::endl;

	createDataBase(); //��������� ���� ������ �� �������������, ���� ��� - ������� �����
    createTables(); //�������� ������ � �������� �����, ���� ����������
    createAdmin();////�������� ������������ � ������� ��������������, ���� ������ ���

}
//���������� �� ��
void DataBase::dataBaseDisconnect()
{
	mysql_close(&mysql);
	std::cout << "���� ������ ���������..." << std::endl;
}
//�������� ������ �� ����� �� ������
bool DataBase::checkLogin(const std::string& login)
{
    std::string query = "SELECT * FROM users WHERE login='" + login + "'";

    if (mysql_query(&mysql, query.c_str()) != 0) 
    {
        std::cout << "������ ��� ���������� ������� SELECT: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    MYSQL_RES* result = mysql_store_result(&mysql);
    if (result == nullptr)
    {
        std::cout << "������ ��� ��������� ���������� �������: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    int num_rows = mysql_num_rows(result);

    mysql_free_result(result);
    
    return (num_rows > 0);
}
//�������� ������ �� ����� �� ������
bool DataBase::checkPassword(const std::string& password)
{
    std::string query = "SELECT * FROM users WHERE password='" + password + "'";

    if (mysql_query(&mysql, query.c_str()) != 0)
    {
        std::cout << "������ ��� ���������� ������� SELECT: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    MYSQL_RES* result = mysql_store_result(&mysql);
    if (result == nullptr)
    {
        std::cout << "������ ��� ��������� ���������� �������: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    int num_rows = mysql_num_rows(result);

    mysql_free_result(result); 

    return (num_rows > 0);
}
//������ ��� �������� ������ ������������
bool DataBase::checkUserLogin(const std::string& login)
{
    std::string query = "SELECT * FROM users WHERE login='" + login + "'";

    if (mysql_query(&mysql, query.c_str()) != 0)
    {
        std::cout << "������ ��� ���������� ������� SELECT: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    MYSQL_RES* result = mysql_store_result(&mysql);
    if (result == nullptr)
    {
        std::cout << "������ ��� ��������� ���������� �������: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    int num_rows = mysql_num_rows(result);

    mysql_free_result(result); 

    return (num_rows > 0);
}
//������ ��� �������� ������ ������������
bool DataBase::checkUserPassword(const std::string& password)
{
    std::string query = "SELECT * FROM users WHERE password='" + password + "'";

    if (mysql_query(&mysql, query.c_str()) != 0)
    {
        std::cout << "������ ��� ���������� ������� SELECT: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    MYSQL_RES* result = mysql_store_result(&mysql);
    if (result == nullptr)
    {
        std::cout << "������ ��� ��������� ���������� �������: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    int num_rows = mysql_num_rows(result);

    mysql_free_result(result); 

    return (num_rows > 0);
}
//�������� ������� ����������
bool DataBase::isBanned(const std::string& login)
{
    std::string query = "SELECT isbanned FROM users WHERE login = '" + login + "';";

    if (mysql_query(&mysql, query.c_str()))
    {
        std::cerr << "������ ���������� �������: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    res = mysql_store_result(&mysql);
    if (res == nullptr)
    {
        std::cerr << "������ ��������� ���������� �������: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    bool isBanned = false;

    // ���� ������������ ������, �������� �������� ������� isbanned
    if (mysql_num_rows(res) > 0)
    {
        row = mysql_fetch_row(res);
        isBanned = (std::stoi(row[0]) != 0); // ����������� �������� � bool
    }

    // ����������� ��������� �������
    mysql_free_result(res);

    return isBanned;
}
//������ �� ��������� ����� ������������ �� ������
std::string DataBase::takeUserName(const std::string& login)
{
    std::string userName; 

    std::string query = "SELECT name FROM users WHERE login='" + login + "'";

    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        res = mysql_store_result(&mysql);

        if (res != nullptr)
        {
            row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                userName = row[0];
            }
            mysql_free_result(res);
        }
    }
    else
    {
        std::cerr << "������ ���������� �������: " << mysql_error(&mysql) << std::endl;
    }

    return userName;
}
//�������� ������������� ������ � ��
bool DataBase::checkLoginExistsInDB(const std::string& login)
{
    std::string query = "SELECT * FROM users WHERE login='" + login + "'";

    if (mysql_query(&mysql, query.c_str()) != 0) {
        std::cerr << "������ ��� ���������� ������� SELECT: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    MYSQL_RES* result = mysql_store_result(&mysql);
    if (result != nullptr) {
        int num_rows = mysql_num_rows(result);

        mysql_free_result(result);

        return (num_rows > 0);
    }
    else {
        std::cerr << "������ ��� ��������� ���������� ������� SELECT: " << mysql_error(&mysql) << std::endl;
        mysql_free_result(result);
        return false;
    }
}
//�������� ������������� ����� � ��
bool DataBase::checkNameExistsInDB(const std::string& name)
{
    if (name == "all" || name == "All" || name == "ALL") {
        return true;
    }
    
    std::string query = "SELECT * FROM users WHERE name='" + name + "'";

    if (mysql_query(&mysql, query.c_str()) != 0) {
        std::cout << "������ ��� ���������� ������� SELECT: " << mysql_error(&mysql) << std::endl;
        return false;
    }

    res = mysql_store_result(&mysql);
    if (res != nullptr) {
        int num_rows = mysql_num_rows(res);

        mysql_free_result(res);
        return (num_rows > 0);
    }
    else {
        std::cout << "������ ��� ��������� ���������� ������� SELECT: " << mysql_error(&mysql) << std::endl;
        mysql_free_result(res);
        return false;
    }
}
//���������� ������������ � ��
bool DataBase::addNewUser(const std::string& login, const std::string& password, const std::string& name)
{
    std::string query = "INSERT INTO users (login, password, name) VALUES ('" + login + "', '" + password + "', '" + name + "')";

    if (mysql_query(&mysql, query.c_str()) == 0) {
        std::cout << "����� ������������ �������� � ���� ������" << std::endl;
        return true;
    }
    else {
        std::cout << "������ ��� ���������� ������� INSERT: " << mysql_error(&mysql) << std::endl;
        return false;
    }
}
//������ ������������� � �� ������
std::string DataBase::takeAllUsersNameStatus()
{
    std::string allUsersData; 

    std::string query = "SELECT name, online FROM users WHERE isbanned = 0";

    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        res = mysql_store_result(&mysql);

        if (res != nullptr)
        {
            int numFields = mysql_num_fields(res);

            // ������� ����� � ������������ �������
            while ((row = mysql_fetch_row(res)))
            {
                for (int i = 0; i < numFields; ++i)
                {
                    std::string name = row[i++];    
                    std::string status = row[i];   
                    allUsersData += "|" + name + "|" + status;
                }
            }
            mysql_free_result(res);
        }
    }
    else
    {
        std::cerr << "������ ���������� �������: " << mysql_error(&mysql) << std::endl;
    }

    allUsersData = "all" + allUsersData;

    return allUsersData;
}
//������ ������������� � �� ������, ������ ����������
std::string DataBase::takeAllUsersNameStatusIsBann()
{
    std::string allUsersData; 

    std::string query = "SELECT name, online, isbanned FROM users";

    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        res = mysql_store_result(&mysql);

        if (res != nullptr)
        {
            int numFields = mysql_num_fields(res);

            while ((row = mysql_fetch_row(res)))
            {
                std::string name = row[0];     
                std::string status = row[1];   
                std::string bann = row[2];     
                allUsersData += name + "|" + status + "|" + bann + "|";
            }
            mysql_free_result(res);
        }
        else
        {
            std::cerr << "������ ���������� �������: " << mysql_error(&mysql) << std::endl;
        }
    }
    else
    {
        std::cerr << "������ ���������� �������: " << mysql_error(&mysql) << std::endl;
    }

    allUsersData = "inf|" + allUsersData;

    return allUsersData;
}
//��������� ������� ���������� ������������
void DataBase::setBannStatus(const std::string& name, bool bann)
{
    int status = bann ? 1 : 0;

    std::ostringstream queryStream;
    queryStream << "UPDATE users SET isbanned = " << status << " WHERE name = '" << name << "'";

    std::string query = queryStream.str();
    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        std::cout << "������������ " << name << " ������������" << std::endl;
    }
    else
    {
        std::cerr << "������ ��� ���������� ������� UPDATE: " << mysql_error(&mysql) << std::endl;
    }
}
//��������� ������� ������������
void DataBase::setUserStatus(const std::string& login, bool online)
{
    int status = online ? 1 : 0;

    std::ostringstream queryStream;
    queryStream << "UPDATE users SET online = " << status << " WHERE login = '" << login << "'";

    std::string query = queryStream.str();
    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        std::cout << "������ ������������ " << login << " ��������" << std::endl;
    }
    else
    {
        std::cerr << "������ ��� ���������� ������� UPDATE: " << mysql_error(&mysql) << std::endl;
    }
}
//����� �������� ���� �������������, ����� ��� ������� �������
void DataBase::resetAllUsersStatus()
{
    std::string query = "UPDATE users SET online = 0";

    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        std::cout << "������ ���� ������������� ������� � ���� ������" << std::endl;
    }
    else
    {
        std::cerr << "������ ��� ���������� ������� UPDATE: " << mysql_error(&mysql) << std::endl;
    }
}
//���������� ��������� ��� ���� � ��
void DataBase::addMessageToAll(const std::string& from, const std::string& to, const std::string& text, bool toAll)
{
    // ������� ������ ��� ���������� ������ ��������� � ������� "messages"
    std::string query = "INSERT INTO messages (from_id, to_id, text, to_all) VALUES ('" + from + "', '" + to + "', '" + text + "', " + "1" + ")";
    // ��������� ������ � ���� ������
    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        std::cout << "����� ��������� ��������� � ���� ������" << std::endl;
        
    }
    else
    {
        std::cerr << "������ ��� ���������� ������� INSERT: " << mysql_error(&mysql) << std::endl;
    }
}
//���������� ��������� ��� ������������ � ��
void DataBase::addMessageToDB(const std::string& from, const std::string& to, const std::string& text)
{
    std::string query = "INSERT INTO messages (from_id, to_id, text) VALUES ('" + from + "', '" + to + "', '" + text + "')";
    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        std::cout << "����� ��������� ��������� � ���� ������" << std::endl;

    }
    else
    {
        std::cerr << "������ ��� ���������� ������� INSERT: " << mysql_error(&mysql) << std::endl;
    }
}
//����� �� �� ��������� ��� ����
std::string DataBase::loadMessagesToAll()
{
    std::string messages;

    std::string query = "SELECT timestamp, from_id, text FROM messages WHERE to_all = 1";
    
    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        MYSQL_RES* result = mysql_store_result(&mysql);
        if (result != nullptr)
        {
            int numFields = mysql_num_fields(result);

            while ((row = mysql_fetch_row(result)))
            {
                std::string timestamp = row[0];
                std::string from_id = row[1];
                std::string text = row[2];

                messages = messages + "|" + timestamp + "|" + from_id + "|" + text;
            }
            mysql_free_result(result);
        }
    }
    else
    {
        std::cerr << "������ ��� ���������� ������� SELECT: " << mysql_error(&mysql) << std::endl;
    }
    messages = "lma" + messages;
    return messages;
}
//����� �� �� ��������� �� � ��� ������������
std::string DataBase::loadMessagesToUser(const std::string& to)
{
    std::string messages;

    std::string query = "SELECT timestamp, from_id, to_id, text FROM messages WHERE (from_id = '" + to + "' OR to_id = '" + to + "') AND to_all = 0 "
        "ORDER BY timestamp ASC"; // ���������� �� ������� ���������� (�������� �� �����)

    if (mysql_query(&mysql, query.c_str()) == 0)
    {
        MYSQL_RES* result = mysql_store_result(&mysql);
        if (result != nullptr)
        {
            int numFields = mysql_num_fields(result);

            while ((row = mysql_fetch_row(result)))
            {
                std::string timestamp = row[0];
                std::string from_id = row[1];
                std::string to_id = row[2];
                std::string text = row[3];

                messages = messages + "|" + timestamp + "|" + from_id + "|" + to_id + "|" + text;
            }
            mysql_free_result(result);
        }
    }
    else
    {
        std::cerr << "������ ��� ���������� ������� SELECT: " << mysql_error(&mysql) << std::endl;
    }

    messages = "lmu" + messages;
    return messages;
}

//////////////////////////////////�������� �����////////////////////////////
//������� ���������� ������� users
void DataBase::showAllUsers()
{
    mysql_query(&mysql, "SELECT * FROM users"); //������ ������ � �������

    //������� ��� ��� ���� � ���� ����� ����
    if (res = mysql_store_result(&mysql)) {
        while (row = mysql_fetch_row(res)) {
            for (int i = 0; i < mysql_num_fields(res); i++) {
                std::cout << row[i] << "  ";
            }
            std::cout << std::endl;
        }
    }
}
