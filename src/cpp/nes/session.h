#ifndef NES_SESSION_H
#define NES_SESSION_H

#include <vector>

#include "types.h"

using namespace std;

namespace nes {

struct Input {
  uint8 buttons;
  uint8 frames;

  Input(uint8 buttons, uint8 frames) :
      buttons(buttons),
      frames(frames) {};
};

class State;
class Console;

class InputSequence {
  public:
    InputSequence(Console* console);

    uint64 startFrame;
    State* state;
    vector<uint8> inputs;

    void RecordInput(uint8 buttons) { inputs.push_back(buttons); };
};

class Console;

class Session {
  public:
    Session(Console* console);
    ~Session() {
      delete input;
      input = NULL;
    }

    Console* console;
    InputSequence* input;

    void RecordFrame();
};

}  // namespace nes

#endif  // define NES_SESSION_H
