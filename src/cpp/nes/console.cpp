#include "console.h"

#include <fstream>

#include "palette.h"

namespace nes {

void Console::Reset() {
  cpu->Reset();
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
  int32 cpuCycles = 0;
  uint64 frame = ppu->frame;
  while (frame == ppu->frame) {
    cpuCycles += Step();
  }
  return cpuCycles;
}

void Console::StepSeconds(float64 seconds) {
  int32 cycles = int(CPU::Frequency * seconds);
  while (cycles > 0) {
    cycles -= Step();
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
  utils::write(out, reinterpret_cast<char*>(&RAM[0]), sizeof(RAM));
  cpu->Save(out);
  apu->Save(out);
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
  utils::read(in, reinterpret_cast<char*>(&RAM[0]), sizeof(RAM));
  cpu->Load(in);
  apu->Load(in);
  ppu->Load(in);
  cartridge->Load(in);
  mapper->Load(in);
}

}  // namespace nes;
