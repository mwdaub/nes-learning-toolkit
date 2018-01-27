#include "controller.h"

namespace nes {

uint8 Controller::Read() {
  uint8 val = 0;
  if (index < 8 && (buttons & (1 << index))) {
    val = 1;
  }
  index++;
  if ((strobe&1) == 1) {
    index = 0;
  }
  return val;
}

void Controller::Write(uint8 val) {
  strobe = val;
  if ((strobe&1) == 1) {
    index = 0;
  }
}

}  // namespace nes
