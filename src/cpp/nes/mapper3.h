#ifndef NES_MAPPER3_H
#define NES_MAPPER3_H

#include "types.h"
#include "cartridge.h"
#include "mapper.h"

namespace nes {

// State that gets persisted when creating a save state.
class Mapper3State : virtual public MapperState {
  public:
    Mapper3State() :
        chrBank(0),
        prgBank1(0),
        prgBank2(0) {};

    int32 chrBank;
    int32 prgBank1;
    int32 prgBank2;

    virtual void Save(ostream& out) override;
    virtual void Load(istream& in) override;
};

// Using inheritence to be consistent with PPU. Maybe refactor someday...
class Mapper3 : virtual public Mapper, public Mapper3State {
  public:
    Mapper3(Cartridge* cartridge) :
        Mapper3State(),
        cartridge(cartridge) {
      prgBank2 = cartridge->prgLength / 0x4000 - 1;
    }
    virtual ~Mapper3() {};

    Cartridge* cartridge;

    virtual uint8 Read(uint16 address) override;
    virtual void Write(uint16 address, uint8 value) override;
    virtual void Step() override {};
    virtual MapperState* Copy() override { return new Mapper3State(*this); };
};

}  // namespace nes

#endif  // define NES_MAPPER3_H
