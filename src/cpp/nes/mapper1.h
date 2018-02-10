#ifndef NES_MAPPER1_H
#define NES_MAPPER1_H

#include "types.h"
#include "cartridge.h"
#include "mapper.h"

using namespace std;

namespace nes {

// State that gets persisted when creating a save state.
class Mapper1State : virtual public MapperState {
  public:
    Mapper1State() :
        shiftRegister(0x10),
        control(0),
        prgMode(0),
        chrMode(0),
        prgBank(0),
        chrBank0(0),
        chrBank1(0),
        prgOffsets{},
        chrOffsets{} {};

    static constexpr uint32 kPrgOffsetsSize = 2;
    static constexpr uint32 kChrOffsetsSize = 2;

    uint8 shiftRegister;
    uint8 control;
    uint8 prgMode;
    uint8 chrMode;
    uint8 prgBank;
    uint8 chrBank0;
    uint8 chrBank1;
    int32 prgOffsets[kPrgOffsetsSize];
    int32 chrOffsets[kChrOffsetsSize];

    virtual void Save(ostream& out) override;
    virtual void Load(istream& in) override;
};

// Using inheritence to be consistent with PPU. Maybe refactor someday...
class Mapper1 : virtual public Mapper, public Mapper1State {
  public:
    Mapper1(Cartridge* cartridge) :
        Mapper1State(),
        cartridge(cartridge) {
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
    virtual MapperState* Copy() override { return new Mapper1State(*this); };

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
