#pragma once
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "rpcrt4.lib")
#pragma comment(linker, \
                "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#define WIN32_LEAN_AND_MEAN
#include <Gdiplus.h>
#include <Gdiplusbrush.h>
#include <Shlwapi.h>
#include <Windows.h>
#include <Windowsx.h>
#include <Ws2tcpip.h>
#include <atlbase.h>

#include <algorithm>
#include <chrono>
#include <codecvt>
#include <execution>
#include <fstream>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>
#include <strstream>
#include <thread>
#include <vector>

#include "DataTransformer.h"
#include "TcpClient.h"
#include "resource.h"

class App {
  enum CTRL_ID {
    STARTBUTTON_ID,
    STOPBUTTON_ID,

    PORTINPUT_ID,
    PORTLABEL_ID,

    IPINPUT_ID,
    IPLABEL_ID,

    NAMEINPUT_ID,
    NAMELABEL_ID,

    MSGINPUT_ID,
    SENDTEXTBUTTON_ID,
    SENDIMAGEBUTTON_ID,

    CHATLISTBOX_ID,

    UIITEMS_LENGTH
  };

  TcpClient* client;
  bool clientState;

  Gdiplus::GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR gdiplusToken;

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
  void switchControl(CTRL_ID ID, bool state);
  void loadImage();
  void drawImage(HDC hdc);
  void addTrayIcon(UINT uID, UINT uCallbackMsg, UINT uIcon);
  void removeTrayIcon(UINT uID);
  void sendImage();

 private:
  const std::wstring mSzAppName{L"TcpClient"};
  const std::wstring mSzClassName{L"TcpClient_Window_class"};

  HWND mHwnd;
  std::vector<HWND> uiHandles;
  GdiImage gdiImage;
  DataTransformer* dt;

  const int mAppWidth = 800, mAppHeight = 600;

  friend DataTransformer;
};
