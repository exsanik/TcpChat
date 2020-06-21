#pragma once

class TcpServer {
 private:
  int port = 3000;
  int sizeSockAddr;

  WSAData wsaData;
  WORD DLLVersion = MAKEWORD(2, 1);

  SOCKADDR_IN addr;
  SOCKET sListen;
  std::vector<SOCKET> connections;
  std::vector<HANDLE> activeThreads;
  SOCKET newConnection = 0;

  struct ThreadStruct {
    TcpServer *This;
    int connectionIndex;
  };

  RegistryManipulator *regManip;

 private:
  DWORD clientHandler(int connectionIndex);
  static DWORD WINAPI StaticHandlerStart(LPVOID Param);

 public:
  explicit TcpServer();

  void configure(int port);
  DWORD start();
  void stop();
  void setRegManip(RegistryManipulator *regManip);
  void sendAllConnections(std::string message);
};