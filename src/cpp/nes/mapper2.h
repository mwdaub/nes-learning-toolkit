#ifndef NES_MAPPER2_H
#define NES_MAPPER2_H

#include "types.h"
#include "cartridge.h"
#include "mapper.h"

namespace nes {

// State that gets persisted when creating a save state.
class Mapper2State : virtual public MapperState {
  public:
    Mapper2State() :
        prgBanks(0),
        prgBank1(0),
        prgBank2(0) {};

    int32 prgBanks;
    int32 prgBank1;
    int32 prgBank2;

    virtual void Save(ostream& out) override;
    virtual void Load(istream& in) override;
};

// Using inheritence to be consistent with PPU. Maybe refactor someday...
class Mapper2 : virtual public Mapper, public Mapper2State {
  public:
    Mapper2(Cartridge* cartridge) :
        Mapper2State(),
        cartridge(cartridge) {
      prgBanks = cartridge->prgLength / 0x4000;
      prgBank2 = prgBanks - 1;
    };
    virtual ~Mapper2() {};

    Cartridge* cartridge;

    virtual uint8 Read(uint16 address) override;
    virtual void Write(uint16 address, uint8 value) override;
    virtual void Step() override {};
    virtual MapperState* Copy() override { return new Mapper2State(*this); };
};

}  // namespace nes

#endif  // define NES_MAPPER2_H
