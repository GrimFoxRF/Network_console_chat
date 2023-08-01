//Вспомогательные методы
#pragma once
#include <string>
#include <unordered_map>

//Установка разрешения доступа к файлам
void SetFilePermissions(const std::string& filePath); 
//Служебные методы для получения даты и времени системы
std::string getCurrentTime();

//класс отвечает за настройки чата
class Settings {
private:

	std::unordered_map<std::string, std::string> settings; // Хранение настроек

public:

	void loadSettingsFromFile(const std::string& filename);//метод для загрузки настроек из файла
	void saveSettingsToFile(const std::string& filename);//сохранение настроек в файл

	std::string getSetting(const std::string& key);//получение значения настройки по ключу
	void setSetting(const std::string& key, const std::string& value) { settings[key] = value; }//изменение значения настройки по ключу
};