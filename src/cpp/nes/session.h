#ifndef NES_SESSION_H
#define NES_SESSION_H

#include <vector>

#include "types.h"

using namespace std;

namespace nes {

struct Input {
  uint8 buttons1;
  uint8 buttons2;

  Input(uint8 buttons1, uint8 buttons2) :
      buttons1(buttons1),
      buttons2(buttons2) {};
};

class State;
class Console;

class InputSequence {
  public:
    InputSequence() : inputs() {}

    vector<Input> inputs;

    void RecordInput(uint8 buttons1, uint8 buttons2);
};

class Console;

class Session {
  public:
    Session(Console* console);
    ~Session();

    Console* console;
    uint64 startFrame;
    State* state;
    InputSequence* input;

    void RecordFrame();
};

}  // namespace nes

#endif  // define NES_SESSION_H
