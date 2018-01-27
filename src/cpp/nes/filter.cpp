#include "filter.h"

#include <math.h>

namespace nes {

float32 Filter::Step(float32 x) {
  float32 y = B0*x + B1*prevX - A1*prevY;
  prevY = y;
  prevX = x;
  return y;
}

// sampleRate: samples per second
// cutoffFreq: oscillations per second
Filter* Filter::LowPassFilter(float32 sampleRate, float32 cutoffFreq) {
	float32 c = sampleRate / M_PI / cutoffFreq;
	float32 a0i = 1 / (1 + c);
	return new Filter(a0i, a0i, (1 - c) * a0i);
}

Filter* Filter::HighPassFilter(float32 sampleRate, float32 cutoffFreq) {
	float32 c = sampleRate / M_PI / cutoffFreq;
	float32 a0i = 1 / (1 + c);
	return new Filter(c * a0i, -c * a0i, (1 - c) * a0i);
}

float32 FilterChain::Step(float32 x) {
  if (initialized) {
    x = filter1->Step(x);
    x = filter2->Step(x);
    x = filter3->Step(x);
  }
  return x;
}

}  // namespace nes
