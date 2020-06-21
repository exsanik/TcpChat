#include "App.h"

void DataTransformer::handleString(std::string message) {
  try {
    std::wstring utfMessage = s2ws(message);

    int offset = 0;
    do {
      int currOffset = min(utfMessage.size() - offset, 50);
      std::wstring wtemp = utfMessage.substr(offset, currOffset);

      SendMessage((*this->uiHandlers)[App::CTRL_ID::CHATLISTBOX_ID],
                  LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(wtemp.c_str()));

      offset += currOffset;
    } while (offset != utfMessage.size());
  } catch (std::exception& err) {
    SendMessage((*this->uiHandlers)[App::CTRL_ID::CHATLISTBOX_ID], LB_ADDSTRING,
                0, reinterpret_cast<LPARAM>(s2ws(err.what()).c_str()));
  }
}

void DataTransformer::handleImage(std::vector<uint8_t>& imageBuffer) {
  gdiImage->setBuffer(imageBuffer);
  gdiImage->imageFromBuffer();
  InvalidateRect(this->mHwnd, 0, 0);
}

std::wstring DataTransformer::s2ws(const std::string& str) {
  std::wostringstream wstm;
  const std::ctype<wchar_t>& ctfacet =
      std::use_facet<std::ctype<wchar_t> >(wstm.getloc());
  for (size_t i = 0; i < str.size(); ++i) wstm << ctfacet.widen(str[i]);
  return wstm.str();
}

std::string DataTransformer::ws2s(const std::wstring& str) {
  std::ostringstream stm;
  const std::ctype<char>& ctfacet =
      std::use_facet<std::ctype<char> >(stm.getloc());
  for (size_t i = 0; i < str.size(); ++i) stm << ctfacet.narrow(str[i], 0);
  return stm.str();
}
