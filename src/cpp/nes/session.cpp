#include "session.h"

#include "console.h"
#include "state.h"

namespace nes {

void InputSequence::RecordInput(uint8 buttons1, uint8 buttons2) {
  Input in(buttons1, buttons2);
  inputs.push_back(in);
}

Session::Session(Console* console) :
    console(console),
    startFrame(console->ppu->frame),
    state(NULL),
    input() {
  if (startFrame != 0) {
    state = new State(console);
  }
}

Session::~Session() {
  if (state) {
    delete state;
    state = NULL;
  }
  if (input) {
    delete input;
    input = NULL;
  }
}

void Session::RecordFrame() {
  uint8 buttons1 = console->controller1->buttons;
  uint8 buttons2 = console->controller2->buttons;
  input->RecordInput(buttons1, buttons2);
}

}  // namespace nes
