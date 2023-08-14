//логгирование сообщений
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <shared_mutex>

class Logger 
{
private:
    std::string _filename;
    std::ofstream _logFile;
    mutable std::shared_mutex _logMutex;


public:
    Logger(const std::string& filename);//файл log.txt создается, если его нет и открывается автоматически
    ~Logger();//закрытие файла log.txt при разрушение объекта logger

    void writeLogToFile(const std::string& logMessage);//запись сообщения в файл логов
    void readLogFromFile();//чтение сообщений из файла логов

};