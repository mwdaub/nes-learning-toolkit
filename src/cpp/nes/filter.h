#ifndef NES_FILTER_H
#define NES_FILTER_H

#include "types.h"

namespace nes {

class Filter {
  public:
    Filter(float32 B0, float32 B1, float32 A1) :
        B0(B0),
        B1(B1),
        A1(A1),
        prevX(0.0),
        prevY(0.0) {};

	float32 B0;
	float32 B1;
	float32 A1;
	float32 prevX;
	float32 prevY;

    float32 Step(float32 x);

    static Filter* LowPassFilter(float32 sampleRate, float32 cutoffFreq);
    static Filter* HighPassFilter(float32 sampleRate, float32 cutoffFreq);
};

class FilterChain {
  public:
    Filter* filter1;
    Filter* filter2;
    Filter* filter3;
    bool initialized = false;

    float32 Step(float32 x);
    void SetSampleRate(float32 sampleRate) {
      if (sampleRate == 0.0f) {
        delete filter1;
        filter1 = 0;
        delete filter2;
        filter1 = 0;
        delete filter3;
        filter1 = 0;
        initialized = false;
      } else {
        filter1 = Filter::HighPassFilter(float32(sampleRate), 90);
        filter2 = Filter::HighPassFilter(float32(sampleRate), 440);
        filter3 = Filter::LowPassFilter(float32(sampleRate), 14000);
        initialized = true;
      }
    }
};

}  // namespace nes

#endif  // define NES_FILTER_H
