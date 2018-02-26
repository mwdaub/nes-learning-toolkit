#include "session.h"

#include <fstream>

#include "console.h"
#include "state.h"
#include "utils.h"

namespace nes {

void Input::Save(ostream& out) {
  utils::writeUint8(out, buttons1);
  utils::writeUint8(out, buttons2);
}

void Output::Save(ostream& out) {
  screen->Save(out);
}

void InputSequence::RecordInput(uint8 buttons1, uint8 buttons2) {
  Input in(buttons1, buttons2);
  inputs.push_back(in);
}

void InputSequence::Save(ostream& out) {
  utils::writeUint32(out, inputs.size());
  for (auto& input : inputs) {
    input.Save(out);
  }
}

void OutputSequence::RecordOutput(Screen* screen) {
  Output out(screen);
  outputs.push_back(std::move(out));
}

void OutputSequence::Save(ostream& out) {
  utils::writeUint32(out, outputs.size());
  for (auto& output : outputs) {
    output.Save(out);
  }
}

Session::Session(Console* console) :
    console(console),
    startFrame(console->ppu->frame),
    state(),
    input(new InputSequence()),
    output(new OutputSequence()) {
  if (startFrame != 0) {
    state.reset(new State(console));
  }
}

Session::~Session() {}

void Session::RecordFrameStart() {
  if (input) {
    uint8 buttons1 = console->controller1->buttons;
    uint8 buttons2 = console->controller2->buttons;
    input->RecordInput(buttons1, buttons2);
  }
}

void Session::RecordFrameEnd() {
  if (output) {
    Screen* screen = console->ppu->front.get();
    output->RecordOutput(screen);
  }
}

void Session::Save(ostream& out) {
  utils::writeUint64(out, startFrame);
  if (state) {
    state->Save(out);
  }
  input->Save(out);
  output->Save(out);
}

void Session::Save(string filename) {
  fstream fs;
  fs.open(filename.c_str(), fstream::out);
  Save(fs);
  fs.close();
}

}  // namespace nes
