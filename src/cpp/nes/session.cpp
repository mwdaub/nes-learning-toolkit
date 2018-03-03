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

void Input::Load(istream& in) {
  buttons1 = utils::readUint8(in);
  buttons2 = utils::readUint8(in);
}

void Output::Save(ostream& out) {
  screen->Save(out);
}

void Output::Load(istream& in) {
  screen.reset(new Screen());
  screen->Load(in);
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

void InputSequence::Load(istream& in) {
  auto size = utils::readUint32(in);
  inputs.clear();
  inputs.resize(size);
  for (auto& input : inputs) {
    input.Load(in);
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

void OutputSequence::Load(istream& in) {
  auto size = utils::readUint32(in);
  outputs.clear();
  outputs.resize(size);
  for (auto& output : outputs) {
    output.Load(in);
  }
}

Session::Session(Console* console, RecordingMode mode) :
    console(console),
    mode(mode),
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
  if (mode & RecordingMode::INPUT) {
    uint8 buttons1 = console->controller1->buttons;
    uint8 buttons2 = console->controller2->buttons;
    input->RecordInput(buttons1, buttons2);
  }
}

void Session::RecordFrameEnd() {
  if (mode & RecordingMode::OUTPUT) {
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

void Session::Load(istream& in) {
  startFrame = utils::readUint64(in);
  if (startFrame) {
    state.reset(new State());
    state->Load(in);
  }
  input->Load(in);
  output->Load(in);
}

void Session::Load(string filename) {
  fstream fs;
  fs.open(filename.c_str(), fstream::in);
  Load(fs);
  fs.close();
}

}  // namespace nes
