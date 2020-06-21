#include "App.h"

GdiImage::GdiImage() {}

GdiImage::~GdiImage() {}

void GdiImage::readImageFromFileToBuffer(std::wstring fileName) {
  this->fileName = fileName;

  std::ifstream fileStream(this->fileName, std::ios::binary);
  this->imageBuffer.assign(std::istreambuf_iterator<char>{fileStream}, {});
  fileStream.close();
}

Gdiplus::Bitmap* GdiImage::imageFromBuffer() {
  CComPtr<IStream> stream;
  stream.Attach(
      SHCreateMemStream(this->imageBuffer.data(), this->imageBuffer.size()));
  this->bmp = Gdiplus::Bitmap::FromStream(stream);
  stream.Detach();
  return this->bmp;
}

Gdiplus::Bitmap* GdiImage::getBitmap() { return this->bmp; }
std::vector<uint8_t> GdiImage::getBuffer() { return this->imageBuffer; }

void GdiImage::setBuffer(const std::vector<uint8_t>& imageBuffer) {
  this->imageBuffer = imageBuffer;
}
