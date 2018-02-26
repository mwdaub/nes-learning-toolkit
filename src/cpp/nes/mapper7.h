#ifndef NES_MAPPER7_H
#define NES_MAPPER7_H

#include "types.h"
#include "cartridge.h"
#include "mapper.h"

using std::make_unique;
using std::shared_ptr;
using std::unique_ptr;

namespace nes {

// State that gets persisted when creating a save state.
class Mapper7State : virtual public MapperState {
  public:
    Mapper7State() :
        prgBank(0) {};

    int32 prgBank;

    virtual void Save(ostream& out) override;
    virtual void Load(istream& in) override;
};

// Using inheritence to be consistent with PPU. Maybe refactor someday...
class Mapper7 : virtual public Mapper, public Mapper7State {
  public:
    Mapper7(shared_ptr<Cartridge> cartridge) :
        Mapper7State(),
        cartridge(cartridge) {};
    virtual ~Mapper7() {};

    shared_ptr<Cartridge> cartridge;

    virtual uint8 Read(uint16 address) override;
    virtual void Write(uint16 address, uint8 value) override;
    virtual void Step() override {};
    virtual unique_ptr<MapperState> Copy() override { return make_unique<Mapper7State>(*this); };
};

}  // namespace nes

#endif  // define NES_MAPPER7_H
