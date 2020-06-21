#include "App.h"

RegistryManipulator::RegistryManipulator() {
  RegOpenKey(HKEY_CURRENT_USER, L"SOFTWARE", &this->hkey);
  RegCreateKey(this->hkey, regDir.c_str(), &this->hkey);
}

RegistryManipulator::~RegistryManipulator() { RegCloseKey(this->hkey); }

void RegistryManipulator::setRegKey(std::wstring key) {
  DWORD value = 1;
  RegSetValueEx(this->hkey, key.c_str(), NULL, REG_DWORD,
                reinterpret_cast<BYTE*>(&value), sizeof(DWORD));
}

void RegistryManipulator::deleteRegKey(std::wstring key) {
  RegDeleteValue(this->hkey, key.c_str());
}

std::wstring RegistryManipulator::getAllKeys() {
  int keyIndex = 0;
  std::wstring allKeys;

  while (true) {
    DWORD buffSize = 255;
    wchar_t* buff = new wchar_t[buffSize];
    HKEY key = this->hkey;
    auto status =
        RegEnumValueW(key, keyIndex++, buff, &buffSize, NULL, NULL, NULL, NULL);

    if (status == ERROR_NO_MORE_ITEMS) {
      delete[] buff;
      break;
    }

    allKeys += std::wstring(buff);
    allKeys += L",   ";
    delete[] buff;
  }

  allKeys = L"Number of connected clients: " + std::to_wstring(keyIndex - 1) +
            L"; Users: " + allKeys;
  return allKeys;
}

void RegistryManipulator::deleteAllKeys() {
  int keyIndex = 0;
  DWORD buffSize = 255;
  wchar_t* buff = new wchar_t[buffSize];
  while (RegEnumValueW(this->hkey, keyIndex++, buff, &buffSize, NULL, NULL,
                       NULL, NULL) != ERROR_NO_MORE_ITEMS) {
    this->deleteRegKey(std::wstring(buff));
    this->deleteAllKeys();
    break;
  }
}
