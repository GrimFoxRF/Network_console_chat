//������� ��� �������
#pragma once
#include <winsock2.h>
#include <iostream>
#include <vector>
#include "Utility.h"

#pragma comment(lib, "ws2_32.lib") 

#define MESSAGE_LENGTH 1024 // ������������ ������ ������ ��� ������
//#define PORT 7777 // ����� �������������� ���� ����� �����


class NetworkClient {
private:
    std::string ipAddress_;
    int port_; //= PORT;
    WSADATA wsaData;
    SOCKET clientSocket_;

    bool createSocket(); //�������� ������
    bool connectToServer(); //���������� � ��������
    void closeSocket(); //�������� ������

public:
    NetworkClient(const std::string& ipAddress, int port); //����������� ������ NetworkClient
    ~NetworkClient();

    void startClient(); //������ ������� �������
    void stopClient(); //��������� ������� ������� � �������� ������
    void sendMessage(SOCKET clientSocket, const std::string& message); //�������� ��������� ��������� �������

    bool parseMessage(const std::string& message, std::vector<std::string>& arguments);//����� ��� ��������� ��������� �� �������
    //---������� � �������---
    std::string loginToServer(const std::string& login, const std::string& password);//������ �� �������� ������ � ������
    std::string askUserName(const std::string& login);//������ �� ��������� ����� ������������ �� ������
    std::string registrationNewUser(const std::string& login, const std::string& password, const std::string& name);//������ �� ����������� ������ ������������
    std::string setUserStatus(const std::string& login, bool status);//������ �� ��������� ������� ������������

    std::string sendMessageToDB(const std::string& from, const std::string& to, const std::string& text);//�������� ��������� � ���� ������
    std::string sendMessageToALL(const std::string& from, const std::string& to, const std::string& text);//�������� ��������� ���� �������������
    std::string checkUserExists(const std::string& to);//�������� ��� ������������ ���� � ��
    std::vector<std::string> loadMessagesToAll();//���������� ��������� ��� ���� �������������
    std::vector<std::string> loadMessagesToUser(const std::string& to);//���������� ��������� ��� �������� ������������

    std::vector<std::string> loadAllUsersFromDB();//���������� ������ ������������� � �� ������
    std::vector<std::string> loadAllUsersInfo();//���������� ������ ������������� � ���������� ��� ������
    std::string setBannStatus(const std::string& name, bool bann);//������ �� ����������/������������� ������������



};