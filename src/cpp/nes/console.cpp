#include "console.h"

#include <fstream>

#include "palette.h"

namespace nes {

void ConsoleState::Save(ostream& out) {
  utils::writeUint8Array(out, &RAM[0], kRAMSize);
}

void ConsoleState::Load(istream& in) {
  utils::readUint8Array(in, &RAM[0], kRAMSize);
}

void Console::Reset() {
  cpu->Reset();
  CloseSession();
}

int Console::Step() {
  int32 cpuCycles = cpu->Step();
  int32 ppuCycles = cpuCycles * 3;
  for (int32 i = 0; i < ppuCycles; i++) {
    ppu->Step();
    mapper->Step();
  }
  for (int32 i = 0; i < cpuCycles; i++) {
    apu->Step();
  }
  return cpuCycles;
}

int Console::StepFrame() {
  if (session) {
    session->RecordFrame();
  }
  int32 cpuCycles = 0;
  uint64 frame = ppu->frame;
  while (frame == ppu->frame) {
    cpuCycles += Step();
  }
  return cpuCycles;
}

void Console::Execute(InputSequence* input) {
  for (auto it = input->inputs.begin(); it != input->inputs.end(); it++) {
    SetButtons1(*it);
    StepFrame();
  }
}

Screen* Console::Buffer() {
  return ppu->front;
}

RGBA* Console::BackgroundColor() {
  return &Palette[ppu->readPalette(0)%64];
}

void Console::SetButtons1(uint8 buttons) {
  controller1->SetButtons(buttons);
}

void Console::SetButtons2(uint8 buttons) {
  controller2->SetButtons(buttons);
}

void Console::SetAudioChannel() {
  // TODO: implement audio
  // apu->channel = channel;
}

void Console::SetAudioSampleRate(float64 sampleRate) {
  apu->SetSampleRate(float32(sampleRate));
}

void Console::SaveState(string filename) {
  fstream fs;
  fs.open(filename.c_str(), fstream::out);
  SaveState(fs);
  fs.close();
}

void Console::SaveState(ostream& out) {
  this->Save(out);
  cpu->Save(out);
  apu->Save(out);
  apu->pulse1.Save(out);
  apu->pulse2.Save(out);
  apu->triangle.Save(out);
  apu->noise.Save(out);
  apu->dmc.Save(out);
  ppu->Save(out);
  cartridge->Save(out);
  mapper->Save(out);
}

void Console::LoadState(string filename) {
  fstream fs;
  fs.open(filename.c_str(), fstream::in);
  LoadState(fs);
  fs.close();
}

void Console::LoadState(istream& in) {
  this->Load(in);
  cpu->Load(in);
  apu->Load(in);
  apu->pulse1.Load(in);
  apu->pulse2.Load(in);
  apu->triangle.Load(in);
  apu->noise.Load(in);
  apu->dmc.Load(in);
  ppu->Load(in);
  cartridge->Load(in);
  mapper->Load(in);
}

void Console::NewSession() {
  delete session;
  session = new Session(this);
}

void Console::CloseSession() {
  if (session) {
    delete session;
    session = NULL;
  }
}

}  // namespace nes;
