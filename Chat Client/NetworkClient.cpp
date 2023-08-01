#include "NetworkClient.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <ws2tcpip.h>
#include <vector>

//создание сокета
bool NetworkClient::createSocket()
{
    clientSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket_ == INVALID_SOCKET) {
        std::cerr << "ОШИБКА: сокет не создан" << std::endl;
        return false;
    }
    return true;
}
//соединение с сервером
bool NetworkClient::connectToServer()
{
    SOCKADDR_IN serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_);
    inet_pton(AF_INET, ipAddress_.c_str(), &(serverAddress.sin_addr));

    if (connect(clientSocket_, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Не удалось установить соединение с сервером" << std::endl;
        return false;
    }
    return true;
}
//закрытие сокета
void NetworkClient::closeSocket()
{
    if (clientSocket_ != INVALID_SOCKET) {
        closesocket(clientSocket_);
        clientSocket_ = INVALID_SOCKET;
    }
}
//конструктор
NetworkClient::NetworkClient(const std::string& ipAddress, int port) : ipAddress_(ipAddress), port_(port), clientSocket_(INVALID_SOCKET) { }
//деструктор
NetworkClient::~NetworkClient()
{
    stopClient();
    WSACleanup();
}
//подключение клиента к серверу
void NetworkClient::startClient()
{
    Settings settings;
    settings.loadSettingsFromFile("settings.txt");

    // Получение порта из настроек
    std::string portStr = settings.getSetting("PORT");
    int port = std::stoi(portStr);

    port_ = port;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Ошибка инициализации" << std::endl;
        return;
    }

    if (!createSocket()) {
        WSACleanup();
        return;
    }

    if (!connectToServer()) {
        closeSocket();
        WSACleanup();
        return;
    }

    std::cout << "Соединение с сервером установлено" << std::endl;
    std::string greeting = "log|admin|admin";

}
//отключение клиента от сервера
void NetworkClient::stopClient()
{
    closeSocket();
    WSACleanup();
}
//отправка тестового сообщения на сервер
void NetworkClient::sendMessage(SOCKET clientSocket, const std::string& message) 
{
    send(clientSocket, message.c_str(), message.length(), 0);
    std::cout << "\nСообщение отправлено: " << message << std::endl;

    char buffer[MESSAGE_LENGTH] = { 0 };
    recv(clientSocket, buffer, MESSAGE_LENGTH, 0);
    std::string response(buffer);
    std::cout << "\nОтвет получен: " << response << "\n" << std::endl;
}
//разделение сообщения по символу "|"
bool NetworkClient::parseMessage(const std::string& message, std::vector<std::string>& arguments)
{
    //разделитель для аргументов
    const char delimiter = '|';

    //создаем поток для чтения из сообщения
    std::istringstream stream(message);

    //разбиваем сообщение на аргументы по разделителю и сохраняем их в векторе
    std::string argument;
    while (std::getline(stream, argument, delimiter))
    {
        arguments.push_back(argument);
    }
    //проверяем, что были извлечены аргументы
    return !arguments.empty();
}
//проверка логина и пароля
std::string NetworkClient::loginToServer(const std::string& login, const std::string& password)
{
    std::string message = "log|" + login + "|" + password;
    send(clientSocket_, message.c_str(), message.length(), 0);

    char buffer[MESSAGE_LENGTH] = { 0 };
    recv(clientSocket_, buffer, MESSAGE_LENGTH, 0);
    std::string response(buffer);
    return response; 
}
//запрос имени пользователя
std::string NetworkClient::askUserName(const std::string& login) 
{
    std::string message = "n|" + login;
    send(clientSocket_, message.c_str(), message.length(), 0);

    char buffer[MESSAGE_LENGTH] = { 0 };
    memset(buffer, 0, MESSAGE_LENGTH);
    recv(clientSocket_, buffer, MESSAGE_LENGTH, 0);
    std::string response(buffer);
    if (response.find("n|") == 0) 
    {
        std::vector<std::string> arguments;
        arguments.clear();
        if (parseMessage(response, arguments))
        {
            // Проверяем, что получено достаточное количество аргументов
            if (arguments.size() >= 2) //аргумент 0 - это префикс
            {
                std::string name = arguments[1];
                std::cout << "\nДобро пожаловать, " << name << "\n" << std::endl;
                return name;
            }
            else
            {
                std::string response = "n|error";
            }
        }
        else
        {
            std::string response = "n|error";
        }
    }
}
//запрос на регистрацию нового пользователя
std::string NetworkClient::registrationNewUser(const std::string& login, const std::string& password, const std::string& name)
{
    std::string message = "reg|" + login + "|" + password + "|" + name;
    send(clientSocket_, message.c_str(), message.length(), 0);

    char buffer[MESSAGE_LENGTH] = { 0 };
    recv(clientSocket_, buffer, MESSAGE_LENGTH, 0);
    std::string response(buffer);
    return response;
}
//запрос на установление статуса пользователя
std::string NetworkClient::setUserStatus(const std::string& login, bool status)
{
    std::string statusStr = status ? "1" : "0";

    std::string message = "us|" + login + "|" + statusStr;
    send(clientSocket_, message.c_str(), message.length(), 0);

    char buffer[MESSAGE_LENGTH] = { 0 };
    memset(buffer, 0, MESSAGE_LENGTH);
    recv(clientSocket_, buffer, MESSAGE_LENGTH, 0);
    std::string response(buffer);
    return response;
}
//запрос на отправку сообщения в БД
std::string NetworkClient::sendMessageToDB(const std::string& from, const std::string& to, const std::string& text)
{
    std::string message = "sm|" + from + "|" + to + "|" + text;
    send(clientSocket_, message.c_str(), message.length(), 0);

    char buffer[MESSAGE_LENGTH] = { 0 };
    memset(buffer, 0, MESSAGE_LENGTH);
    recv(clientSocket_, buffer, MESSAGE_LENGTH, 0);
    std::string response(buffer);
    if (response == "sm|success")
    {
        std::cout << "\nСообщение доставлено" << std::endl;
    }
    else
    {
        std::cout << "\nОШИБКА: сообщение не доставлено" << std::endl;
    }
    return response;
}
//запрос на отправку сообщения для всех
std::string NetworkClient::sendMessageToALL(const std::string& from, const std::string& to, const std::string& text)
{
    std::string message = "sma|" + from + "|" + to + "|" + text;
    send(clientSocket_, message.c_str(), message.length(), 0);

    char buffer[MESSAGE_LENGTH] = { 0 };
    memset(buffer, 0, MESSAGE_LENGTH);
    recv(clientSocket_, buffer, MESSAGE_LENGTH, 0);
    std::string response(buffer);
    if (response == "sma|success") 
    {
        std::cout << "\nСообщение доставлено" << std::endl;
    }
    else 
    {
        std::cout << "\nОШИБКА: сообщение не доставлено" << std::endl;
    }
    return response;
}
//проверка существует ли пользователь, кому отправлено сообщение
std::string NetworkClient::checkUserExists(const std::string& to)
{
    std::string message = "ue|" + to;
    send(clientSocket_, message.c_str(), message.length(), 0);

    char buffer[MESSAGE_LENGTH] = { 0 };
    memset(buffer, 0, MESSAGE_LENGTH);
    recv(clientSocket_, buffer, MESSAGE_LENGTH, 0);
    std::string response(buffer);

    return response;
}
//вывод сообщений для всех
std::vector<std::string> NetworkClient::loadMessagesToAll()
{
    std::string message = "lma|all";
    send(clientSocket_, message.c_str(), message.length(), 0);

    char buffer[MESSAGE_LENGTH] = { 0 };
    memset(buffer, 0, MESSAGE_LENGTH);
    recv(clientSocket_, buffer, MESSAGE_LENGTH, 0);
    std::string response(buffer);

    std::vector<std::string> arguments;

    if (parseMessage(response, arguments)) 
    {
        std::vector<std::string> loadMessages;
        for (size_t i = 1; i < arguments.size(); i += 3) 
        {
            std::string time = arguments[i];
            std::string from = arguments[i + 1];
            std::string text = arguments[i + 2];

            loadMessages.push_back(time + "|" + from + "|" + text + "|");
        }

        return loadMessages;
    }
}
//вывод сообщений от и для пользователя
std::vector<std::string> NetworkClient::loadMessagesToUser(const std::string& to)
{
    std::string message = "lmu|" + to;
    send(clientSocket_, message.c_str(), message.length(), 0);

    char buffer[MESSAGE_LENGTH] = { 0 };
    memset(buffer, 0, MESSAGE_LENGTH);
    recv(clientSocket_, buffer, MESSAGE_LENGTH, 0);
    std::string response(buffer);

    std::vector<std::string> arguments;
    arguments.clear();

    if (parseMessage(response, arguments))
    {
        std::vector<std::string> loadMessages;
        for (size_t i = 1; i < arguments.size(); i += 4)
        {
            std::string time = arguments[i];
            std::string from = arguments[i + 1];
            std::string to = arguments[i + 2];
            std::string text = arguments[i + 3];

            loadMessages.push_back(time + "|" + from + "|" + to + "|" + text + "|");
        }

        return loadMessages;
    }
}
//вывод списка пользователей
std::vector<std::string> NetworkClient::loadAllUsersFromDB()
{
    std::string message = "all|";
    send(clientSocket_, message.c_str(), message.length(), 0);

    char buffer[MESSAGE_LENGTH] = { 0 };
    memset(buffer, 0, MESSAGE_LENGTH);
    recv(clientSocket_, buffer, MESSAGE_LENGTH, 0);
    std::string response(buffer);

    std::vector<std::string> arguments;
    arguments.clear();
    if (parseMessage(response, arguments))
    {
        if (arguments.size() > 0) 
        {
            std::vector<std::string> users;
            users.clear();
            for (size_t i = 1; i < arguments.size(); i += 2) 
            {
                std::string name = arguments[i];
                std::string status = arguments[i + 1];
                users.push_back(name + "|" + status);
            }
            return users;
        }
    }
}
//вывод списка пользователей, включая заблокированных
std::vector<std::string> NetworkClient::loadAllUsersInfo()
{
    std::string message = "inf|";
    send(clientSocket_, message.c_str(), message.length(), 0);

    char buffer[MESSAGE_LENGTH] = { 0 };
    memset(buffer, 0, MESSAGE_LENGTH);
    recv(clientSocket_, buffer, MESSAGE_LENGTH, 0);
    std::string response(buffer);

    std::vector<std::string> arguments;
    arguments.clear();
    if (parseMessage(response, arguments))
    {
        if (arguments.size() > 0)
        {
            std::vector<std::string> users;
            users.clear();
            for (size_t i = 1; i < arguments.size(); i += 3)
            {
                std::string name = arguments[i];
                std::string status = arguments[i + 1];
                std::string bann = arguments[i + 2];
                users.push_back(name + "|" + status + "|" + bann + "|");
            }
            return users;
        }
    }
}
//блокировка пользователя
std::string NetworkClient::setBannStatus(const std::string& name, bool bann)
{
    std::string isBanned = bann ? "1" : "0";

    std::string message = "ban|" + name + "|" + isBanned;
    send(clientSocket_, message.c_str(), message.length(), 0);

    char buffer[MESSAGE_LENGTH] = { 0 };
    memset(buffer, 0, MESSAGE_LENGTH);
    recv(clientSocket_, buffer, MESSAGE_LENGTH, 0);
    std::string response(buffer);
    return response;
}
