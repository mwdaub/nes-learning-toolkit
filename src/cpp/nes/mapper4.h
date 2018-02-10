#ifndef NES_MAPPER4_H
#define NES_MAPPER4_H

#include "types.h"
#include "cartridge.h"
#include "mapper.h"

namespace nes {

class Console;

// State that gets persisted when creating a save state.
class Mapper4State : virtual public MapperState {
  public:
    Mapper4State() :
        reg(0),
        registers{},
        prgMode(0),
        chrMode(0),
        prgOffsets{},
        chrOffsets{},
        reload(0),
        counter(0),
        irqEnable(false) {};

    static constexpr uint32 kRegistersSize = 8;
    static constexpr uint32 kPrgOffsetsSize = 4;
    static constexpr uint32 kChrOffsetsSize = 8;

    uint8 reg;
    uint8 registers[kRegistersSize];
    uint8 prgMode;
    uint8 chrMode;
    int32 prgOffsets[kPrgOffsetsSize];
    int32 chrOffsets[kChrOffsetsSize];
    uint8 reload;
    uint8 counter;
    bool irqEnable;

    virtual void Save(ostream& out) override;
    virtual void Load(istream& in) override;
};

// Using inheritence to be consistent with PPU. Maybe refactor someday...
class Mapper4 : virtual public Mapper, public Mapper4State {
  public:
    Mapper4(Cartridge* cartridge, Console* console) :
        Mapper4State(),
        cartridge(cartridge),
        console(console) {
      prgOffsets[0] = prgBankOffset(0);
      prgOffsets[1] = prgBankOffset(1);
      prgOffsets[2] = prgBankOffset(-2);
      prgOffsets[3] = prgBankOffset(-1);
    };
    virtual ~Mapper4() {};

    Cartridge* cartridge;
    Console* console;

    virtual uint8 Read(uint16 address) override;
    virtual void Write(uint16 address, uint8 value) override;
    virtual void Step() override;
    virtual MapperState* Copy() override { return new Mapper4State(*this); };

    void HandleScanLine();
    void writeRegister(uint16 address, uint8 value);
    void writeBankSelect(uint8 value);
    void writeBankData(uint8 value);
    void writeMirror(uint8 value);
    void writeProtect(uint8 value);
    void writeIRQLatch(uint8 value);
    void writeIRQReload(uint8 value);
    void writeIRQDisable(uint8 value);
    void writeIRQEnable(uint8 value);
    int32 prgBankOffset(int32 index);
    int32 chrBankOffset(int32 index);
    void updateOffsets();
};

}  // namespace nes

#endif  // define NES_MAPPER4_H
