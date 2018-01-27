#ifndef NES_CONTROLLER_H
#define NES_CONTROLLER_H

#include "types.h"

namespace nes {

class Controller {
  public:
    Controller() :
        buttons(0),
        index(0),
        strobe(0) {};

    uint8 buttons;
    uint8 index;
    uint8 strobe;

    void SetButtons(uint8 val) { buttons = val; };
    uint8 Read();
    void Write(uint8 val);
};

}  // namespace nes

#endif  // define NES_CONTROLLER_H
