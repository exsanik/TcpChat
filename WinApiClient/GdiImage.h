#pragma once

class GdiImage {
  std::wstring fileName;
  Gdiplus::Bitmap* bmp;
  std::vector<uint8_t> imageBuffer;

 public:
  GdiImage();
  ~GdiImage();

  void readImageFromFileToBuffer(std::wstring fileName);
  Gdiplus::Bitmap* imageFromBuffer();
  Gdiplus::Bitmap* getBitmap();
  std::vector<uint8_t> getBuffer();
  void setBuffer(const std::vector<uint8_t>& imageBuffer);
};