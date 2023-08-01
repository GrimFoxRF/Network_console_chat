//Сетевой код клиента
#pragma once
#include <winsock2.h>
#include <iostream>
#include <vector>
#include "Utility.h"

#pragma comment(lib, "ws2_32.lib") 

#define MESSAGE_LENGTH 1024 // Максимальный размер буфера для данных
//#define PORT 7777 // Будет использоваться этот номер порта


class NetworkClient {
private:
    std::string ipAddress_;
    int port_; //= PORT;
    WSADATA wsaData;
    SOCKET clientSocket_;

    bool createSocket(); //создание сокета
    bool connectToServer(); //соединение с сервером
    void closeSocket(); //закрытие сокета

public:
    NetworkClient(const std::string& ipAddress, int port); //конструктор класса NetworkClient
    ~NetworkClient();

    void startClient(); //запуск сетевых функций
    void stopClient(); //остановка сетевых функций и закрытие сокета
    void sendMessage(SOCKET clientSocket, const std::string& message); //отправка тестового сообщения серверу

    bool parseMessage(const std::string& message, std::vector<std::string>& arguments);//метод для лбработки сообщений от сервера
    //---ЗАПРОСЫ К СЕРВЕРУ---
    std::string loginToServer(const std::string& login, const std::string& password);//запрос на проверку логина и пароля
    std::string askUserName(const std::string& login);//запрос на получение имени пользователя по логину
    std::string registrationNewUser(const std::string& login, const std::string& password, const std::string& name);//запрос на регистрацию нового пользователя
    std::string setUserStatus(const std::string& login, bool status);//запрос на изменение статуса пользователя

    std::string sendMessageToDB(const std::string& from, const std::string& to, const std::string& text);//отправка сообщения в базу данных
    std::string sendMessageToALL(const std::string& from, const std::string& to, const std::string& text);//отправка сообщения всем пользователям
    std::string checkUserExists(const std::string& to);//проверка что пользователь есть в БД
    std::vector<std::string> loadMessagesToAll();//показывает сообщения дла всех пользователей
    std::vector<std::string> loadMessagesToUser(const std::string& to);//показывает сообщения для текущего пользователя

    std::vector<std::string> loadAllUsersFromDB();//показывает список пользователей и их статус
    std::vector<std::string> loadAllUsersInfo();//показывает список пользователей и информацию для админа
    std::string setBannStatus(const std::string& name, bool bann);//запрос на блокировку/разблокировку пользователя



};