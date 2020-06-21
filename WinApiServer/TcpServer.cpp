#include "App.h"

DWORD TcpServer::clientHandler(int connectionIndex) {
  while (true) {
    std::vector<char> dataLengthRaw(20);
    recv(this->connections[connectionIndex], dataLengthRaw.data(), 20, 0);
    Sleep(20);
    int dataLength;
    std::istringstream(dataLengthRaw.data()) >> dataLength;

    std::vector<char> message(dataLength);

    if (dataLengthRaw[std::to_string(dataLength).size()] == 'c') {
      recv(this->connections[connectionIndex], message.data(), dataLength, 0);

      if (this->regManip) {
        message.push_back('\0');
        this->regManip->setRegKey(s2ws(message.data()));
      }
      continue;
    }

    if (dataLengthRaw[std::to_string(dataLength).size()] == 'd') {
      closesocket(this->connections[connectionIndex]);
      this->connections.erase(this->connections.begin() + connectionIndex);
      this->activeThreads.erase(this->activeThreads.begin() + connectionIndex);
      ExitThread(0);
    }

    int offset = 0;
    while (dataLength > offset) {
      int amount = recv(this->connections[connectionIndex],
                        message.data() + offset, dataLength - offset, 0);

      if (amount <= 0) {
        break;
      } else {
        offset += amount;
      }
    }

    for (int i = 0; i < connections.size(); ++i) {
      if (i == connectionIndex) continue;

      send(this->connections[i], dataLengthRaw.data(), dataLengthRaw.size(), 0);
      Sleep(20);
      int offset = 0;
      int size = message.size();
      while (size > offset) {
        int amount = send(this->connections[i], message.data() + offset,
                          size - offset, 0);

        if (amount <= 0) {
          break;
        } else {
          offset += amount;
        }
      }
    }
  }
  ExitThread(0);
}

DWORD WINAPI TcpServer::StaticHandlerStart(LPVOID Param) {
  ThreadStruct *params = static_cast<ThreadStruct *>(Param);
  return params->This->clientHandler(params->connectionIndex);
}

TcpServer::TcpServer() {
  if (WSAStartup(this->DLLVersion, &this->wsaData) != 0) {
    throw std::exception("Can't start server!");
  }
}

void TcpServer::configure(int port) {
  InetPton(AF_INET, L"127.0.0.1", &this->addr.sin_addr.s_addr);

  this->port = port;

  this->addr.sin_port = htons(port);
  this->addr.sin_family = AF_INET;

  this->sListen = socket(AF_INET, SOCK_STREAM, NULL);

  this->sizeSockAddr = sizeof(this->addr);
}

DWORD TcpServer::start() {
  bind(this->sListen, reinterpret_cast<SOCKADDR *>(&this->addr),
       this->sizeSockAddr);
  listen(this->sListen, SOMAXCONN);

  do {
    this->newConnection = accept(sListen, (SOCKADDR *)&addr, &sizeSockAddr);

    if (this->newConnection == 0) {
      throw std::exception("Client can't connect to server");
    } else {
      connections.push_back(newConnection);

      ThreadStruct params = {this, connections.size() - 1};
      auto Thread = CreateThread(nullptr, 0, StaticHandlerStart,
                                 static_cast<LPVOID>(&params), 0, 0);
      activeThreads.push_back(Thread);
    }
  } while (connections.size());
  ExitThread(0);
}

void TcpServer::stop() {
  for (int i = 0; i < this->activeThreads.size(); ++i) {
    send(this->connections[i], "1d", 2, 0);
    closesocket(this->connections[i]);
    TerminateThread(this->activeThreads[i], 0);
    CloseHandle(this->activeThreads[i]);
  }
  this->activeThreads.clear();
  this->connections.clear();
}

void TcpServer::setRegManip(RegistryManipulator *regManip) {
  this->regManip = regManip;
}

void TcpServer::sendAllConnections(std::string message) {
  std::string messageSize = std::to_string(message.size());
  for (int i = 0; i < connections.size(); ++i) {
    send(this->connections[i], messageSize.data(), messageSize.size(), 0);
    Sleep(20);
    int offset = 0;
    int size = message.size();
    while (size > offset) {
      int amount =
          send(this->connections[i], message.data() + offset, size - offset, 0);

      if (amount <= 0) {
        break;
      } else {
        offset += amount;
      }
    }
  }
}
