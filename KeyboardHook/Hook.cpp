#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

HINSTANCE hInstance = NULL;
HWND hLauncherWnd = NULL;
UINT uiHookMessage = WM_NULL;
HHOOK hKeyboardHook = NULL;

LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
  LPKBDLLHOOKSTRUCT l = (LPKBDLLHOOKSTRUCT)lParam;
  if (wParam == WM_KEYDOWN && hLauncherWnd) {
    if (l->vkCode >= 'A' && l->vkCode <= 'Z' &&
        (GetKeyState(VK_CONTROL) & 0x8000)) {
      PostMessage(hLauncherWnd, uiHookMessage, wParam, (LPARAM)l->vkCode);
    }
  }
  return CallNextHookEx(hKeyboardHook, code, wParam, lParam);
}

extern "C" {

__declspec(dllexport) LRESULT CALLBACK SetKeyboardHook(HWND hParentWnd) {
  hLauncherWnd = hParentWnd;
  if (!hKeyboardHook)
    hKeyboardHook =
        SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, hInstance, 0);
  return hKeyboardHook ? TRUE : FALSE;
}

__declspec(dllexport) VOID CALLBACK UnhookKeyboardHook() {
  if (hKeyboardHook) {
    UnhookWindowsHookEx(hKeyboardHook);
  }
  hKeyboardHook = NULL;
  hLauncherWnd = NULL;
}
}

BOOL WINAPI DllMain(HINSTANCE hDLL, DWORD dwReason, LPVOID lpvReserved) {
  switch (dwReason) {
    case DLL_PROCESS_ATTACH: {
      hInstance = hDLL;
      uiHookMessage = RegisterWindowMessage(L"HookMessage");
      break;
    }
    case DLL_PROCESS_DETACH:
      break;
  }
  return TRUE;
}