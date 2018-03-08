#ifndef NES_CARTRIDGE_H
#define NES_CARTRIDGE_H

#include <array>
#include <iostream>
#include <vector>

#include "types.h"

using namespace std;

using std::array;

namespace nes {

class Console;

// State that gets persisted when creating a save state.
class CartridgeState {
  public:
    CartridgeState() {};
    CartridgeState(vector<uint8>& prg, vector<uint8>& chr, uint8 mapper, uint8 mirror) :
        PRG(prg),
        CHR(chr),
        SRAM{},
        mapper(mapper),
        mirror(mirror) {};

    static constexpr uint32 kSRAMSize = 0x2000;

    vector<uint8> PRG;         // PRG-ROM banks
    vector<uint8> CHR;         // CHR-ROM banks
    array<uint8, kSRAMSize> SRAM; // Save RAM
    uint8 mapper;       // mapper type
    uint8 mirror;       // mirroring mode

    void Save(ostream& out);
    void Load(istream& in);
};

// Using inheritence to be consistent with PPU. Maybe refactor someday...
class Cartridge : public CartridgeState {
  public:
    Cartridge(vector<uint8>& prg, vector<uint8>& chr, uint8 mapper, uint8 mirror, uint8 battery) :
        CartridgeState(prg, chr, mapper, mirror),
        battery(battery) {};

    uint8 battery;      // battery present
};

}  // namespace nes

#endif  // define NES_CARTRIDGE_H
