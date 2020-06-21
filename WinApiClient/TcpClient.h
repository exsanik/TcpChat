#pragma once

class TcpClient {
  int port = 3000;
  int sizeSockAddr = 0;
  std::wstring ip;
  std::wstring name;

  WSAData wsaData;
  WORD DLLVersion = MAKEWORD(2, 1);
  DataTransformer* dt;

  SOCKADDR_IN addr;
  SOCKET connection;

  DWORD outputHandler();
  static DWORD WINAPI StaticHandlerStart(LPVOID Param);

  HANDLE outputThread;

 public:
  explicit TcpClient(DataTransformer* dt);
  bool start();
  void stop();

  void configure(int port, std::wstring name, std::wstring ip = L"127.0.0.1");
  void inputHandler(std::wstring message, char param = ' ');
  void imageHandler(const std::vector<uint8_t>& imageBuffer);
};
