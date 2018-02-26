#ifndef NES_CONSOLE_H
#define NES_CONSOLE_H

#include <array>
#include <iostream>
#include <memory>

#include "types.h"
#include "apu.h"
#include "cpu.h"
#include "ppu.h"
#include "cartridge.h"
#include "controller.h"
#include "mapper.h"
#include "session.h"

using std::array;
using std::istream;
using std::make_unique;
using std::ostream;
using std::string;
using std::shared_ptr;
using std::unique_ptr;

namespace nes {

// State that gets persisted when creating a save state.
class ConsoleState {
  public:
    ConsoleState() : RAM{} {};

    static constexpr uint32 kRAMSize = 2048;

    array<uint8, kRAMSize> RAM;

    void Save(ostream& out);
    void Load(istream& in);
};

// Using inheritence to be consistent with PPU. Maybe refactor someday...
class Console : public ConsoleState {
  public:
    Console(Cartridge* cartridge) :
        ConsoleState(),
        cartridge(cartridge),
        session() {
      mapper = Mapper::Create(this);
      controller1 = make_unique<Controller>();
      controller2 = make_unique<Controller>();
      cpu = make_unique<CPU>(this);
      apu = make_unique<APU>(this);
      ppu = make_unique<PPU>(this);
    };

    unique_ptr<CPU> cpu;
    unique_ptr<APU> apu;
    unique_ptr<PPU> ppu;
    shared_ptr<Cartridge> cartridge;
    unique_ptr<Controller> controller1;
    unique_ptr<Controller> controller2;
    unique_ptr<Mapper> mapper;
    unique_ptr<Session> session;

    uint32 Size();
    void Reset();
    int Step();
    int StepFrame();
    void Replay(shared_ptr<InputSequence> seq, ostream& vout, ostream& aout);
    void Replay(shared_ptr<InputSequence> seq, string video_file, string audio_file);
    void PixelIndexes(uint8* idx);
    void PixelValues(uint8* vals);
    const RGB* BackgroundColor();
    void SetButtons1(uint8 buttons);
    void SetButtons2(uint8 buttons);
    void SetAudioSamplesPerFrame(uint32 samplesPerFrame);
    void SaveState(string filename);
    void SaveState(ostream& out);
    void LoadState(string filename);
    void LoadState(istream& in);
    void NewSession();
};

}  // namespace nes

#endif  // define NES_CONSOLE_H
