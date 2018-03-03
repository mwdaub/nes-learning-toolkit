#ifndef NES_MAPPER225_H
#define NES_MAPPER225_H

#include "types.h"
#include "cartridge.h"
#include "mapper.h"

using std::make_unique;
using std::shared_ptr;
using std::unique_ptr;

namespace nes {

// State that gets persisted when creating a save state.
class Mapper225State : virtual public MapperState {
  public:
    Mapper225State() :
        chrBank(0),
        prgBank1(0),
        prgBank2(0) {};

    int32 chrBank;
    int32 prgBank1;
    int32 prgBank2;

    virtual void Save(ostream& out) override;
    virtual void Load(istream& in) override;
    virtual void Load(MapperState* o) {
      o->CopyTo(this);
    }
    virtual void CopyTo(Mapper225State* o) {
      *o = *this;
    }
};

// Using inheritence to be consistent with PPU. Maybe refactor someday...
class Mapper225 : virtual public Mapper, public Mapper225State {
  public:
    Mapper225(shared_ptr<Cartridge> cartridge) :
        Mapper225State(),
        cartridge(cartridge) {
      prgBank2 = cartridge->PRG.size() / 0x4000 - 1;
    };
    virtual ~Mapper225() {};

    shared_ptr<Cartridge> cartridge;

    virtual uint8 Read(uint16 address) override;
    virtual void Write(uint16 address, uint8 value) override;
    virtual void Step() override {};
    virtual unique_ptr<MapperState> Copy() override { return make_unique<Mapper225State>(*this); };
};

}  // namespace nes

#endif  // define NES_MAPPER225_H
