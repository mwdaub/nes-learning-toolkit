#ifndef NES_STATE_H
#define NES_STATE_H

#include <iostream>
#include <memory>
#include <vector>

#include "types.h"
#include "console.h"

using namespace std;

using std::shared_ptr;

namespace nes {

class State {
  public:
    State(Console* console) :
       consoleState(*console),
       cpuState(*console->cpu),
       apuState(*console->apu),
       pulseState1(console->apu->pulse1),
       pulseState2(console->apu->pulse2),
       triangleState(console->apu->triangle),
       noiseState(console->apu->noise),
       dmcState(console->apu->dmc),
       ppuState(*console->ppu),
       cartridgeState(*console->cartridge),
       mapperState(Mapper::Copy(console->mapper.get())) {};

    ConsoleState consoleState;
    CPUState cpuState;
    APUState apuState;
    PulseState pulseState1;
    PulseState pulseState2;
    TriangleState triangleState;
    NoiseState noiseState;
    DMCState dmcState;
    PPUState ppuState;
    CartridgeState cartridgeState;
    unique_ptr<MapperState> mapperState;

    void Save(ostream& out);
};

}  // namespace nes

#endif  // define NES_STATE_H
