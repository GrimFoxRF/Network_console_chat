#include "NetworkServer.h"
#include "Logger.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <thread>
#include <functional>

NetworkServer::NetworkServer(int port) : port_(port), logger("log.txt"), serverSocket_(INVALID_SOCKET), clientSocket_(INVALID_SOCKET) { } // Конструктор класса

NetworkServer::~NetworkServer() //деструктор
{
    closeServerSocket();
    WSACleanup();
}
//запуск сетевых функций
void NetworkServer::startServer()
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

    std::cout << "Сервер запущен. Ожидание соединения..." << std::endl;
    while (true) {
        if (!acceptClientConnection()) {
            closeServerSocket();
            WSACleanup();
            return;
        }

        std::cout << "Клиент подключился к серверу" << std::endl;
       
        db.dataBaseConnect(); // Подключение к базе данных
        //создание отдельного потока для обработки запросов клиента
        std::thread clientThread(&NetworkServer::handleClient, this, clientSocket_, std::ref(db));
        clientThread.detach();
    }
    
}
//создание сокета
bool NetworkServer::createSocket()
{
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ == INVALID_SOCKET) {
        std::cerr << "ОШИБКА: сокет не создан" << std::endl;
        return false;
    }
    return true;
}
//бинд сокета
bool NetworkServer::bindSocket()
{
    SOCKADDR_IN serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket_, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Не удалось привязать сокет" << std::endl;
        return false;
    }
    std::cout << "Сокет успешно привязан" << std::endl;
    return true;
}
//прослушивание соединения
bool NetworkServer::listenForConnections()
{
    if (listen(serverSocket_, 1) == SOCKET_ERROR) {
        std::cerr << "Не удалось прослушать сокет" << std::endl;
        return false;
    }
    std::cout << "Сокет прослушивается" << std::endl;
    return true;
}
//прием соединения клиента
bool NetworkServer::acceptClientConnection()
{
    clientSocket_ = accept(serverSocket_, NULL, NULL);
    if (clientSocket_ == INVALID_SOCKET) {
        std::cerr << "Не удалось принять соединение клиента" << std::endl;
        return false;
    }
    std::cout << "Удалось принять соединение клиента" << std::endl;

    return true;
}
//закрытие сокета
void NetworkServer::closeServerSocket()
{
    if (serverSocket_ != INVALID_SOCKET) {
        closesocket(serverSocket_);
        serverSocket_ = INVALID_SOCKET;
    }
}
//прием сообщения от клиента и отправка ответа
void NetworkServer::handleClient(SOCKET clientSocket, DataBase& db)// Logger& logger
{
    char buffer[MESSAGE_LENGTH] = { 0 };
    bool connect = true;
    // Цикл для принятия и обработки сообщений от клиента
    while (connect)
    {
        recv(clientSocket, buffer, MESSAGE_LENGTH, 0);

        std::string message(buffer);
        // Проверка префикса и обработка соответствующего запроса
        if (message.find("log|") == 0) // Префикс log - проверка пользователя при логине
        {
            void loginToServer();
            std::vector<std::string> arguments;
            if (parseMessage(message, arguments))
            {
                // Проверяем, что получено достаточное количество аргументов
                if (arguments.size() >= 3) //аргумент 0 - это префикс
                {
                    std::string login = arguments[1];
                    std::string password = arguments[2];

                    // Выполнение проверки логина и пароля в базе данных
                    bool loginSuccess = db.checkUserLogin(login);
                    bool passwordSuccess = db.checkUserPassword(password);
                    bool banned = db.isBanned(login);

                    // Отправка ответа клиенту
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
        else if (message.find("n|") == 0) //поиск имени по логину
        {
            std::vector<std::string> arguments;
            if (parseMessage(message, arguments))
            {
                if (arguments.size() >= 2)
                {
                    std::string login = arguments[1];

                    // Выполнение проверки логина и статуса в базе данных
                    bool loginSuccess = db.checkUserLogin(login);
                    bool banned = db.isBanned(login);
                    std::string name = db.takeUserName(login);
                    // Отправка ответа клиенту
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
        else if (message.find("reg|") == 0) //запрос на регистрацию нового пользователя
        {
            std::vector<std::string> arguments;
            if (parseMessage(message, arguments))
            {
                if (arguments.size() >= 4)
                {
                    std::string login = arguments[1];
                    std::string password = arguments[2];
                    std::string name = arguments[3];

                    // Выполнение проверки логина и пароля в базе данных
                    bool loginExists = db.checkLoginExistsInDB(login);
                    bool nameExists = db.checkNameExistsInDB(name);
                    // Отправка ответа клиенту
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
        else if (message.find("all|") == 0) //поиск имен всех пользователей и их статус
        {
            std::string response = db.takeAllUsersNameStatus();
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else if (message.find("us|") == 0) //изменение статуса пользователя
        {
            std::vector<std::string> arguments;
            if (parseMessage(message, arguments))
            {
                if (arguments.size() >= 3)
                {
                    std::string login = arguments[1];
                    std::string statusStr = arguments[2];

                    bool status = (statusStr == "1");

                    // Выполнение проверки логина и пароля в базе данных
                    bool loginExists = db.checkLoginExistsInDB(login);

                    // Отправка ответа клиенту
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
        else if (message.find("ue|") == 0) //проверка существует ли пользователь
        {
            std::vector<std::string> arguments;
            if (parseMessage(message, arguments)) 
            {
                if (arguments.size() >= 2)
                {
                    std::string to = arguments[1];

                    bool nameExists = db.checkNameExistsInDB(to);

                    // Отправка ответа клиенту
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
        else if (message.find("sma|") == 0) //запро на добавление в БД сообщений для всех
        {
            std::vector<std::string> arguments;
            if (parseMessage(message, arguments)) 
            {
                if (arguments.size() >= 4) 
                {
                    std::string from = arguments[1];
                    std::string to = "Администратор";
                    std::string text = arguments[3];

                    bool nameFrom = db.checkNameExistsInDB(from);
                    bool nameTo = db.checkNameExistsInDB(to);

                    std::string response = "sma|";
                    if (nameFrom && nameTo) 
                    {
                        std::string logMessage = "[" + getCurrentTime() + "]" + " от " + from + " для " + to + ": " + text;
                        logger.writeLogToFile(logMessage);//запись сообщения в файл log.txt

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
        else if (message.find("sm|") == 0) //добавить в БД сообщение от и для пользователя
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
                        std::string logMessage = "[" + getCurrentTime() + "]" + " от " + from + " для " + to + ": " + text;
                        logger.writeLogToFile(logMessage);//запись сообщения в файл log.txt

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
        else if (message.find("lma|") == 0) //вывод сообщений для всех
        {
            std::string response = db.loadMessagesToAll();
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else if (message.find("lmu|") == 0)// вывод сообщений для пользователя
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
        else if (message.find("inf|") == 0) //поиск имен всех пользователей, статуса и блокировки
        {
            std::string response = db.takeAllUsersNameStatusIsBann();
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else if (message.find("ban|") == 0) //изменение статуса пользователя
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
        // Очищаем буфер для следующего сообщения
        memset(buffer, 0, MESSAGE_LENGTH);
    }
}
//закрытие сервера
void NetworkServer::stopServer() 
{
    // Закрытие сокета сервера
    if (serverSocket_ != INVALID_SOCKET) {
        closesocket(serverSocket_);
        serverSocket_ = INVALID_SOCKET;
    }

    // Закрытие сокета клиента
    if (clientSocket_ != INVALID_SOCKET) {
        closesocket(clientSocket_);
        clientSocket_ = INVALID_SOCKET;
    }
    
    WSACleanup(); //освобождения ресурсов
}

bool NetworkServer::parseMessage(const std::string& message, std::vector<std::string>& arguments)
{
    // Разделитель "|" для аргументов
    const char delimiter = '|';

    // Создаем поток для чтения из сообщения
    std::istringstream stream(message);

    // Разбиваем сообщение на аргументы по разделителю и сохраняем их в векторе
    std::string argument;
    while (std::getline(stream, argument, delimiter))
    {
        arguments.push_back(argument);
    }

    return !arguments.empty();
}

