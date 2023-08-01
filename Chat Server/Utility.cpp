//���������� ��������������� �������
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <filesystem> //��������� �++17 � ����
#include "Utility.h"
#include <unordered_map>

namespace fs = std::filesystem;

//��������� ���������� ������� � ������
void SetFilePermissions(const std::string& filePath)
{
	fs::perms permissions = fs::perms::owner_read | fs::perms::owner_write;

	fs::file_status status = fs::status(filePath);
	if (status.permissions() != permissions) {
		fs::permissions(filePath, permissions);
	}
}

//������� ��� ����������� ���� � ������� ����
std::string getCurrentTime() 
{
	struct tm newtime;
	time_t now = time(0);
	localtime_s(&newtime, &now);

	int day = newtime.tm_mday;
	int month = 1 + newtime.tm_mon;
	int year = 1900 + newtime.tm_year;
	int hour = newtime.tm_hour;
	int min = newtime.tm_min;

	std::ostringstream oss;
	oss << std::setfill('0') << std::setw(2) << day << "." << std::setw(2) << month << "." << year << " "
		<< std::setw(2) << hour << ":" << std::setw(2) << min;

	return oss.str();
}

void Settings::loadSettingsFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            size_t delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos)
            {
                std::string key = line.substr(0, delimiterPos);
                std::string value = line.substr(delimiterPos + 1);
                settings[key] = value;
            }
        }
        file.close();
        std::cout << "��������� ������� ��������� �� �����." << std::endl;
    }
    else 
    {
        std::cerr << "���� � ����������� �� ������. \n����� ���� ������ �������� ����������� �� ���������." << std::endl;
        std::ofstream newFile(filename);
        if (newFile.is_open())
        {
            newFile << "IP=127.0.0.1\n";
            newFile << "PORT=7777\n";
            newFile << "MESSAGE_LENGTH=1024\n";
            newFile.close();
            std::cout << "���� � ����������� ������� ������." << std::endl;
        }
        else
        {
            std::cerr << "������ ��� �������� ����� � �����������." << std::endl;
        }
    }
}

void Settings::saveSettingsToFile(const std::string& filename)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        for (const auto& entry : settings) 
        {
            file << entry.first << "=" << entry.second << "\n";
        }
        file.close();
        std::cout << "��������� ������� ��������� � ����." << std::endl;
    }
    else
    {
        std::cerr << "������ ��� ���������� �������� � ����." << std::endl;
    }
}

std::string Settings::getSetting(const std::string& key)
{
    if (settings.count(key)) {
        return settings[key];
    }
    return "";
}
