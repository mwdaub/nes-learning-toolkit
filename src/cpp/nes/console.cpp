#include "console.h"

#include <fstream>

#include "palette.h"
#include "state.h"

namespace nes {

void ConsoleState::Save(ostream& out) {
  utils::writeUint8Array(out, RAM);
}

void ConsoleState::Load(istream& in) {
  utils::readUint8Array(in, RAM);
}

void Console::Reset() {
  session.release();
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
  if (session) {
    session->RecordFrameStart();
  }
  apu->channel->Reset();
  int32 cpuCycles = 0;
  uint64 frame = ppu->frame;
  while (frame == ppu->frame) {
    cpuCycles += Step();
  }
  if (session) {
    session->RecordFrameEnd();
  }
  return cpuCycles;
}

void Console::Replay(InputSequence& is, ostream& vout, ostream& aout) {
  for (auto& in : is.inputs) {
    SetButtons1(in.buttons1);
    SetButtons2(in.buttons2);
    StepFrame();
    ppu->front->SaveValues(vout);
    apu->channel->Save(aout);
  }
}

void Console::Replay(string& input_file, string& video_file, string& audio_file) {
  Session session(this, RecordingMode::INPUT);
  fstream input_fs;
  input_fs.open(input_file.c_str(), fstream::in);
  session.Load(input_fs);
  input_fs.close();
  fstream video_fs;
  fstream audio_fs;
  video_fs.open(video_file.c_str(), fstream::out);
  audio_fs.open(audio_file.c_str(), fstream::out);
  Replay(*session.input, video_fs, audio_fs);
  video_fs.close();
  audio_fs.close();
}

void Console::PixelIndexes(uint8* idx) {
  ppu->front->GetPixelIndexes(idx);
}

void Console::PixelValues(uint8* vals) {
  ppu->front->GetPixelValues(vals);
}

const RGB* Console::BackgroundColor() {
  return &Palette[ppu->readPalette(0)%64];
}

void Console::SetButtons1(uint8 buttons) {
  controller1->SetButtons(buttons);
}

void Console::SetButtons2(uint8 buttons) {
  controller2->SetButtons(buttons);
}

void Console::SetAudioSamplesPerFrame(uint32 samplesPerFrame) {
  apu->SetSamplesPerFrame(samplesPerFrame);
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

void Console::LoadState(State& state) {
  ConsoleState* consoleState = this;
  *consoleState = state.consoleState;
  CPUState* cpuState = cpu.get();
  *cpuState = state.cpuState;
  APUState* apuState = apu.get();
  *apuState = state.apuState;
  PulseState* pulseState = &apu->pulse1;
  *pulseState = state.pulseState1;
  pulseState = &apu->pulse2;
  *pulseState = state.pulseState2;
  TriangleState* triangleState = &apu->triangle;
  *triangleState = state.triangleState;
  NoiseState* noiseState = &apu->noise;
  *noiseState = state.noiseState;
  DMCState* dmcState = &apu->dmc;
  *dmcState = state.dmcState;
  PPUState* ppuState = ppu.get();
  *ppuState = state.ppuState;
  CartridgeState* cartridgeState = cartridge.get();
  *cartridgeState = state.cartridgeState;
  MapperState* mapperState = mapper.get();
  mapperState->Load(state.mapperState.get());
}

void Console::NewSession(RecordingMode mode) {
  session.reset(new Session(this, mode));
}

void Console::LoadSession(istream& in, RecordingMode mode) {
  session.reset(new Session(this, mode));
  session->Load(in);
}

void Console::LoadSession(const string& filename, RecordingMode mode) {
  fstream fs;
  fs.open(filename.c_str(), fstream::in);
  LoadSession(fs);
  fs.close();
}

}  // namespace nes;
