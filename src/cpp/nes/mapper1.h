#ifndef NES_MAPPER1_H
#define NES_MAPPER1_H

#include "types.h"
#include "cartridge.h"
#include "mapper.h"

using namespace std;

namespace nes {

class Mapper1 : public Mapper {
  public:
    Mapper1(Cartridge* cartridge) :
        cartridge(cartridge),
        shiftRegister(0x10),
        control(0),
        prgMode(0),
        chrMode(0),
        prgBank(0),
        chrBank0(0),
        chrBank1(0),
        prgOffsets{},
        chrOffsets{} {
	  prgOffsets[1] = prgBankOffset(-1);
    };
    virtual ~Mapper1() {};

    Cartridge* cartridge;
    uint8 shiftRegister;
    uint8 control;
    uint8 prgMode;
    uint8 chrMode;
    uint8 prgBank;
    uint8 chrBank0;
    uint8 chrBank1;
    int32 prgOffsets[2];
    int32 chrOffsets[2];

    virtual uint8 Read(uint16 address) override;
    virtual void Write(uint16 address, uint8 value) override;
    virtual void Step() override {};
    virtual void Save(ostream& out) override;
    virtual void Load(istream& in) override;

  private:
    void loadRegister(uint16 address, uint8 value);
    void writeRegister(uint16 address, uint8 value);
    void writeControl(uint8 value);
    void writeCHRBank0(uint8 value);
    void writeCHRBank1(uint8 value);
    void writePRGBank(uint8 value);
    int32 prgBankOffset(int32 index);
    int32 chrBankOffset(int32 index);
    void updateOffsets();
};

}  // namespace nes

#endif  // define NES_MAPPER1_H
