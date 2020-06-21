#pragma once

class RegistryManipulator {
 private:
  const std::wstring regDir{L"TcpServer"};

  HKEY hkey = NULL;

 public:
  RegistryManipulator();
  ~RegistryManipulator();

  void setRegKey(std::wstring key);
  void deleteRegKey(std::wstring key);
  std::wstring getAllKeys();
  void deleteAllKeys();
};