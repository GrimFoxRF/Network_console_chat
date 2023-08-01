//��������������� ������
#pragma once
#include <string>
#include <unordered_map>

//��������� ���������� ������� � ������
void SetFilePermissions(const std::string& filePath); 
//��������� ������ ��� ��������� ���� � ������� �������
std::string getCurrentTime();

//����� �������� �� ��������� ����
class Settings {
private:

	std::unordered_map<std::string, std::string> settings; // �������� ��������

public:

	void loadSettingsFromFile(const std::string& filename);//����� ��� �������� �������� �� �����
	void saveSettingsToFile(const std::string& filename);//���������� �������� � ����

	std::string getSetting(const std::string& key);//��������� �������� ��������� �� �����
	void setSetting(const std::string& key, const std::string& value) { settings[key] = value; }//��������� �������� ��������� �� �����
};