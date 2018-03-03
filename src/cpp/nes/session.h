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

  Input() : buttons1(0), buttons2(0) {};
  Input(uint8 buttons1, uint8 buttons2) :
      buttons1(buttons1),
      buttons2(buttons2) {};

  void Save(ostream& out);
  void Load(istream& in);
};

struct Output {
  unique_ptr<Screen> screen;

  Output() : screen() {};
  Output(Screen* s) :
      screen(new Screen(*s)) {};

  void Save(ostream& out);
  void Load(istream& in);
};

class InputSequence {
  public:
    InputSequence() : inputs() {};

    vector<Input> inputs;

    void RecordInput(uint8 buttons1, uint8 buttons2);
    void Save(ostream& out);
    void Load(istream& in);
};

class OutputSequence {
  public:
    OutputSequence() : outputs() {};

    vector<Output> outputs;

    void RecordOutput(Screen* screen);
    void Save(ostream& out);
    void Load(istream& in);
};

class Console;
class State;

enum RecordingMode {
  INPUT = 1,
  OUTPUT = 2,
  BOTH = INPUT | OUTPUT
};

class Session {
  public:
    Session(Console* console, RecordingMode mode);
    ~Session();

    Console* console;
    RecordingMode mode;
    uint64 startFrame;
    unique_ptr<State> state;
    unique_ptr<InputSequence> input;
    unique_ptr<OutputSequence> output;

    void RecordFrameStart();
    void RecordFrameEnd();
    void Save(ostream& out);
    void Save(string filename);
    void Load(istream& in);
    void Load(string filename);
};

}  // namespace nes

#endif  // define NES_SESSION_H
