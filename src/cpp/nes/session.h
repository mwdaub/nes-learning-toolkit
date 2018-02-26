#ifndef NES_SESSION_H
#define NES_SESSION_H

#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include "types.h"
#include "palette.h"

using namespace std;

using std::unique_ptr;

namespace nes {

struct Input {
  uint8 buttons1;
  uint8 buttons2;

  Input(uint8 buttons1, uint8 buttons2) :
      buttons1(buttons1),
      buttons2(buttons2) {};

  void Save(ostream& out);
};

struct Output {
  unique_ptr<Screen> screen;

  Output(Screen* s) :
      screen(new Screen(*s)) {};

  void Save(ostream& out);
};

class InputSequence {
  public:
    InputSequence() : inputs() {};

    vector<Input> inputs;

    void RecordInput(uint8 buttons1, uint8 buttons2);
    void Save(ostream& out);
};

class OutputSequence {
  public:
    OutputSequence() : outputs() {};

    vector<Output> outputs;

    void RecordOutput(Screen* screen);
    void Save(ostream& out);
};

class Console;
class State;

class Session {
  public:
    Session(Console* console);
    ~Session();

    Console* console;
    uint64 startFrame;
    unique_ptr<State> state;
    unique_ptr<InputSequence> input;
    unique_ptr<OutputSequence> output;

    void RecordFrameStart();
    void RecordFrameEnd();
    void Save(ostream& out);
    void Save(string filename);
};

}  // namespace nes

#endif  // define NES_SESSION_H
