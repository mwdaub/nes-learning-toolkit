#ifndef NES_MAPPER4_H
#define NES_MAPPER4_H

#include <array>

#include "types.h"
#include "cartridge.h"
#include "mapper.h"

using std::array;
using std::make_unique;
using std::shared_ptr;
using std::unique_ptr;

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
    array<uint8, kRegistersSize> registers;
    uint8 prgMode;
    uint8 chrMode;
    array<int32, kPrgOffsetsSize> prgOffsets;
    array<int32, kChrOffsetsSize> chrOffsets;
    uint8 reload;
    uint8 counter;
    bool irqEnable;

    virtual void Save(ostream& out) override;
    virtual void Load(istream& in) override;
};

// Using inheritence to be consistent with PPU. Maybe refactor someday...
class Mapper4 : virtual public Mapper, public Mapper4State {
  public:
    Mapper4(shared_ptr<Cartridge> cartridge, Console* console) :
        Mapper4State(),
        cartridge(cartridge),
        console(console) {
      prgOffsets[0] = prgBankOffset(0);
      prgOffsets[1] = prgBankOffset(1);
      prgOffsets[2] = prgBankOffset(-2);
      prgOffsets[3] = prgBankOffset(-1);
    };
    virtual ~Mapper4() {};

    shared_ptr<Cartridge> cartridge;
    Console* console;

    virtual uint8 Read(uint16 address) override;
    virtual void Write(uint16 address, uint8 value) override;
    virtual void Step() override;
    virtual unique_ptr<MapperState> Copy() override { return make_unique<Mapper4State>(*this); };

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
