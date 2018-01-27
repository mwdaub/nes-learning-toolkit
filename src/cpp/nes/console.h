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

using namespace std;

namespace nes {

class Console {
  public:
    Console(Cartridge* cartridge) :
        cartridge(cartridge),
        RAM{} {
      mapper = Mapper::create(this);
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
    }

    CPU* cpu;
    APU* apu;
    PPU* ppu;
    Cartridge* cartridge;
    Controller* controller1;
    Controller* controller2;
    Mapper* mapper;
    uint8 RAM[2048];

    void Reset();
    int Step();
    int StepFrame();
    void StepSeconds(float64 seconds);
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
};

}  // namespace nes

#endif  // define NES_CONSOLE_H
