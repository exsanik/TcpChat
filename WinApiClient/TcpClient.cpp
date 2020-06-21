#include "App.h"

DWORD TcpClient::outputHandler() {
  while (true) {
    std::vector<char> dataLengthRaw(21);
    recv(connection, dataLengthRaw.data(), 20, NULL);
    int dataLength;
    std::istringstream(dataLengthRaw.data()) >> dataLength;

    if (dataLengthRaw[std::to_string(dataLength).size()] == 'i') {
      std::vector<uint8_t> imageBuffer(dataLength);
      int offset = 0;
      while (dataLength > offset) {
        int amount = recv(connection,
                          reinterpret_cast<char *>(imageBuffer.data()) + offset,
                          dataLength - offset, 0);

        if (amount <= 0) {
          break;
        } else {
          offset += amount;
        }
      }

      this->dt->handleImage(imageBuffer);
      continue;
    }

    if (dataLengthRaw[std::to_string(dataLength).size()] == 'd') {
      this->outputThread = nullptr;
      ExitThread(0);
    }

    std::vector<char> message(dataLength);
    recv(connection, message.data(), dataLength, NULL);
    std::string asciiMsg(message.begin(), message.end());

    this->dt->handleString(asciiMsg);
  }
  ExitThread(0);
}

DWORD WINAPI TcpClient::StaticHandlerStart(LPVOID Param) {
  TcpClient *This = static_cast<TcpClient *>(Param);
  return This->outputHandler();
}

TcpClient::TcpClient(DataTransformer *dt) {
  this->dt = dt;
  if (WSAStartup(this->DLLVersion, &this->wsaData) != 0) {
    throw std::exception("Can't start client!");
  }
}

void TcpClient::configure(int port, std::wstring name, std::wstring ip) {
  this->port = port;
  this->ip = ip;
  this->name = name;

  this->sizeSockAddr = sizeof(this->addr);
  InetPton(AF_INET, static_cast<PCWSTR>(ip.c_str()),
           &this->addr.sin_addr.s_addr);
  addr.sin_port = htons(this->port);
  addr.sin_family = AF_INET;

  this->connection = socket(AF_INET, SOCK_STREAM, NULL);
}

void TcpClient::inputHandler(std::wstring message, char param) {
  std::wstring temp = this->name + L": " + message;
  std::string asciiMessage = this->dt->ws2s(temp);

  if (param != 'c' && param != 'd') {
    this->dt->handleString(asciiMessage.data());
  }

  std::string messageSize = (std::to_string(asciiMessage.size()) + param);

  send(this->connection, messageSize.data(), messageSize.size(), 0);
  Sleep(20);

  send(this->connection, asciiMessage.data(), asciiMessage.size(), 0);
  Sleep(20);
}

void TcpClient::imageHandler(const std::vector<uint8_t> &imageBuffer) {
  std::string imageSize = (std::to_string(imageBuffer.size()) + 'i');
  send(this->connection, imageSize.data(), imageSize.size(), 0);
  Sleep(20);

  int offset = 0;
  int size = imageBuffer.size();
  while (size > offset) {
    int amount =
        send(this->connection,
             reinterpret_cast<const char *>(imageBuffer.data()) + offset,
             size - offset, 0);

    if (amount <= 0) {
      break;
    } else {
      offset += amount;
    }
  }

  Sleep(20);
}

bool TcpClient::start() {
  if (connect(this->connection, reinterpret_cast<SOCKADDR *>(&this->addr),
              sizeSockAddr) != 0) {
    return false;
  }
  UUID uuid;
  UuidCreate(&uuid);
  char *str;
  UuidToStringA(&uuid, reinterpret_cast<RPC_CSTR *>(&str));

  this->inputHandler(L"ID: " + this->dt->s2ws(str), 'c');
  this->outputThread = CreateThread(nullptr, 0, StaticHandlerStart,
                                    static_cast<LPVOID>(this), 0, 0);
  return true;
}

void TcpClient::stop() {
  if (this->outputThread) {
    this->inputHandler(L"", 'd');
    closesocket(this->connection);
    TerminateThread(this->outputThread, 0);
    CloseHandle(this->outputThread);
  }
  this->outputThread = nullptr;
}