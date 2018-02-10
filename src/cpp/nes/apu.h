#ifndef NES_APU_H
#define NES_APU_H

#include <iostream>
#include <limits>

#include "types.h"
#include "cpu.h"
#include "filter.h"

using namespace std;

namespace nes {

class Console;

// State that gets persisted when creating a save state.
class PulseState {
  public:
    PulseState() :
        enabled(false),
        channel(0),
        lengthEnabled(false),
        lengthValue(0),
        timerPeriod(0),
        timerValue(0),
        dutyMode(0),
        dutyValue(0),
        sweepReload(false),
        sweepEnabled(false),
        sweepNegate(false),
        sweepShift(0),
        sweepPeriod(0),
        sweepValue(0),
        envelopeEnabled(false),
        envelopeLoop(false),
        envelopeStart(false),
        envelopePeriod(0),
        envelopeValue(0),
        envelopeVolume(0),
        constantVolume(0) {};

    bool enabled;
    uint8 channel;
    bool lengthEnabled;
    uint8 lengthValue;
    uint16 timerPeriod;
    uint16 timerValue;
    uint8 dutyMode;
    uint8 dutyValue;
    bool sweepReload;
    bool sweepEnabled;
    bool sweepNegate;
    uint8 sweepShift;
    uint8 sweepPeriod;
    uint8 sweepValue;
    bool envelopeEnabled;
    bool envelopeLoop;
    bool envelopeStart;
    uint8 envelopePeriod;
    uint8 envelopeValue;
    uint8 envelopeVolume;
    uint8 constantVolume;

    void Save(ostream& out);
    void Load(istream& in);
};

// Using inheritence to be consistent with PPU. Maybe refactor someday...
class Pulse : public PulseState {
  public:
    Pulse() : PulseState() {};

    void writeControl(uint8 value);
    void writeSweep(uint8 value);
    void writeTimerLow(uint8 value);
    void writeTimerHigh(uint8 value);
    void stepTimer();
    void stepEnvelope();
    void stepSweep();
    void stepLength();
    void sweep();
    uint8 output();
};

// State that gets persisted when creating a save state.
class TriangleState {
  public:
    TriangleState() :
        enabled(false),
        lengthEnabled(false),
        lengthValue(0),
        timerPeriod(0),
        timerValue(0),
        dutyValue(0),
        counterPeriod(0),
        counterValue(0),
        counterReload(false) {};

    bool enabled;
    bool lengthEnabled;
    uint8 lengthValue;
    uint16 timerPeriod;
    uint16 timerValue;
    uint8 dutyValue;
    uint8 counterPeriod;
    uint8 counterValue;
    bool counterReload;

    void Save(ostream& out);
    void Load(istream& in);
};

// Using inheritence to be consistent with PPU. Maybe refactor someday...
class Triangle : public TriangleState {
  public:
    Triangle() : TriangleState() {};

    void writeControl(uint8 value);
    void writeTimerLow(uint8 value);
    void writeTimerHigh(uint8 value);
    void stepTimer();
    void stepLength();
    void stepCounter();
    uint8 output();
};

// State that gets persisted when creating a save state.
class NoiseState {
  public:
    NoiseState() :
        enabled(false),
        mode(false),
        shiftRegister(0),
        lengthEnabled(false),
        lengthValue(0),
        timerPeriod(0),
        timerValue(0),
        envelopeEnabled(false),
        envelopeLoop(false),
        envelopeStart(false),
        envelopePeriod(0),
        envelopeValue(0),
        envelopeVolume(0),
        constantVolume(0) {};

    bool enabled;
    bool mode;
    uint16 shiftRegister;
    bool lengthEnabled;
    uint8 lengthValue;
    uint16 timerPeriod;
    uint16 timerValue;
    bool envelopeEnabled;
    bool envelopeLoop;
    bool envelopeStart;
    uint8 envelopePeriod;
    uint8 envelopeValue;
    uint8 envelopeVolume;
    uint8 constantVolume;

    void Save(ostream& out);
    void Load(istream& in);
};

// Using inheritence to be consistent with PPU. Maybe refactor someday...
class Noise : public NoiseState {
  public:
    Noise() : NoiseState() {};

    void writeControl(uint8 value);
    void writePeriod(uint8 value);
    void writeLength(uint8 value);
    void stepTimer();
    void stepEnvelope();
    void stepLength();
    uint8 output();
};

// State that gets persisted when creating a save state.
class DMCState {
  public:
    DMCState() :
        enabled(false),
        value(0),
        sampleAddress(0),
        sampleLength(0),
        currentAddress(0),
        currentLength(0),
        shiftRegister(0),
        bitCount(0),
        tickPeriod(0),
        tickValue(0),
        loop(false),
        irq(false) {};

    bool enabled;
    uint8 value;
    uint16 sampleAddress;
    uint16 sampleLength;
    uint16 currentAddress;
    uint16 currentLength;
    uint8 shiftRegister;
    uint8 bitCount;
    uint8 tickPeriod;
    uint8 tickValue;
    bool loop;
    bool irq;

    void Save(ostream& out);
    void Load(istream& in);
};

// Using inheritence to be consistent with PPU. Maybe refactor someday...
class DMC : public DMCState {
  public:
    DMC(CPU* cpu) :
        DMCState(),
        cpu(cpu) {};

    CPU* cpu;

    void writeControl(uint8 value);
    void writeValue(uint8 value);
    void writeAddress(uint8 value);
    void writeLength(uint8 value);
    void restart();
    void stepTimer();
    void stepReader();
    void stepShifter();
    uint8 output();
};

// State that gets persisted when creating a save state.
class APUState {
  public:
    APUState() :
        cycle(0),
        framePeriod(0),
        frameValue(0),
        frameIRQ(false) {};

    uint64 cycle;
    uint8 framePeriod;
    uint8 frameValue;
    bool frameIRQ;

    void Save(ostream& out);
    void Load(istream& in);
};

// Using inheritence to be consistent with PPU. Maybe refactor someday...
class APU : public APUState {
  public:
    APU(Console* console, CPU* cpu) :
        APUState(),
        console(console),
        // channel(0),
        sampleRate(numeric_limits<float64>::infinity()),
        pulse1(),
        pulse2(),
        triangle(),
        noise(),
        dmc(cpu),
        filterChain() {};
    ~APU() {
      // delete channel;
      // channel = 0;
    }

    static constexpr float64 frameCounterRate = CPU::Frequency / 240.0;

    Console* console;
    // float32* channel;
    float64 sampleRate;
    Pulse pulse1;
    Pulse pulse2;
    Triangle triangle;
    Noise noise;
    DMC dmc;
    FilterChain filterChain;

    void Step();
    void SetSampleRate(float64 rate);
    void sendSample();
    float32 output();
    void stepFrameCounter();
    void stepTimer();
    void stepEnvelope();
    void stepSweep();
    void stepLength();
    void fireIRQ();
    uint8 readRegister(uint16 address);
    void writeRegister(uint16 address, uint8 value);
    uint8 readStatus();
    void writeControl(uint8 value);
    void writeFrameCounter(uint8 value);
};

}  // namespace nes

#endif  // define NES_APU_H
