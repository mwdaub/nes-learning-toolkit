#ifndef NES_MAPPER225_H
#define NES_MAPPER225_H

#include "types.h"
#include "cartridge.h"
#include "mapper.h"

namespace nes {

class Mapper225 : public Mapper {
  public:
    Mapper225(Cartridge* cartridge) :
        cartridge(cartridge),
        chrBank(0),
        prgBank1(0),
        prgBank2(cartridge->prgLength / 0x4000 - 1) {};
    virtual ~Mapper225() {};

    Cartridge* cartridge;
    int32 chrBank;
    int32 prgBank1;
    int32 prgBank2;

    virtual uint8 Read(uint16 address) override;
    virtual void Write(uint16 address, uint8 value) override;
    virtual void Step() override {};
    virtual void Save(ostream& out) override;
    virtual void Load(istream& in) override;
};

}  // namespace nes

#endif  // define NES_MAPPER225_H
