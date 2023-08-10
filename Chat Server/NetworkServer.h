//Сетевой код сервера
#pragma once
#include <string>
#include <winsock2.h>
#include "Utility.h"
#include "DataBase.h"

#pragma comment(lib, "ws2_32.lib") 

#define MESSAGE_LENGTH 1024 // Максимальный размер буфера для данных
//#define PORT 7777 // Будет использоваться этот номер порта

class NetworkServer {
private:
	int port_;// = PORT;
	WSADATA wsaData;
	SOCKET serverSocket_ = INVALID_SOCKET;
	SOCKET clientSocket_ = INVALID_SOCKET;

	DataBase db; // Объект базы данных как член класса

	bool createSocket(); //создание сокета
	bool bindSocket(); //бинд сокета
	bool listenForConnections(); //прослушивание порта
	bool acceptClientConnection(); //прием соединения от клиента
	void closeServerSocket(); //закрытие сокета

public:
	NetworkServer(int port); //конструктор класса NetworkServer
	NetworkServer() {} // Инициализация объекта NetworkServer по умолчанию
	~NetworkServer();

	void startServer(); //запуск сетевых функций
	void handleClient(SOCKET clientSocket, DataBase& db); //прием сообщений от клиента
	void stopServer(); //отключение сетевых функций

	bool parseMessage(const std::string& message, std::vector<std::string>& arguments);//метод для обработки сообщения

};