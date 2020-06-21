#pragma once
#pragma comment(lib, "ws2_32.lib")
#pragma comment(linker, \
                "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <WinSock2.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Ws2tcpip.h>

#include <algorithm>
#include <chrono>
#include <execution>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "RegistryManipulator.h"
#include "TcpServer.h"

class App {
  enum class CTRL_ID {
    STARTBUTTON_ID,
    STOPBUTTON_ID,
    PORTINPUT_ID,
    PORTLABEL_ID
  };
  TcpServer server;
  HANDLE tcpServerHandle;

  RegistryManipulator regManip;

  UINT uiHookMessage = WM_NULL;
  HMODULE hHookDLL = NULL;
  BOOL(CALLBACK* SetKeyboardHook)(HWND hParentWnd) = NULL;
  VOID(CALLBACK* UnhookKeyboardHook)() = NULL;

 public:
  explicit App();
  ~App();

  int run();

 private:
  void initNativeWindowObj();

  static LRESULT CALLBACK applicationProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                          LPARAM lParam);
  LRESULT CALLBACK windowProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                              LPARAM lParam);
  void createNativeControls();
  static DWORD WINAPI StaticTcpServerStart(LPVOID Param);
  void switchControl(CTRL_ID ID, bool state);

 private:
  const std::wstring mSzAppName{L"TcpServer"};
  const std::wstring mSzClassName{L"TcpServer_Window_class"};

  HWND mHwnd, mHwndStartButton, mHwndStopButton, mHwndPortInput, mHwndPortLabel;
  const int mAppWidth = 800, mAppHeight = 600;
};

// helpers
std::string ws2s(const std::wstring& str);
std::wstring s2ws(const std::string& str);