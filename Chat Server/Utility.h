//Вспомогательные методы
#pragma once
#include<iostream>
#include <exception>
#include <string>
#include <fstream>
#include <string>
#include <unordered_map>

//Исключения неверный логин и имя
class loginException : public std::exception
{
public:
	const char* what() const noexcept override
	{
		return "ОШИБКА: логин занят";
	}
};
class nameException : public std::exception
{
public:
	const char* what() const noexcept override
	{
		return "ОШИБКА: имя занято";
	}
};

void SetFilePermissions(const std::string& filePath); //Установка разрешения доступа к файлам

//Служебные методы для получения даты и времени системы
std::string getCurrentTime();

class Settings {
private:

    std::unordered_map<std::string, std::string> settings; // Хранение настроек

public:
    
    void loadSettingsFromFile(const std::string& filename);//метод для загрузки настроек из файла
	void saveSettingsToFile(const std::string& filename);//сохранение настроек в файл

    std::string getSetting(const std::string& key);//получение значения настройки по ключу
    void setSetting(const std::string& key, const std::string& value) {settings[key] = value;}//изменение значения настройки по ключу
};