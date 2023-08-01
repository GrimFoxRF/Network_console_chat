#include "NetworkServer.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

NetworkServer::NetworkServer(int port) : port_(port), serverSocket_(INVALID_SOCKET), clientSocket_(INVALID_SOCKET) { } // ����������� ������

NetworkServer::~NetworkServer() //����������
{
    closeServerSocket();
    WSACleanup();
}
//������ ������� �������
void NetworkServer::startServer()
{
    Settings settings;
    settings.loadSettingsFromFile("settings.txt");

    // ��������� ����� �� ��������
    std::string portStr = settings.getSetting("PORT");
    int port = std::stoi(portStr);

    port_ = port;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "������ �������������" << std::endl;
        return;
    }

    if (!createSocket()) {
        WSACleanup();
        return;
    }

    if (!bindSocket()) {
        closeServerSocket();
        WSACleanup();
        return;
    }

    if (!listenForConnections()) {
        closeServerSocket();
        WSACleanup();
        return;
    }

    std::cout << "������ �������. �������� ����������..." << std::endl;

    if (!acceptClientConnection()) {
        closeServerSocket();
        WSACleanup();
        return;
    }

    std::cout << "������ ����������� � �������" << std::endl;
    DataBase db;
    db.dataBaseConnect(); //����������� � ���� ������
    handleClient(clientSocket_, db); //��������� �������� �� �������

}
//�������� ������
bool NetworkServer::createSocket()
{
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ == INVALID_SOCKET) {
        std::cerr << "������: ����� �� ������" << std::endl;
        return false;
    }
    return true;
}
//���� ������
bool NetworkServer::bindSocket()
{
    SOCKADDR_IN serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket_, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "�� ������� ��������� �����" << std::endl;
        return false;
    }
    std::cout << "����� ������� ��������" << std::endl;
    return true;
}
//������������� ����������
bool NetworkServer::listenForConnections()
{
    if (listen(serverSocket_, 1) == SOCKET_ERROR) {
        std::cerr << "�� ������� ���������� �����" << std::endl;
        return false;
    }
    std::cout << "����� ��������������" << std::endl;
    return true;
}
//����� ���������� �������
bool NetworkServer::acceptClientConnection()
{
    clientSocket_ = accept(serverSocket_, NULL, NULL);
    if (clientSocket_ == INVALID_SOCKET) {
        std::cerr << "�� ������� ������� ���������� �������" << std::endl;
        return false;
    }
    std::cout << "������� ������� ���������� �������" << std::endl;
    return true;
}
//�������� ������
void NetworkServer::closeServerSocket()
{
    if (serverSocket_ != INVALID_SOCKET) {
        closesocket(serverSocket_);
        serverSocket_ = INVALID_SOCKET;
    }
}
//����� ��������� �� ������� � �������� ������
void NetworkServer::handleClient(SOCKET clientSocket, DataBase& db)
{
    char buffer[MESSAGE_LENGTH] = { 0 };
    bool connect = true;
    // ���� ��� �������� � ��������� ��������� �� �������
    while (connect)
    {
        recv(clientSocket, buffer, MESSAGE_LENGTH, 0);

        std::string message(buffer);
        // �������� �������� � ��������� ���������������� �������
        if (message.find("log|") == 0) // ������� log - �������� ������������ ��� ������
        {
            void loginToServer();
            std::vector<std::string> arguments;
            if (parseMessage(message, arguments))
            {
                // ���������, ��� �������� ����������� ���������� ����������
                if (arguments.size() >= 3) //�������� 0 - ��� �������
                {
                    std::string login = arguments[1];
                    std::string password = arguments[2];

                    // ���������� �������� ������ � ������ � ���� ������
                    bool loginSuccess = db.checkUserLogin(login);
                    bool passwordSuccess = db.checkUserPassword(password);
                    bool banned = db.isBanned(login);

                    // �������� ������ �������
                    std::string response = "log|";
                    if (loginSuccess && passwordSuccess && !banned)
                    {
                        
                        response += "success";
                    }
                    else
                    {
                        response += "fail";
                    }
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
                else
                {
                    std::string response = "log|error";
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
            }
            else
            {
                std::string response = "log|error";
                send(clientSocket, response.c_str(), response.length(), 0);
            }
        }
        else if (message.find("n|") == 0) //����� ����� �� ������
        {
            std::vector<std::string> arguments;
            if (parseMessage(message, arguments))
            {
                if (arguments.size() >= 2)
                {
                    std::string login = arguments[1];

                    // ���������� �������� ������ � ������� � ���� ������
                    bool loginSuccess = db.checkUserLogin(login);
                    bool banned = db.isBanned(login);
                    std::string name = db.takeUserName(login);
                    // �������� ������ �������
                    std::string response = "n|";
                    if (loginSuccess && !banned)
                    {
                        response += name;
                    }
                    else
                    {
                        response += "fail";
                    }
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
                else
                {
                    std::string response = "n|error";
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
            }
            else
            {
                std::string response = "n|error";
                send(clientSocket, response.c_str(), response.length(), 0);
            }
        }
        else if (message.find("reg|") == 0) //������ �� ����������� ������ ������������
        {
            std::vector<std::string> arguments;
            if (parseMessage(message, arguments))
            {
                if (arguments.size() >= 4)
                {
                    std::string login = arguments[1];
                    std::string password = arguments[2];
                    std::string name = arguments[3];

                    // ���������� �������� ������ � ������ � ���� ������
                    bool loginExists = db.checkLoginExistsInDB(login);
                    bool nameExists = db.checkNameExistsInDB(name);
                    // �������� ������ �������
                    std::string response = "reg|";
                    if (!loginExists && !nameExists)
                    {
                        db.addNewUser(login, password, name);
                        response += "success";
                    }
                    else if(loginExists )
                    {
                        response += "LoginFail";
                    }
                    else if (nameExists)
                    {
                        response += "NameFail";
                    }
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
                else
                {
                    std::string response = "reg|error";
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
            }
            else
            {
                std::string response = "reg|error";
                send(clientSocket, response.c_str(), response.length(), 0);
            }
        }
        else if (message.find("all|") == 0) //����� ���� ���� ������������� � �� ������
        {
            std::string response = db.takeAllUsersNameStatus();
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else if (message.find("us|") == 0) //��������� ������� ������������
        {
            std::vector<std::string> arguments;
            if (parseMessage(message, arguments))
            {
                if (arguments.size() >= 3)
                {
                    std::string login = arguments[1];
                    std::string statusStr = arguments[2];

                    bool status = (statusStr == "1");

                    // ���������� �������� ������ � ������ � ���� ������
                    bool loginExists = db.checkLoginExistsInDB(login);

                    // �������� ������ �������
                    std::string response = "us|";
                    if (loginExists)
                    {
                        db.setUserStatus(login, status);
                        response += "success";
                    }
                    else
                    {
                        response += "LoginFail";
                    }

                    send(clientSocket, response.c_str(), response.length(), 0);
                }
                else
                {
                    std::string response = "us|error";
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
            }
            else
            {
                std::string response = "us|error";
                send(clientSocket, response.c_str(), response.length(), 0);
            }
        }
        else if (message.find("ue|") == 0) //�������� ���������� �� ������������
        {
            std::vector<std::string> arguments;
            if (parseMessage(message, arguments)) 
            {
                if (arguments.size() >= 2)
                {
                    std::string to = arguments[1];

                    bool nameExists = db.checkNameExistsInDB(to);

                    // �������� ������ �������
                    std::string response = "ue|";
                    if (nameExists)
                    {
                        response += "Exists";
                    }
                    else
                    {
                        response += "NotExists";
                    }
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
                else
                {
                    std::string response = "ue|error";
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
            }
            else
            {
                std::string response = "ue|error";
                send(clientSocket, response.c_str(), response.length(), 0);
            }
        }
        else if (message.find("sma|") == 0) //����� �� ���������� � �� ��������� ��� ����
        {
            std::vector<std::string> arguments;
            if (parseMessage(message, arguments)) 
            {
                if (arguments.size() >= 4) 
                {
                    std::string from = arguments[1];
                    std::string to = "�������������";
                    std::string text = arguments[3];

                    bool nameFrom = db.checkNameExistsInDB(from);
                    bool nameTo = db.checkNameExistsInDB(to);

                    std::string response = "sma|";
                    if (nameFrom && nameTo) 
                    {
                        db.addMessageToAll(from, to, text, true);
                        std::string response = "sma|success";
                        send(clientSocket, response.c_str(), response.length(), 0);
                    }
                }
                else
                {
                    std::string response = "sma|error";
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
            }
            else
            {
                std::string response = "sma|error";
                send(clientSocket, response.c_str(), response.length(), 0);
            }
        }
        else if (message.find("sm|") == 0) //�������� � �� ��������� �� � ��� ������������
        {
            std::vector<std::string> arguments;
            if (parseMessage(message, arguments))
            {
                if (arguments.size() >= 4)
                {
                    std::string from = arguments[1];
                    std::string to = arguments[2];
                    std::string text = arguments[3];

                    bool nameFrom = db.checkNameExistsInDB(from);
                    bool nameTo = db.checkNameExistsInDB(to);
                    bool isBanned = db.isBanned(to);

                    std::string response = "sm|";
                    if (nameFrom && nameTo && !isBanned)
                    {
                        db.addMessageToDB(from, to, text);
                        std::string response = "sm|success";
                        send(clientSocket, response.c_str(), response.length(), 0);
                    }
                }
                else
                {
                    std::string response = "sm|error";
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
            }
            else
            {
                std::string response = "sm|error";
                send(clientSocket, response.c_str(), response.length(), 0);
            }
            
            
        }
        else if (message.find("lma|") == 0) //����� ��������� ��� ����
        {
            std::string response = db.loadMessagesToAll();
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else if (message.find("lmu|") == 0)// ����� ��������� ��� ������������
        {
            std::vector<std::string> arguments;
            if (parseMessage(message, arguments))
            {
                if (arguments.size() >= 2)
                {
                    std::string to = arguments[1];

                    std::string response = db.loadMessagesToUser(to);
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
                else
                {
                    std::string response = "lmu|error";
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
            }
            else
            {
                std::string response = "lmu|error";
                send(clientSocket, response.c_str(), response.length(), 0);
            }
        }
        else if (message.find("inf|") == 0) //����� ���� ���� �������������, ������� � ����������
        {
            std::string response = db.takeAllUsersNameStatusIsBann();
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else if (message.find("ban|") == 0) //��������� ������� ������������
        {
            std::vector<std::string> arguments;
            if (parseMessage(message, arguments))
            {
                if (arguments.size() >= 3)
                {
                    std::string name = arguments[1];
                    std::string isBann = arguments[2];

                    bool bann = (isBann == "1");

                    bool nameExists = db.checkNameExistsInDB(name);

                    std::string response = "ban|";
                    if (nameExists)
                    {
                        db.setBannStatus(name, bann);
                        response += "success";
                    }
                    else
                    {
                        response += "BannFail";
                    }

                    send(clientSocket, response.c_str(), response.length(), 0);
                }
                else
                {
                    std::string response = "ban|error";
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
            }
            else
            {
                std::string response = "us|error";
                send(clientSocket, response.c_str(), response.length(), 0);
            }
        }
        // ������� ����� ��� ���������� ���������
        memset(buffer, 0, MESSAGE_LENGTH);
    }
}
//�������� �������
void NetworkServer::stopServer() 
{
    // �������� ������ �������
    if (serverSocket_ != INVALID_SOCKET) {
        closesocket(serverSocket_);
        serverSocket_ = INVALID_SOCKET;
    }

    // �������� ������ �������
    if (clientSocket_ != INVALID_SOCKET) {
        closesocket(clientSocket_);
        clientSocket_ = INVALID_SOCKET;
    }
    
    WSACleanup(); //������������ ��������
}

bool NetworkServer::parseMessage(const std::string& message, std::vector<std::string>& arguments)
{
    // ����������� "|" ��� ����������
    const char delimiter = '|';

    // ������� ����� ��� ������ �� ���������
    std::istringstream stream(message);

    // ��������� ��������� �� ��������� �� ����������� � ��������� �� � �������
    std::string argument;
    while (std::getline(stream, argument, delimiter))
    {
        arguments.push_back(argument);
    }

    return !arguments.empty();
}

