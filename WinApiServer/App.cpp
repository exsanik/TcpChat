#include "App.h"

App::App() {
  using std::string;
  using std::wstring;

  try {
    this->initNativeWindowObj();
    this->createNativeControls();

    this->server.setRegManip(&this->regManip);
    this->server.configure(3000);
  } catch (const std::exception &e) {
    string exepData = e.what();

    MessageBox(nullptr, wstring(begin(exepData), end(exepData)).c_str(),
               L"Error", MB_ICONERROR | MB_OK);
    ExitProcess(EXIT_FAILURE);
  }
}

App::~App() {}

int App::run() {
  MSG msg;
  ShowWindow(this->mHwnd, SW_SHOWDEFAULT);
  UpdateWindow(this->mHwnd);
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return static_cast<int>(msg.wParam);
}

void App::initNativeWindowObj() {
  WNDCLASSEX wc{sizeof(WNDCLASSEX)};
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
  wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
  wc.hInstance = GetModuleHandle(nullptr);
  wc.lpfnWndProc = App::applicationProc;
  wc.lpszClassName = this->mSzClassName.c_str();
  wc.lpszMenuName = nullptr;
  wc.style = CS_VREDRAW | CS_HREDRAW;

  if (!RegisterClassEx(&wc)) {
    throw std::exception("Error, can't register window class!");
  }

  RECT windowRC{0, 0, this->mAppWidth, this->mAppHeight};
  AdjustWindowRect(&windowRC, WS_OVERLAPPEDWINDOW, false);

  this->mHwnd = CreateWindowEx(
      0, this->mSzClassName.c_str(), this->mSzAppName.c_str(),
      WS_DLGFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZE,
      ((GetSystemMetrics(SM_CXSCREEN) - windowRC.right) / 2), 0, windowRC.right,
      windowRC.bottom, nullptr, nullptr, nullptr, this);

  if (!this->mHwnd) {
    throw std::exception("Error can't create main window!");
  }
}

void App::createNativeControls() {
  this->mHwndStartButton = CreateWindowEx(
      0, L"BUTTON", L"Start Server", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 300,
      200, 150, 50, this->mHwnd,
      reinterpret_cast<HMENU>(App::CTRL_ID::STARTBUTTON_ID), nullptr, nullptr);
  if (!this->mHwndStartButton) {
    throw std::exception("Error can't create button!");
  }

  this->mHwndStopButton = CreateWindowEx(
      0, L"BUTTON", L"Stop Server", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 300,
      300, 150, 50, this->mHwnd,
      reinterpret_cast<HMENU>(App::CTRL_ID::STOPBUTTON_ID), nullptr, nullptr);
  if (!this->mHwndStopButton) {
    throw std::exception("Error can't create button!");
  }
  this->mHwndPortLabel = CreateWindowEx(
      WS_EX_CLIENTEDGE, L"STATIC", L"Port:", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      330, 150, 50, 24, this->mHwnd,
      reinterpret_cast<HMENU>(App::CTRL_ID::PORTLABEL_ID), nullptr, nullptr);

  this->mHwndPortInput = CreateWindowEx(
      WS_EX_CLIENTEDGE, L"EDIT", L"3000", WS_CHILD | WS_VISIBLE | ES_NUMBER,
      370, 150, 50, 24, this->mHwnd,
      reinterpret_cast<HMENU>(App::CTRL_ID::PORTINPUT_ID), nullptr, nullptr);
  if (!this->mHwndPortInput) {
    throw std::exception("Error can't create input!");
  }

  this->switchControl(CTRL_ID::STOPBUTTON_ID, false);

  HFONT hFont = CreateFont(18, 0, 0, 0, FW_REGULAR, 0, 0, 0, DEFAULT_CHARSET,
                           OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Roboto");
  SendMessage(this->mHwndStartButton, WM_SETFONT,
              reinterpret_cast<WPARAM>(hFont), true);
  SendMessage(this->mHwndStopButton, WM_SETFONT,
              reinterpret_cast<WPARAM>(hFont), true);
}

LRESULT App::applicationProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                             LPARAM lParam) {
  App *pApp;
  if (uMsg == WM_NCCREATE) {
    pApp = static_cast<App *>(
        reinterpret_cast<CREATESTRUCT *>(lParam)->lpCreateParams);
    SetLastError(0);
    if (!SetWindowLongPtr(hWnd, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(pApp))) {
      if (GetLastError() != 0) {
        return false;
      }
    }
  } else {
    pApp = reinterpret_cast<App *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
  }

  if (pApp) {
    pApp->mHwnd = hWnd;
    return pApp->windowProc(hWnd, uMsg, wParam, lParam);
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT App::windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_CREATE: {
      this->uiHookMessage = RegisterWindowMessage(L"HookMessage");
      this->hHookDLL = LoadLibrary(L"KeyboardHook.dll");
      if (this->hHookDLL) {
        reinterpret_cast<FARPROC &>(this->SetKeyboardHook) =
            GetProcAddress(this->hHookDLL, "_SetKeyboardHook@4");

        reinterpret_cast<FARPROC &>(this->UnhookKeyboardHook) =
            GetProcAddress(this->hHookDLL, "_UnhookKeyboardHook@0");
      }
      if (this->SetKeyboardHook) {
        this->SetKeyboardHook(this->mHwnd);
      }

      return 0;
    }
    case WM_COMMAND: {
      switch (static_cast<App::CTRL_ID>(LOWORD(wParam))) {
        case App::CTRL_ID::STARTBUTTON_ID: {
          std::vector<WCHAR> portRaw(7);
          GetWindowText(this->mHwndPortInput, portRaw.data(), 7);
          int port = _wtoi(portRaw.data());

          if (port < 1000) {
            port = 3000;
            MessageBox(this->mHwnd, L"Port was set to 3000", L"Warning",
                       MB_ICONWARNING | MB_OK);
          }
          this->server.configure(port);

          this->tcpServerHandle = CreateThread(nullptr, 0, StaticTcpServerStart,
                                               (void *)this, 0, 0);

          std::wstring infoMsg =
              L"Server started on port " + std::to_wstring(port);
          MessageBox(this->mHwnd, infoMsg.data(), L"Warning",
                     MB_ICONINFORMATION | MB_OK);

          this->switchControl(CTRL_ID::STOPBUTTON_ID, true);
          this->switchControl(CTRL_ID::STARTBUTTON_ID, false);
          break;
        }
        case App::CTRL_ID::STOPBUTTON_ID: {
          MessageBox(this->mHwnd, L"Server was stoped!", L"Info",
                     MB_ICONINFORMATION | MB_OK);

          this->server.stop();
          TerminateThread(this->tcpServerHandle, 0);
          CloseHandle(this->tcpServerHandle);
          this->tcpServerHandle = nullptr;

          this->switchControl(CTRL_ID::STOPBUTTON_ID, false);
          this->switchControl(CTRL_ID::STARTBUTTON_ID, true);
          break;
        }
        default:
          break;
      }
      return 0;
    }
    case WM_CLOSE: {
      this->regManip.deleteAllKeys();

      if (this->UnhookKeyboardHook) {
        this->UnhookKeyboardHook();
      }

      return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    case WM_DESTROY: {
      PostQuitMessage(EXIT_SUCCESS);
      return 0;
    }

    default: {
      if (this->uiHookMessage == uMsg) {
        if (lParam == 'R') {
          if (!this->tcpServerHandle) {
            MessageBox(this->mHwnd, L"Server have not started yet!", L"Info",
                       MB_ICONINFORMATION | MB_OK);
          } else {
            std::wstring users = this->regManip.getAllKeys();
            this->server.sendAllConnections("Server: " + ws2s(users));
            MessageBox(this->mHwnd, L"A message was sent!", L"Info",
                       MB_ICONINFORMATION | MB_OK);
          }
        }
      }
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
  }
}

DWORD WINAPI App::StaticTcpServerStart(LPVOID Param) {
  App *This = static_cast<App *>(Param);
  return This->server.start();
}

void App::switchControl(CTRL_ID ID, bool state) {
  EnableWindow(GetDlgItem(this->mHwnd, static_cast<int>(ID)), state);
}

// helpers

std::wstring s2ws(const std::string &str) {
  std::wostringstream wstm;
  const std::ctype<wchar_t> &ctfacet =
      std::use_facet<std::ctype<wchar_t> >(wstm.getloc());
  for (size_t i = 0; i < str.size(); ++i) wstm << ctfacet.widen(str[i]);
  return wstm.str();
}

std::string ws2s(const std::wstring &str) {
  std::ostringstream stm;
  const std::ctype<char> &ctfacet =
      std::use_facet<std::ctype<char> >(stm.getloc());
  for (size_t i = 0; i < str.size(); ++i) stm << ctfacet.narrow(str[i], 0);
  return stm.str();
}