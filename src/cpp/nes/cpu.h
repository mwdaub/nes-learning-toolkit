#ifndef NES_CPU_H
#define NES_CPU_H

#include <iostream>

#include "types.h"
#include "memory.h"

using namespace std;

namespace nes {

struct StepInfo {
  uint16 address;
  uint16 pc;
  uint8 mode;

  StepInfo(uint16 address, uint16 pc, uint8 mode) :
      address(address),
      pc(pc),
      mode(mode) {};
};

// State that gets persisted when creating a save state.
class CPUState {
  public:
    CPUState() :
        cycles(0),
        PC(0),
        SP(0),
        A(0),
        X(0),
        Y(0),
        C(0),
        Z(0),
        I(0),
        D(0),
        B(0),
        U(0),
        V(0),
        N(0),
        interrupt(0),
        stall(0) {};

    uint64 cycles;   // number of cycles
    uint16 PC;       // program counter
    uint8 SP;        // stack pointer
    uint8 A;         // accumulator
    uint8 X;         // x register
    uint8 Y;         // y register
    uint8 C;         // carry flag
    uint8 Z;         // zero flag
    uint8 I;         // interrupt disable flag
    uint8 D;         // decimal mode flag
    uint8 B;         // break command flag
    uint8 U;         // unused flag
    uint8 V;         // overflow flag
    uint8 N;         // negative flag
    uint8 interrupt; // interrupt type to perform
    int32 stall;     // number of cycles to stall

    void Save(ostream& out);
    void Load(istream& in);
};

class Console;

// Using inheritence to be consistent with PPU. Maybe refactor someday...
class CPU : public CPUState {
  public:
    CPU(Console* console) :
        CPUState(),
        console(console) {
      Reset();
    };

    static const int32 Frequency = 1789773;

    Console* console;

    uint8 Read(uint16 address);
    void Write(uint16 address, uint8 value);
    int32 Step();

    void Reset();

    void PrintInstruction();
    void addBranchCycles(StepInfo* info);
    void compare(uint8 a, uint8 b);
    uint16 Read16(uint16 address);
    uint16 read16bug(uint16 address);

    void push(uint8 value);
    uint8 pull();
    void push16(uint16 value);
    uint16 pull16();

    uint8 Flags();
    void SetFlags(uint8 flags);

    void setN(uint8 value);
    void setZ(uint8 value);
    void setZN(uint8 value);
    void triggerNMI();
    void triggerIRQ();

    void nmi();
    void irq();
};

}  // namespace nes

#endif  // define NES_CPU_H
