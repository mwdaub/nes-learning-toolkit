#ifndef NES_CARTRIDGE_H
#define NES_CARTRIDGE_H

#include <iostream>

#include "types.h"

using namespace std;

namespace nes {

class Console;

// State that gets persisted when creating a save state.
class CartridgeState {
  public:
    CartridgeState(uint8 mirror) : SRAM{}, mirror(mirror) {};

    static constexpr uint32 kSRAMSize = 0x2000;

    uint8 SRAM[kSRAMSize]; // Save RAM
    uint8 mirror;       // mirroring mode

    void Save(ostream& out);
    void Load(istream& in);
};

// Using inheritence to be consistent with PPU. Maybe refactor someday...
class Cartridge : public CartridgeState {
  public:
    Cartridge(uint8* prg, uint32 prgLength, uint8* chr, uint32 chrLength, uint8 mapper, uint8 mirror, uint8 battery) :
        CartridgeState(mirror),
        PRG(prg),
        prgLength(prgLength),
        CHR(chr),
        chrLength(chrLength),
        mapper(mapper),
        battery(battery) {};

    uint8* PRG;         // PRG-ROM banks
    uint32 prgLength;
    uint8* CHR;         // CHR-ROM banks
    uint32 chrLength;
    uint8 mapper;       // mapper type
    uint8 battery;      // battery present
};

}  // namespace nes

#endif  // define NES_CARTRIDGE_H
