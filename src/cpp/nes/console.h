#ifndef NES_CONSOLE_H
#define NES_CONSOLE_H

#include <iostream>

#include "types.h"
#include "apu.h"
#include "cpu.h"
#include "ppu.h"
#include "cartridge.h"
#include "controller.h"
#include "mapper.h"
#include "session.h"

using namespace std;

namespace nes {

// State that gets persisted when creating a save state.
class ConsoleState {
  public:
    ConsoleState() : RAM{} {};

    static constexpr uint32 kRAMSize = 2048;

    uint8 RAM[kRAMSize];

    void Save(ostream& out);
    void Load(istream& in);
};

// Using inheritence to be consistent with PPU. Maybe refactor someday...
class Console : public ConsoleState {
  public:
    Console(Cartridge* cartridge) :
        cartridge(cartridge),
        state(),
        session(NULL) {
      mapper = Mapper::Create(this);
      controller1 = new Controller();
      controller2 = new Controller();
      cpu = new CPU(this);
      apu = new APU(this, cpu);
      ppu = new PPU(this);
    };

    ~Console() {
      delete cpu;
      cpu = NULL;
      delete apu;
      apu = NULL;
      delete ppu;
      ppu = NULL;
      delete cartridge;
      cartridge = NULL;
      delete controller1;
      controller1 = NULL;
      delete controller2;
      controller2 = NULL;
      delete mapper;
      mapper = NULL;
      CloseSession();
    };

    CPU* cpu;
    APU* apu;
    PPU* ppu;
    Cartridge* cartridge;
    Controller* controller1;
    Controller* controller2;
    Mapper* mapper;
    ConsoleState state;
    Session* session;

    uint32 Size();
    void Reset();
    int Step();
    int StepFrame();
    void Execute(InputSequence* seq);
    Screen* Buffer();
    RGBA* BackgroundColor();
    void SetButtons1(uint8 buttons);
    void SetButtons2(uint8 buttons);
    void SetAudioChannel();
    void SetAudioSampleRate(float64 sampleRate);
    void SaveState(string filename);
    void SaveState(ostream& out);
    void LoadState(string filename);
    void LoadState(istream& in);
    void NewSession();
    void CloseSession();
};

}  // namespace nes

#endif  // define NES_CONSOLE_H
