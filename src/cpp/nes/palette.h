#ifndef NES_PALETTE_H
#define NES_PALETTE_H

#include <array>
#include <iostream>
#include <memory>
#include <vector>

#include "types.h"
#include "utils.h"

using namespace std;

using std::array;
using std::shared_ptr;
using std::vector;

namespace nes {

struct RGB {
  uint8 R;
  uint8 G;
  uint8 B;

  RGB(uint32 x) :
      R(x >> 16),
      G(x >> 8),
      B(x) {}
};

extern const RGB Palette[64];

class Screen {
  public:
    Screen() : pixels{} {};

    static constexpr uint32 kWidth = 256;
    static constexpr uint32 kHeight = 240;
    static constexpr uint32 kNumPixels = kWidth*kHeight;
    static constexpr uint32 kNumChannels = 3;
    static constexpr uint32 kNumPixelValues = kNumPixels*kNumChannels;

    array<uint8, kNumPixels> pixels;

    void SetPixel(int32 x, int32 y, uint8 val);
    void GetPixelIndexes(uint8* data);
    void GetPixelValues(uint8* data);
    void Save(ostream& out);
    void SaveValues(ostream& out);
    void Load(istream& in);
};

class AudioChannel {
  public:
    AudioChannel(uint32 length) :
        values(length),
        position(0) {}

    vector<float32> values;
    uint32 position;

    void Write(float32 value);
    void GetAudioSamples(uint8* data);
    void Save(ostream& out);
    void Reset() { position = 0; };
};

}  // namespace nes

#endif  // define NES_PALETTE_H
