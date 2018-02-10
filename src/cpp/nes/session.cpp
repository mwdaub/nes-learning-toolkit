#include "session.h"

#include "console.h"
#include "state.h"

namespace nes {

InputSequence::InputSequence(Console* console) :
    startFrame(console->ppu->frame),
    state(NULL),
    inputs() {
  if (startFrame != 0) {
    state = new State(console);
  }
}

Session::Session(Console* console) :
    console(console),
    input(new InputSequence(console)) {};

void Session::RecordFrame() {
  uint8 buttons = console->controller1->buttons;
  input->RecordInput(buttons);
}

}  // namespace nes
