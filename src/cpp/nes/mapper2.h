#ifndef NES_MAPPER2_H
#define NES_MAPPER2_H

#include "types.h"
#include "cartridge.h"
#include "mapper.h"

namespace nes {

class Mapper2 : public Mapper {
  public:
    Mapper2(Cartridge* cartridge) :
        cartridge(cartridge),
        prgBanks(cartridge->prgLength / 0x4000),
        prgBank1(0),
        prgBank2(prgBanks - 1) {};
    virtual ~Mapper2() {};

    Cartridge* cartridge;
    int32 prgBanks;
    int32 prgBank1;
    int32 prgBank2;

    virtual uint8 Read(uint16 address) override;
    virtual void Write(uint16 address, uint8 value) override;
    virtual void Step() override {};
    virtual void Save(ostream& out) override;
    virtual void Load(istream& in) override;
};

}  // namespace nes

#endif  // define NES_MAPPER2_H
