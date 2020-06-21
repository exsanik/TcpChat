#include "App.h"

App::App() {
  using std::string;
  using std::wstring;

  try {
    this->dt =
        new DataTransformer(this->uiHandles, this->mHwnd, this->gdiImage);
    this->client = new TcpClient(dt);
    Gdiplus::GdiplusStartup(&this->gdiplusToken, &this->gdiplusStartupInput,
                            NULL);

    uiHandles.resize(CTRL_ID::UIITEMS_LENGTH);
    this->initNativeWindowObj();
    this->createNativeControls();

  } catch (const std::exception& e) {
    string exepData = e.what();

    MessageBox(nullptr, wstring(begin(exepData), end(exepData)).c_str(),
               L"Error", MB_ICONERROR | MB_OK);
    ExitProcess(EXIT_FAILURE);
  }
}

App::~App() { Gdiplus::GdiplusShutdown(this->gdiplusToken); }

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
  wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
  wc.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
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
  this->uiHandles[CTRL_ID::IPLABEL_ID] = CreateWindowEx(
      WS_EX_CLIENTEDGE, L"STATIC", L"IP:", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      10, 10, 30, 24, this->mHwnd, reinterpret_cast<HMENU>(CTRL_ID::IPLABEL_ID),
      nullptr, nullptr);

  this->uiHandles[CTRL_ID::IPINPUT_ID] = CreateWindowEx(
      WS_EX_CLIENTEDGE, L"EDIT", L"127.0.0.1", WS_CHILD | WS_VISIBLE, 50, 10,
      120, 24, this->mHwnd, reinterpret_cast<HMENU>(App::CTRL_ID::IPINPUT_ID),
      nullptr, nullptr);

  this->uiHandles[CTRL_ID::PORTLABEL_ID] = CreateWindowEx(
      WS_EX_CLIENTEDGE, L"STATIC", L"Port:", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      200, 10, 50, 24, this->mHwnd,
      reinterpret_cast<HMENU>(CTRL_ID::PORTLABEL_ID), nullptr, nullptr);

  this->uiHandles[CTRL_ID::PORTINPUT_ID] = CreateWindowEx(
      WS_EX_CLIENTEDGE, L"EDIT", L"3000", WS_CHILD | WS_VISIBLE | ES_NUMBER,
      260, 10, 50, 24, this->mHwnd,
      reinterpret_cast<HMENU>(App::CTRL_ID::PORTINPUT_ID), nullptr, nullptr);

  this->uiHandles[CTRL_ID::NAMELABEL_ID] = CreateWindowEx(
      WS_EX_CLIENTEDGE, L"STATIC", L"Name:", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      340, 10, 50, 24, this->mHwnd,
      reinterpret_cast<HMENU>(CTRL_ID::NAMELABEL_ID), nullptr, nullptr);

  this->uiHandles[CTRL_ID::NAMEINPUT_ID] = CreateWindowEx(
      WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE, 400, 10, 150, 24,
      this->mHwnd, reinterpret_cast<HMENU>(App::CTRL_ID::NAMEINPUT_ID), nullptr,
      nullptr);

  this->uiHandles[CTRL_ID::STARTBUTTON_ID] = CreateWindowEx(
      0, L"BUTTON", L"Connect", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 570, 10,
      100, 24, this->mHwnd,
      reinterpret_cast<HMENU>(App::CTRL_ID::STARTBUTTON_ID), nullptr, nullptr);

  this->uiHandles[CTRL_ID::STOPBUTTON_ID] = CreateWindowEx(
      0, L"BUTTON", L"Disconnect", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 680,
      10, 100, 24, this->mHwnd,
      reinterpret_cast<HMENU>(App::CTRL_ID::STOPBUTTON_ID), nullptr, nullptr);

  this->uiHandles[CTRL_ID::MSGINPUT_ID] = CreateWindowEx(
      WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE, 10,
      510, 600, 48, this->mHwnd,
      reinterpret_cast<HMENU>(App::CTRL_ID::MSGINPUT_ID), nullptr, nullptr);

  this->uiHandles[CTRL_ID::SENDTEXTBUTTON_ID] = CreateWindowEx(
      0, L"BUTTON", L"Send text", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 620,
      510, 70, 48, this->mHwnd,
      reinterpret_cast<HMENU>(App::CTRL_ID::SENDTEXTBUTTON_ID), nullptr,
      nullptr);

  this->uiHandles[CTRL_ID::SENDIMAGEBUTTON_ID] = CreateWindowEx(
      0, L"BUTTON", L"Send Image", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 700,
      510, 85, 48, this->mHwnd,
      reinterpret_cast<HMENU>(App::CTRL_ID::SENDIMAGEBUTTON_ID), nullptr,
      nullptr);

  this->uiHandles[CTRL_ID::CHATLISTBOX_ID] = CreateWindowEx(
      0, L"LISTBOX", 0, WS_VSCROLL | WS_VISIBLE | WS_CHILD, 10, 50, 420, 450,
      this->mHwnd, reinterpret_cast<HMENU>(App::CTRL_ID::CHATLISTBOX_ID),
      nullptr, nullptr);

  for (auto uiElement : uiHandles) {
    if (!uiElement) {
      throw std::exception("Can't create ui elements!");
    }
  }

  this->switchControl(CTRL_ID::STOPBUTTON_ID, false);
  this->switchControl(CTRL_ID::SENDIMAGEBUTTON_ID, false);
  this->switchControl(CTRL_ID::SENDTEXTBUTTON_ID, false);

  HFONT hFont = CreateFont(17, 0, 0, 0, FW_REGULAR, 0, 0, 0, DEFAULT_CHARSET,
                           OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Roboto");
  SendMessage(this->uiHandles[CTRL_ID::CHATLISTBOX_ID], WM_SETFONT,
              reinterpret_cast<WPARAM>(hFont), true);
  SendMessage(this->uiHandles[CTRL_ID::MSGINPUT_ID], WM_SETFONT,
              reinterpret_cast<WPARAM>(hFont), true);
}

LRESULT App::applicationProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                             LPARAM lParam) {
  App* pApp;
  if (uMsg == WM_NCCREATE) {
    pApp = static_cast<App*>(
        reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
    SetLastError(0);
    if (!SetWindowLongPtr(hWnd, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(pApp))) {
      if (GetLastError() != 0) {
        return false;
      }
    }
  } else {
    pApp = reinterpret_cast<App*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
  }

  if (pApp) {
    pApp->mHwnd = hWnd;
    return pApp->windowProc(hWnd, uMsg, wParam, lParam);
  }
  return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT App::windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  HDC hdc;
  PAINTSTRUCT ps;

  switch (uMsg) {
    case WM_CREATE: {
      this->addTrayIcon(1, WM_APP, 0);
      this->uiHookMessage = RegisterWindowMessage(L"HookMessage");

      this->hHookDLL = LoadLibrary(L"KeyboardHook.dll");
      if (hHookDLL) {
        reinterpret_cast<FARPROC&>(this->SetKeyboardHook) =
            GetProcAddress(this->hHookDLL, "_SetKeyboardHook@4");

        reinterpret_cast<FARPROC&>(this->UnhookKeyboardHook) =
            GetProcAddress(this->hHookDLL, "_UnhookKeyboardHook@0");
      }
      if (this->SetKeyboardHook) {
        this->SetKeyboardHook(this->mHwnd);
      }

      return 0;
    }

    case WM_PAINT: {
      hdc = BeginPaint(this->mHwnd, &ps);
      this->drawImage(hdc);
      EndPaint(this->mHwnd, &ps);
      return 0;
    }

    case WM_COMMAND: {
      switch (static_cast<App::CTRL_ID>(LOWORD(wParam))) {
        case App::CTRL_ID::STARTBUTTON_ID: {
          std::vector<WCHAR> portRaw(5);
          GetWindowText(this->uiHandles[CTRL_ID::PORTINPUT_ID], portRaw.data(),
                        5);
          int port = _wtoi(portRaw.data());
          if (port < 1000) {
            port = 3000;
            MessageBox(this->mHwnd, L"Wrong port! Port was set to 3000",
                       L"Warning", MB_ICONWARNING | MB_OK);
          }

          std::vector<WCHAR> ipRaw(15);
          GetWindowText(this->uiHandles[CTRL_ID::IPINPUT_ID], ipRaw.data(), 15);

          std::vector<WCHAR> nameRaw(20);
          GetWindowText(this->uiHandles[CTRL_ID::NAMEINPUT_ID], nameRaw.data(),
                        20);

          this->client->configure(port, nameRaw.data(), ipRaw.data());
          this->clientState = this->client->start();

          if (!this->clientState) {
            MessageBox(this->mHwnd, L"Client can't connect to server! :(",
                       L"Warning", MB_ICONWARNING | MB_OK);

          } else {
            MessageBox(this->mHwnd, L"Client connected to server!", L"Info",
                       MB_ICONINFORMATION | MB_OK);
            this->switchControl(CTRL_ID::STOPBUTTON_ID, true);
            this->switchControl(CTRL_ID::STARTBUTTON_ID, false);
            this->switchControl(CTRL_ID::SENDIMAGEBUTTON_ID, true);
            this->switchControl(CTRL_ID::SENDTEXTBUTTON_ID, true);
          }

          break;
        }
        case App::CTRL_ID::STOPBUTTON_ID: {
          this->client->stop();
          this->clientState = false;
          MessageBox(this->mHwnd, L"Client disconnected!", L"Info",
                     MB_ICONINFORMATION | MB_OK);

          this->switchControl(CTRL_ID::STOPBUTTON_ID, false);
          this->switchControl(CTRL_ID::STARTBUTTON_ID, true);
          this->switchControl(CTRL_ID::SENDIMAGEBUTTON_ID, false);
          this->switchControl(CTRL_ID::SENDTEXTBUTTON_ID, false);
          break;
        }
        case App::CTRL_ID::SENDTEXTBUTTON_ID: {
          std::vector<WCHAR> message(255);
          GetWindowText(this->uiHandles[CTRL_ID::MSGINPUT_ID], message.data(),
                        255);
          this->client->inputHandler(message.data());
          SetWindowText(this->uiHandles[CTRL_ID::MSGINPUT_ID], L"");

          break;
        }
        case App::CTRL_ID::SENDIMAGEBUTTON_ID: {
          this->sendImage();
          break;
        }
        default:
          break;
      }
      return 0;
    }
    case WM_SIZE: {
      if (wParam == SIZE_MINIMIZED) {
        ShowWindow(this->mHwnd, SW_HIDE);
      }
      return 0;
    }

    case WM_CLOSE: {
      removeTrayIcon(1);
      if (this->clientState) {
        this->client->stop();
      }
      if (this->UnhookKeyboardHook) {
        this->UnhookKeyboardHook();
      }

      return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    case WM_DESTROY: {
      PostQuitMessage(EXIT_SUCCESS);
      return 0;
    }

    case WM_APP: {
      switch (lParam) {
        case WM_LBUTTONDBLCLK: {
          SetWindowPos(this->mHwnd, HWND_TOP, 0, 0, 800, 600,
                       SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
          break;
        }
      }
      return 0;
    }

    default: {
      if (this->uiHookMessage == uMsg) {
        if (lParam == 'H') {
          if (!this->clientState) {
            MessageBox(this->mHwnd, L"You are not connected to server!",
                       L"Info", MB_ICONINFORMATION | MB_OK);
          } else {
            this->sendImage();
          }
        }
      }

      return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
  }
}

void App::switchControl(CTRL_ID ID, bool state) {
  EnableWindow(GetDlgItem(this->mHwnd, static_cast<int>(ID)), state);
}

void App::loadImage() {
  OPENFILENAME ofn;
  ZeroMemory(&ofn, sizeof(OPENFILENAME));

  wchar_t fileName[255];
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = this->mHwnd;
  ofn.lpstrFile = fileName;
  ofn.lpstrFile[0] = '\0';
  ofn.nMaxFile = 255;
  ofn.lpstrFilter = L"*.jpg|*.png|*.bmp\0";
  ofn.nFilterIndex = 1;

  GetOpenFileName(&ofn);
  std::wstring fileNameStr(fileName);

  gdiImage.readImageFromFileToBuffer(fileNameStr);
  gdiImage.imageFromBuffer();
}

void App::drawImage(HDC hdc) {
  Gdiplus::Graphics gf(hdc);
  Gdiplus::Bitmap* bmp = gdiImage.getBitmap();
  if (bmp) {
    Gdiplus::LinearGradientBrush linGrBrush(
        Gdiplus::Point(0, 10), Gdiplus::Point(100, 10),
        Gdiplus::Color(255, 255, 0, 0), Gdiplus::Color(255, 0, 0, 255));

    Gdiplus::Pen pen(&linGrBrush, 10);

    gf.DrawImage(bmp, 430, 160, 320, 240);
    gf.DrawLine(&pen, 430, 160, 430 + 320, 160);
    gf.DrawLine(&pen, 430, 160, 430, 160 + 240);
    gf.DrawLine(&pen, 430 + 320, 160, 430 + 320, 160 + 240);
    gf.DrawLine(&pen, 430, 160 + 240, 430 + 320, 160 + 240);
  }
}

void App::addTrayIcon(UINT uID, UINT uCallbackMsg, UINT uIcon) {
  NOTIFYICONDATA nid;
  nid.hWnd = this->mHwnd;
  nid.uID = uID;
  nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
  nid.uCallbackMessage = uCallbackMsg;

  ExtractIconEx(L"./chat-icon.ico", 0, NULL, &(nid.hIcon), 1);

  wcscpy(nid.szTip, L"TcpClient");

  Shell_NotifyIcon(NIM_ADD, &nid);
}

void App::removeTrayIcon(UINT uID) {
  NOTIFYICONDATA nid;
  nid.hWnd = this->mHwnd;
  nid.uID = uID;

  Shell_NotifyIcon(NIM_DELETE, &nid);
}

void App::sendImage() {
  this->loadImage();
  InvalidateRect(this->mHwnd, 0, 0);
  this->client->imageHandler(this->gdiImage.getBuffer());
}