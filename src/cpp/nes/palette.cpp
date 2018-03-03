#include "palette.h"

#include <cstring>

#include "utils.h"

using std::make_shared;

namespace nes {

const RGB Palette[64] = {
    RGB(0x666666), RGB(0x002A88), RGB(0x1412A7), RGB(0x3B00A4),
    RGB(0x5C007E), RGB(0x6E0040), RGB(0x6C0600), RGB(0x561D00),
    RGB(0x333500), RGB(0x0B4800), RGB(0x005200), RGB(0x004F08),
    RGB(0x00404D), RGB(0x000000), RGB(0x000000), RGB(0x000000),
    RGB(0xADADAD), RGB(0x155FD9), RGB(0x4240FF), RGB(0x7527FE),
    RGB(0xA01ACC), RGB(0xB71E7B), RGB(0xB53120), RGB(0x994E00),
    RGB(0x6B6D00), RGB(0x388700), RGB(0x0C9300), RGB(0x008F32),
    RGB(0x007C8D), RGB(0x000000), RGB(0x000000), RGB(0x000000),
    RGB(0xFFFEFF), RGB(0x64B0FF), RGB(0x9290FF), RGB(0xC676FF),
    RGB(0xF36AFF), RGB(0xFE6ECC), RGB(0xFE8170), RGB(0xEA9E22),
    RGB(0xBCBE00), RGB(0x88D800), RGB(0x5CE430), RGB(0x45E082),
    RGB(0x48CDDE), RGB(0x4F4F4F), RGB(0x000000), RGB(0x000000),
    RGB(0xFFFEFF), RGB(0xC0DFFF), RGB(0xD3D2FF), RGB(0xE8C8FF),
    RGB(0xFBC2FF), RGB(0xFEC4EA), RGB(0xFECCC5), RGB(0xF7D8A5),
    RGB(0xE4E594), RGB(0xCFEF96), RGB(0xBDF4AB), RGB(0xB3F3CC),
    RGB(0xB5EBF2), RGB(0xB8B8B8), RGB(0x000000), RGB(0x000000)
};

void Screen::SetPixel(int32 x, int32 y, uint8 val) {
  int i = (y*kWidth) + x;
  pixels[i] = val;
};

void Screen::GetPixelIndexes(uint8* pixel_indexes) {
  std::memcpy(pixel_indexes, pixels.data(), kNumPixels);
}

void Screen::GetPixelValues(uint8* pixel_values) {
  for (uint32 pixel = 0; pixel < kNumPixels; pixel++) {
    uint8 index = pixels[pixel];
    const RGB* rgb = &Palette[index];
    *(pixel_values++) = rgb->R;
    *(pixel_values++) = rgb->G;
    *(pixel_values++) = rgb->B;
  }
}

void Screen::Save(ostream& out) {
  utils::writeUint8Array(out, pixels);
}

void Screen::SaveValues(ostream& out) {
  array<uint8, kNumPixelValues> values;
  GetPixelValues(values.data());
  utils::writeUint8Array(out, values);
}

void Screen::Load(istream& in) {
  utils::readUint8Array(in, pixels);
}

void AudioChannel::Write(float32 value) {
  if (position < values.size()) {
    values[position] = value;
    position++;
  }
}

}  // namespace nes
