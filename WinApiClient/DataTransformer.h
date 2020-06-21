#pragma once
#include "GdiImage.h"

class DataTransformer {
 private:
  std::vector<HWND>* uiHandlers;
  HWND mHwnd;
  GdiImage* gdiImage;

 public:
  DataTransformer(std::vector<HWND>& uiHandlers, HWND& mHwnd,
                  GdiImage& gdiImage)
      : uiHandlers(&uiHandlers), mHwnd(mHwnd), gdiImage(&gdiImage) {}

  void handleString(std::string message);
  void handleImage(std::vector<uint8_t>& imageBuffer);

  std::wstring s2ws(const std::string& str);
  std::string ws2s(const std::wstring& wstr);
  const wchar_t* GetWC(const char* c);
};