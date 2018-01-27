#ifndef NES_MAPPER7_H
#define NES_MAPPER7_H

#include "types.h"
#include "cartridge.h"
#include "mapper.h"

namespace nes {

class Mapper7 : public Mapper {
  public:
    Mapper7(Cartridge* cartridge) :
        cartridge(cartridge),
        prgBank(0) {};
    virtual ~Mapper7() {};

    Cartridge* cartridge;
    int32 prgBank;

    virtual uint8 Read(uint16 address) override;
    virtual void Write(uint16 address, uint8 value) override;
    virtual void Step() override {};
    virtual void Save(ostream& out) override;
    virtual void Load(istream& in) override;
};

}  // namespace nes

#endif  // define NES_MAPPER7_H
