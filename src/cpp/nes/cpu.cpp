#include "cpu.h"

#include <string>

#include "console.h"
#include "utils.h"

namespace nes {

// interrupt types
enum InterruptTypes {
  interruptNone = 1,
  interruptNMI = 2,
  interruptIRQ = 3
};

// addressing modes
enum AddressingModes {
  modeAbsolute = 1,
  modeAbsoluteX = 2,
  modeAbsoluteY = 3,
  modeAccumulator = 4,
  modeImmediate = 5,
  modeImplied = 6,
  modeIndexedIndirect = 7,
  modeIndirect = 8,
  modeIndirectIndexed = 9,
  modeRelative = 10,
  modeZeroPage = 11,
  modeZeroPageX = 12,
  modeZeroPageY = 13
};

// instructionModes indicates the addressing mode for each instruction
static uint8 instructionModes[256] = {
  6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
  10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
  1, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
  10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
  6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
  10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
  6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 8, 1, 1, 1,
  10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
  5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
  10, 9, 6, 9, 12, 12, 13, 13, 6, 3, 6, 3, 2, 2, 3, 3,
  5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
  10, 9, 6, 9, 12, 12, 13, 13, 6, 3, 6, 3, 2, 2, 3, 3,
  5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
  10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
  5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
  10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2
};

// instructionSizes indicates the size of each instruction in bytes
static uint8 instructionSizes[256] = {
  1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
  3, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
  1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
  1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 0, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 0, 3, 0, 0,
  2, 2, 2, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
  2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0
};

// instructionCycles indicates the number of cycles used by each instruction,
// not including conditional cycles
static uint8 instructionCycles[256] = {
  7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
  2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
  6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
  2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
  6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
  2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
  6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
  2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
  2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
  2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
  2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
  2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
  2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
  2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
  2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
  2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7
};

// instructionPageCycles indicates the number of cycles used by each
// instruction when a page is crossed
static uint8 instructionPageCycles[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0
};

// instructionNames indicates the name of each instruction
static std::string instructionNames[256] = {
  "BRK", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO",
  "PHP", "ORA", "ASL", "ANC", "NOP", "ORA", "ASL", "SLO",
  "BPL", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO",
  "CLC", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO",
  "JSR", "AND", "KIL", "RLA", "BIT", "AND", "ROL", "RLA",
  "PLP", "AND", "ROL", "ANC", "BIT", "AND", "ROL", "RLA",
  "BMI", "AND", "KIL", "RLA", "NOP", "AND", "ROL", "RLA",
  "SEC", "AND", "NOP", "RLA", "NOP", "AND", "ROL", "RLA",
  "RTI", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE",
  "PHA", "EOR", "LSR", "ALR", "JMP", "EOR", "LSR", "SRE",
  "BVC", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE",
  "CLI", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE",
  "RTS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA",
  "PLA", "ADC", "ROR", "ARR", "JMP", "ADC", "ROR", "RRA",
  "BVS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA",
  "SEI", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA",
  "NOP", "STA", "NOP", "SAX", "STY", "STA", "STX", "SAX",
  "DEY", "NOP", "TXA", "XAA", "STY", "STA", "STX", "SAX",
  "BCC", "STA", "KIL", "AHX", "STY", "STA", "STX", "SAX",
  "TYA", "STA", "TXS", "TAS", "SHY", "STA", "SHX", "AHX",
  "LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX",
  "TAY", "LDA", "TAX", "LAX", "LDY", "LDA", "LDX", "LAX",
  "BCS", "LDA", "KIL", "LAX", "LDY", "LDA", "LDX", "LAX",
  "CLV", "LDA", "TSX", "LAS", "LDY", "LDA", "LDX", "LAX",
  "CPY", "CMP", "NOP", "DCP", "CPY", "CMP", "DEC", "DCP",
  "INY", "CMP", "DEX", "AXS", "CPY", "CMP", "DEC", "DCP",
  "BNE", "CMP", "KIL", "DCP", "NOP", "CMP", "DEC", "DCP",
  "CLD", "CMP", "NOP", "DCP", "NOP", "CMP", "DEC", "DCP",
  "CPX", "SBC", "NOP", "ISC", "CPX", "SBC", "INC", "ISC",
  "INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISC",
  "BEQ", "SBC", "KIL", "ISC", "NOP", "SBC", "INC", "ISC",
  "SED", "SBC", "NOP", "ISC", "NOP", "SBC", "INC", "ISC"
};

void adc(CPU* cpu, StepInfo* info);
void and_(CPU* cpu, StepInfo* info);
void asl(CPU* cpu, StepInfo* info);
void bcc(CPU* cpu, StepInfo* info);
void bcs(CPU* cpu, StepInfo* info);
void beq(CPU* cpu, StepInfo* info);
void bit(CPU* cpu, StepInfo* info);
void bmi(CPU* cpu, StepInfo* info);
void bne(CPU* cpu, StepInfo* info);
void bpl(CPU* cpu, StepInfo* info);
void brk(CPU* cpu, StepInfo* info);
void bvc(CPU* cpu, StepInfo* info);
void bvs(CPU* cpu, StepInfo* info);
void clc(CPU* cpu, StepInfo* info);
void cld(CPU* cpu, StepInfo* info);
void cli(CPU* cpu, StepInfo* info);
void clv(CPU* cpu, StepInfo* info);
void cmp(CPU* cpu, StepInfo* info);
void cpx(CPU* cpu, StepInfo* info);
void cpy(CPU* cpu, StepInfo* info);
void dec(CPU* cpu, StepInfo* info);
void dex(CPU* cpu, StepInfo* info);
void dey(CPU* cpu, StepInfo* info);
void eor(CPU* cpu, StepInfo* info);
void inc(CPU* cpu, StepInfo* info);
void inx(CPU* cpu, StepInfo* info);
void iny(CPU* cpu, StepInfo* info);
void jmp(CPU* cpu, StepInfo* info);
void jsr(CPU* cpu, StepInfo* info);
void lda(CPU* cpu, StepInfo* info);
void ldx(CPU* cpu, StepInfo* info);
void ldy(CPU* cpu, StepInfo* info);
void lsr(CPU* cpu, StepInfo* info);
void nop(CPU* cpu, StepInfo* info);
void ora(CPU* cpu, StepInfo* info);
void pha(CPU* cpu, StepInfo* info);
void php(CPU* cpu, StepInfo* info);
void pla(CPU* cpu, StepInfo* info);
void plp(CPU* cpu, StepInfo* info);
void rol(CPU* cpu, StepInfo* info);
void ror(CPU* cpu, StepInfo* info);
void rti(CPU* cpu, StepInfo* info);
void rts(CPU* cpu, StepInfo* info);
void sbc(CPU* cpu, StepInfo* info);
void sec(CPU* cpu, StepInfo* info);
void sed(CPU* cpu, StepInfo* info);
void sei(CPU* cpu, StepInfo* info);
void sta(CPU* cpu, StepInfo* info);
void stx(CPU* cpu, StepInfo* info);
void sty(CPU* cpu, StepInfo* info);
void tax(CPU* cpu, StepInfo* info);
void tay(CPU* cpu, StepInfo* info);
void tsx(CPU* cpu, StepInfo* info);
void txa(CPU* cpu, StepInfo* info);
void txs(CPU* cpu, StepInfo* info);
void tya(CPU* cpu, StepInfo* info);
void ahx(CPU* cpu, StepInfo* info);
void alr(CPU* cpu, StepInfo* info);
void anc(CPU* cpu, StepInfo* info);
void arr(CPU* cpu, StepInfo* info);
void axs(CPU* cpu, StepInfo* info);
void dcp(CPU* cpu, StepInfo* info);
void isc(CPU* cpu, StepInfo* info);
void kil(CPU* cpu, StepInfo* info);
void las(CPU* cpu, StepInfo* info);
void lax(CPU* cpu, StepInfo* info);
void rla(CPU* cpu, StepInfo* info);
void rra(CPU* cpu, StepInfo* info);
void sax(CPU* cpu, StepInfo* info);
void shx(CPU* cpu, StepInfo* info);
void shy(CPU* cpu, StepInfo* info);
void slo(CPU* cpu, StepInfo* info);
void sre(CPU* cpu, StepInfo* info);
void tas(CPU* cpu, StepInfo* info);
void xaa(CPU* cpu, StepInfo* info);

// function table for each instruction
void (*table[256]) (CPU*, StepInfo*) = {
  brk, ora, kil, slo, nop, ora, asl, slo,
  php, ora, asl, anc, nop, ora, asl, slo,
  bpl, ora, kil, slo, nop, ora, asl, slo,
  clc, ora, nop, slo, nop, ora, asl, slo,
  jsr, and_, kil, rla, bit, and_, rol, rla,
  plp, and_, rol, anc, bit, and_, rol, rla,
  bmi, and_, kil, rla, nop, and_, rol, rla,
  sec, and_, nop, rla, nop, and_, rol, rla,
  rti, eor, kil, sre, nop, eor, lsr, sre,
  pha, eor, lsr, alr, jmp, eor, lsr, sre,
  bvc, eor, kil, sre, nop, eor, lsr, sre,
  cli, eor, nop, sre, nop, eor, lsr, sre,
  rts, adc, kil, rra, nop, adc, ror, rra,
  pla, adc, ror, arr, jmp, adc, ror, rra,
  bvs, adc, kil, rra, nop, adc, ror, rra,
  sei, adc, nop, rra, nop, adc, ror, rra,
  nop, sta, nop, sax, sty, sta, stx, sax,
  dey, nop, txa, xaa, sty, sta, stx, sax,
  bcc, sta, kil, ahx, sty, sta, stx, sax,
  tya, sta, txs, tas, shy, sta, shx, ahx,
  ldy, lda, ldx, lax, ldy, lda, ldx, lax,
  tay, lda, tax, lax, ldy, lda, ldx, lax,
  bcs, lda, kil, lax, ldy, lda, ldx, lax,
  clv, lda, tsx, las, ldy, lda, ldx, lax,
  cpy, cmp, nop, dcp, cpy, cmp, dec, dcp,
  iny, cmp, dex, axs, cpy, cmp, dec, dcp,
  bne, cmp, kil, dcp, nop, cmp, dec, dcp,
  cld, cmp, nop, dcp, nop, cmp, dec, dcp,
  cpx, sbc, nop, isc, cpx, sbc, inc, isc,
  inx, sbc, nop, sbc, cpx, sbc, inc, isc,
  beq, sbc, kil, isc, nop, sbc, inc, isc,
  sed, sbc, nop, isc, nop, sbc, inc, isc
};

void CPUState::Save(ostream& out) {
  utils::writeUint64(out, cycles);
  utils::writeUint16(out, PC);
  utils::writeUint8(out, SP);
  utils::writeUint8(out, A);
  utils::writeUint8(out, X);
  utils::writeUint8(out, Y);
  utils::writeUint8(out, C);
  utils::writeUint8(out, Z);
  utils::writeUint8(out, I);
  utils::writeUint8(out, D);
  utils::writeUint8(out, B);
  utils::writeUint8(out, U);
  utils::writeUint8(out, V);
  utils::writeUint8(out, N);
  utils::writeUint8(out, interrupt);
  utils::writeInt32(out, stall);
}

void CPUState::Load(istream& in) {
  cycles = utils::readUint64(in);
  PC = utils::readUint16(in);
  SP = utils::readUint8(in);
  A = utils::readUint8(in);
  X = utils::readUint8(in);
  Y = utils::readUint8(in);
  C = utils::readUint8(in);
  Z = utils::readUint8(in);
  I = utils::readUint8(in);
  D = utils::readUint8(in);
  B = utils::readUint8(in);
  U = utils::readUint8(in);
  V = utils::readUint8(in);
  N = utils::readUint8(in);
  interrupt = utils::readUint8(in);
  stall = utils::readInt32(in);
}

uint8 CPU::Read(uint16 address) {
  if (address < 0x2000) {
    return console->RAM[address%0x0800];
  } else if (address < 0x4000) {
    return console->ppu->readRegister(0x2000 + address%8);
  } else if (address == 0x4014) {
    return console->ppu->readRegister(address);
  } else if (address == 0x4015) {
    return console->apu->readRegister(address);
  } else if (address == 0x4016) {
    return console->controller1->Read();
  } else if (address == 0x4017) {
    return console->controller2->Read();
  } else if (address < 0x6000) {
    // TODO: I/O registers
  } else if (address >= 0x6000) {
    return console->mapper->Read(address);
  } else {
    // TODO: add logging.
    // log.Fatalf("unhandled cpu memory read at address: 0x%04X", address)
  }
  return 0;
}

void CPU::Write(uint16 address, uint8 value) {
  if (address < 0x2000) {
    console->RAM[address%0x0800] = value;
  } else if (address < 0x4000) {
    console->ppu->writeRegister(0x2000+address%8, value);
  } else if (address < 0x4014) {
    console->apu->writeRegister(address, value);
  } else if (address == 0x4014) {
    console->ppu->writeRegister(address, value);
  } else if (address == 0x4015) {
    console->apu->writeRegister(address, value);
  } else if (address == 0x4016) {
    console->controller1->Write(value);
    console->controller2->Write(value);
  } else if (address == 0x4017) {
    console->apu->writeRegister(address, value);
  } else if (address < 0x6000) {
    // TODO: I/O registers
  } else if (address >= 0x6000) {
    console->mapper->Write(address, value);
  } else {
    // TODO: add logging.
    // log.Fatalf("unhandled cpu memory write at address: 0x%04X", address)
  }
}

// Reset resets the CPU to its initial powerup state
void CPU::Reset() {
  PC = Read16(0xFFFC);
  SP = 0xFD;
  SetFlags(0x24);
}

// PrintInstruction prints the current CPU state
void CPU::PrintInstruction() {
  uint8 opcode = Read(PC);
  uint8 bytes = instructionSizes[opcode];
  std::string name = instructionNames[opcode];
  char* buf;
  asprintf(&buf,"%02X", Read(PC+0));
  std::string w0(buf);
  free(buf);
  asprintf(&buf,"%02X", Read(PC+1));
  std::string w1(buf);
  free(buf);
  asprintf(&buf,"%02X", Read(PC+2));
  std::string w2(buf);
  free(buf);
  if (bytes < 2) {
    w1 = std::string("  ");
  }
  if (bytes < 3) {
    w2 = std::string("  ");
  }
  asprintf(&buf,
      "%4X  %s %s %s  %s %28sA:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%3d\n",
      PC, w0.c_str(), w1.c_str(), w2.c_str(), name.c_str(), "",
      A, X, Y, Flags(), SP, uint32((cycles*3)%341));
  std::cout << buf;
  free(buf);
}

// pagesDiffer returns true if the two addresses reference different pages
static bool pagesDiffer(uint16 a, uint16 b) {
  return (a&0xFF00) != (b&0xFF00);
}

// addBranchCycles adds a cycle for taking a branch and adds another cycle
// if the branch jumps to a new page
void CPU::addBranchCycles(StepInfo* info) {
  cycles++;
  if (pagesDiffer(info->pc, info->address)) {
    cycles++;
  }
}

void CPU::compare(uint8 a, uint8 b) {
  setZN(a - b);
  if (a >= b) {
    C = 1;
  } else {
    C = 0;
  }
}

// Read16 reads two bytes using Read to return a double-word value
uint16 CPU::Read16(uint16 address) {
  uint16 lo = uint16(Read(address));
  uint16 hi = uint16(Read(address + 1));
  return (hi<<8) | lo;
}

// read16bug emulates a 6502 bug that caused the low byte to wrap without
// incrementing the high byte
uint16 CPU::read16bug(uint16 address) {
  uint16 a = address;
  uint16 b = (a & 0xFF00) | uint16(uint8(a)+1);
  uint16 lo = uint16(Read(a));
  uint16 hi = uint16(Read(b));
  return (hi<<8) | lo;
}

// push pushes a byte onto the stack
void CPU::push(uint8 value) {
  Write(0x100|uint16(SP), value);
  SP--;
}

// pull pops a byte from the stack
uint8 CPU::pull() {
  SP++;
  return Read(0x100 | uint16(SP));
}

// push16 pushes two bytes onto the stack
void CPU::push16(uint16 value) {
  uint8 hi = uint8(value >> 8);
  uint8 lo = uint8(value & 0xFF);
  push(hi);
  push(lo);
}

// pull16 pops two bytes from the stack
uint16 CPU::pull16() {
  uint16 lo = uint16(pull());
  uint16 hi = uint16(pull());
  return (hi<<8) | lo;
}

// Flags returns the processor status flags
uint8 CPU::Flags() {
  uint8 flags = 0;
  flags |= C << 0;
  flags |= Z << 1;
  flags |= I << 2;
  flags |= D << 3;
  flags |= B << 4;
  flags |= U << 5;
  flags |= V << 6;
  flags |= N << 7;
  return flags;
}

// SetFlags sets the processor status flags
void CPU::SetFlags(uint8 flags) {
  C = (flags >> 0) & 1;
  Z = (flags >> 1) & 1;
  I = (flags >> 2) & 1;
  D = (flags >> 3) & 1;
  B = (flags >> 4) & 1;
  U = (flags >> 5) & 1;
  V = (flags >> 6) & 1;
  N = (flags >> 7) & 1;
}

// setZ sets the zero flag if the argument is zero
void CPU::setZ(uint8 value) {
  if (value == 0) {
    Z = 1;
  } else {
    Z = 0;
  }
}

// setN sets the negative flag if the argument is negative (high bit is set)
void CPU::setN(uint8 value) {
  if ((value&0x80) != 0) {
    N = 1;
  } else {
    N = 0;
  }
}

// setZN sets the zero flag and the negative flag
void CPU::setZN(uint8 value) {
  setZ(value);
  setN(value);
}

// triggerNMI causes a non-maskable interrupt to occur on the next cycle
void CPU::triggerNMI() {
  interrupt = InterruptTypes::interruptNMI;
}

// triggerIRQ causes an IRQ interrupt to occur on the next cycle
void CPU::triggerIRQ() {
  if (I == 0) {
    interrupt = InterruptTypes::interruptIRQ;
  }
}

// Step executes a single CPU instruction
int32 CPU::Step() {
  if (stall > 0) {
    stall--;
    return 1;
  }

  uint64 prevCycles = cycles;

  switch (interrupt) {
  case InterruptTypes::interruptNMI:
    nmi();
    break;
  case InterruptTypes::interruptIRQ:
    irq();
    break;
  }
  interrupt = InterruptTypes::interruptNone;

  uint8 opcode = Read(PC);
  uint8 mode = instructionModes[opcode];

  uint16 address = 0;
  bool pageCrossed = false;
  switch (mode) {
  case AddressingModes::modeAbsolute:
    address = Read16(PC + 1);
    break;
  case AddressingModes::modeAbsoluteX:
    address = Read16(PC+1) + uint16(X);
    pageCrossed = pagesDiffer(address-uint16(X), address);
    break;
  case AddressingModes::modeAbsoluteY:
    address = Read16(PC+1) + uint16(Y);
    pageCrossed = pagesDiffer(address-uint16(Y), address);
    break;
  case AddressingModes::modeAccumulator:
    address = 0;
    break;
  case AddressingModes::modeImmediate:
    address = PC + 1;
    break;
  case AddressingModes::modeImplied:
    address = 0;
    break;
  case AddressingModes::modeIndexedIndirect:
    address = read16bug(uint16(Read(PC+1) + X));
    break;
  case AddressingModes::modeIndirect:
    address = read16bug(Read16(PC + 1));
    break;
  case AddressingModes::modeIndirectIndexed:
    address = read16bug(uint16(Read(PC+1))) + uint16(Y);
    pageCrossed = pagesDiffer(address-uint16(Y), address);
    break;
  case AddressingModes::modeRelative:
    {
      uint16 offset = uint16(Read(PC + 1));
      if (offset < 0x80) {
        address = PC + 2 + offset;
      } else {
        address = PC + 2 + offset - 0x100;
      }
      break;
    }
  case AddressingModes::modeZeroPage:
    address = uint16(Read(PC + 1));
    break;
  case AddressingModes::modeZeroPageX:
    address = uint16(Read(PC+1)+X) & 0xff;
    break;
  case AddressingModes::modeZeroPageY:
    address = uint16(Read(PC+1)+Y) & 0xff;
    break;
  }

  PC += uint16(instructionSizes[opcode]);
  cycles += uint64(instructionCycles[opcode]);
  if (pageCrossed) {
    cycles += uint64(instructionPageCycles[opcode]);
  }
  StepInfo info(address, PC, mode);
  table[opcode](this, &info);

  return int(cycles - prevCycles);
}

// NMI - Non-Maskable Interrupt
void CPU::nmi() {
  push16(PC);
  php(this, NULL);
  PC = Read16(0xFFFA);
  I = 1;
  cycles += 7;
}

// IRQ - IRQ Interrupt
void CPU::irq() {
  push16(PC);
  php(this, NULL);
  PC = Read16(0xFFFE);
  I = 1;
  cycles += 7;
}

// ADC - Add with Carry;
void adc(CPU* cpu, StepInfo* info) {
  uint8 a = cpu->A;
  uint8 b = cpu->Read(info->address);
  uint8 c = cpu->C;
  cpu->A = a + b + c;
  cpu->setZN(cpu->A);
  if (int32(a)+int32(b)+int32(c) > 0xFF) {
    cpu->C = 1;
  } else {
    cpu->C = 0;
  }
  if (((a^b)&0x80) == 0 && ((a^cpu->A)&0x80) != 0) {
    cpu->V = 1;
  } else {
    cpu->V = 0;
  }
}

// AND - Logical AND;
void and_(CPU* cpu, StepInfo* info) {
  cpu->A = cpu->A & cpu->Read(info->address);
  cpu->setZN(cpu->A);
}

// ASL - Arithmetic Shift Left;
void asl(CPU* cpu, StepInfo* info) {
  if (info->mode == modeAccumulator) {
    cpu->C = (cpu->A >> 7) & 1;
    cpu->A <<= 1;
    cpu->setZN(cpu->A);
  } else {
    uint8 value = cpu->Read(info->address);
    cpu->C = (value >> 7) & 1;
    value <<= 1;
    cpu->Write(info->address, value);
    cpu->setZN(value);
  }
}

// BCC - Branch if Carry Clear;
void bcc(CPU* cpu, StepInfo* info) {
  if (cpu->C == 0) {
    cpu->PC = info->address;
    cpu->addBranchCycles(info);
  }
}

// BCS - Branch if Carry Set;
void bcs(CPU* cpu, StepInfo* info) {
  if (cpu->C != 0) {
    cpu->PC = info->address;
    cpu->addBranchCycles(info);
  }
}

// BEQ - Branch if Equal;
void beq(CPU* cpu, StepInfo* info) {
  if (cpu->Z != 0) {
    cpu->PC = info->address;
    cpu->addBranchCycles(info);
  }
}

// BIT - Bit Test;
void bit(CPU* cpu, StepInfo* info) {
  uint8 value = cpu->Read(info->address);
  cpu->V = (value >> 6) & 1;
  cpu->setZ(value & cpu->A);
  cpu->setN(value);
}

// BMI - Branch if Minus;
void bmi(CPU* cpu, StepInfo* info) {
  if (cpu->N != 0) {
    cpu->PC = info->address;
    cpu->addBranchCycles(info);
  }
}

// BNE - Branch if Not Equal;
void bne(CPU* cpu, StepInfo* info) {
  if (cpu->Z == 0) {
    cpu->PC = info->address;
    cpu->addBranchCycles(info);
  }
}

// BPL - Branch if Positive;
void bpl(CPU* cpu, StepInfo* info) {
  if (cpu->N == 0) {
    cpu->PC = info->address;
    cpu->addBranchCycles(info);
  }
}

// BRK - Force Interrupt;
void brk(CPU* cpu, StepInfo* info) {
  cpu->push16(cpu->PC);
  php(cpu, info);
  sei(cpu, info);
  cpu->PC = cpu->Read16(0xFFFE);
}

// BVC - Branch if Overflow Clear;
void bvc(CPU* cpu, StepInfo* info) {
  if (cpu->V == 0) {
    cpu->PC = info->address;
    cpu->addBranchCycles(info);
  }
}

// BVS - Branch if Overflow Set;
void bvs(CPU* cpu, StepInfo* info) {
  if (cpu->V != 0) {
    cpu->PC = info->address;
    cpu->addBranchCycles(info);
  }
}

// CLC - Clear Carry Flag;
void clc(CPU* cpu, StepInfo* info) {
  cpu->C = 0;
}

// CLD - Clear Decimal Mode;
void cld(CPU* cpu, StepInfo* info) {
  cpu->D = 0;
}

// CLI - Clear Interrupt Disable;
void cli(CPU* cpu, StepInfo* info) {
  cpu->I = 0;
}

// CLV - Clear Overflow Flag;
void clv(CPU* cpu, StepInfo* info) {
  cpu->V = 0;
}

// CMP - Compare;
void cmp(CPU* cpu, StepInfo* info) {
  uint8 value = cpu->Read(info->address);
  cpu->compare(cpu->A, value);
}

// CPX - Compare X Register;
void cpx(CPU* cpu, StepInfo* info) {
  uint8 value = cpu->Read(info->address);
  cpu->compare(cpu->X, value);
}

// CPY - Compare Y Register;
void cpy(CPU* cpu, StepInfo* info) {
  uint8 value = cpu->Read(info->address);
  cpu->compare(cpu->Y, value);
}

// DEC - Decrement Memory;
void dec(CPU* cpu, StepInfo* info) {
  uint8 value = cpu->Read(info->address) - 1;
  cpu->Write(info->address, value);
  cpu->setZN(value);
}

// DEX - Decrement X Register;
void dex(CPU* cpu, StepInfo* info) {
  cpu->X--;
  cpu->setZN(cpu->X);
}

// DEY - Decrement Y Register;
void dey(CPU* cpu, StepInfo* info) {
  cpu->Y--;
  cpu->setZN(cpu->Y);
}

// EOR - Exclusive OR;
void eor(CPU* cpu, StepInfo* info) {
  cpu->A = cpu->A ^ cpu->Read(info->address);
  cpu->setZN(cpu->A);
}

// INC - Increment Memory;
void inc(CPU* cpu, StepInfo* info) {
  uint8 value = cpu->Read(info->address) + 1;
  cpu->Write(info->address, value);
  cpu->setZN(value);
}

// INX - Increment X Register;
void inx(CPU* cpu, StepInfo* info) {
  cpu->X++;
  cpu->setZN(cpu->X);
}

// INY - Increment Y Register;
void iny(CPU* cpu, StepInfo* info) {
  cpu->Y++;
  cpu->setZN(cpu->Y);
}

// JMP - Jump;
void jmp(CPU* cpu, StepInfo* info) {
  cpu->PC = info->address;
}

// JSR - Jump to Subroutine;
void jsr(CPU* cpu, StepInfo* info) {
  cpu->push16(cpu->PC - 1);
  cpu->PC = info->address;
}

// LDA - Load Accumulator;
void lda(CPU* cpu, StepInfo* info) {
  cpu->A = cpu->Read(info->address);
  cpu->setZN(cpu->A);
}

// LDX - Load X Register;
void ldx(CPU* cpu, StepInfo* info) {
  cpu->X = cpu->Read(info->address);
  cpu->setZN(cpu->X);
}

// LDY - Load Y Register;
void ldy(CPU* cpu, StepInfo* info) {
  cpu->Y = cpu->Read(info->address);
  cpu->setZN(cpu->Y);
}

// LSR - Logical Shift Right;
void lsr(CPU* cpu, StepInfo* info) {
  if (info->mode == modeAccumulator) {
    cpu->C = cpu->A & 1;
    cpu->A >>= 1;
    cpu->setZN(cpu->A);
  } else {
    uint8 value = cpu->Read(info->address);
    cpu->C = value & 1;
    value >>= 1;
    cpu->Write(info->address, value);
    cpu->setZN(value);
  }
}

// NOP - No Operation;
void nop(CPU* cpu, StepInfo* info) {}

// ORA - Logical Inclusive OR;
void ora(CPU* cpu, StepInfo* info) {
  cpu->A = cpu->A | cpu->Read(info->address);
  cpu->setZN(cpu->A);
}

// PHA - Push Accumulator;
void pha(CPU* cpu, StepInfo* info) {
  cpu->push(cpu->A);
}

// PHP - Push Processor Status;
void php(CPU* cpu, StepInfo* info) {
  cpu->push(cpu->Flags() | 0x10);
}

// PLA - Pull Accumulator;
void pla(CPU* cpu, StepInfo* info) {
  cpu->A = cpu->pull();
  cpu->setZN(cpu->A);
}

// PLP - Pull Processor Status;
void plp(CPU* cpu, StepInfo* info) {
  cpu->SetFlags((cpu->pull()&0xEF) | 0x20);
}

// ROL - Rotate Left;
void rol(CPU* cpu, StepInfo* info) {
  if (info->mode == modeAccumulator) {
    uint8 c = cpu->C;
    cpu->C = (cpu->A >> 7) & 1;
    cpu->A = (cpu->A << 1) | c;
    cpu->setZN(cpu->A);
  } else {
    uint8 c = cpu->C;
    uint8 value = cpu->Read(info->address);
    cpu->C = (value >> 7) & 1;
    value = (value << 1) | c;
    cpu->Write(info->address, value);
    cpu->setZN(value);
  }
}

// ROR - Rotate Right;
void ror(CPU* cpu, StepInfo* info) {
  if (info->mode == modeAccumulator) {
    uint8 c = cpu->C;
    cpu->C = cpu->A & 1;
    cpu->A = (cpu->A >> 1) | (c << 7);
    cpu->setZN(cpu->A);
  } else {
    uint8 c = cpu->C;
    uint8 value = cpu->Read(info->address);
    cpu->C = value & 1;
    value = (value >> 1) | (c << 7);
    cpu->Write(info->address, value);
    cpu->setZN(value);
  }
}

// RTI - Return from Interrupt;
void rti(CPU* cpu, StepInfo* info) {
  cpu->SetFlags((cpu->pull()&0xEF) | 0x20);
  cpu->PC = cpu->pull16();
}

// RTS - Return from Subroutine;
void rts(CPU* cpu, StepInfo* info) {
  cpu->PC = cpu->pull16() + 1;
}

// SBC - Subtract with Carry;
void sbc(CPU* cpu, StepInfo* info) {
  uint8 a = cpu->A;
  uint8 b = cpu->Read(info->address);
  uint8 c = cpu->C;
  cpu->A = a - b - (1 - c);
  cpu->setZN(cpu->A);
  if (int32(a)-int32(b)-int32(1-c) >= 0) {
    cpu->C = 1;
  } else {
    cpu->C = 0;
  }
  if (((a^b)&0x80) != 0 && ((a^cpu->A)&0x80) != 0) {
    cpu->V = 1;
  } else {
    cpu->V = 0;
  }
}

// SEC - Set Carry Flag;
void sec(CPU* cpu, StepInfo* info) {
  cpu->C = 1;
}

// SED - Set Decimal Flag;
void sed(CPU* cpu, StepInfo* info) {
  cpu->D = 1;
}

// SEI - Set Interrupt Disable;
void sei(CPU* cpu, StepInfo* info) {
  cpu->I = 1;
}

// STA - Store Accumulator;
void sta(CPU* cpu, StepInfo* info) {
  cpu->Write(info->address, cpu->A);
}

// STX - Store X Register;
void stx(CPU* cpu, StepInfo* info) {
  cpu->Write(info->address, cpu->X);
}

// STY - Store Y Register;
void sty(CPU* cpu, StepInfo* info) {
  cpu->Write(info->address, cpu->Y);
}

// TAX - Transfer Accumulator to X;
void tax(CPU* cpu, StepInfo* info) {
  cpu->X = cpu->A;
  cpu->setZN(cpu->X);
}

// TAY - Transfer Accumulator to Y;
void tay(CPU* cpu, StepInfo* info) {
  cpu->Y = cpu->A;
  cpu->setZN(cpu->Y);
}

// TSX - Transfer Stack Pointer to X;
void tsx(CPU* cpu, StepInfo* info) {
  cpu->X = cpu->SP;
  cpu->setZN(cpu->X);
}

// TXA - Transfer X to Accumulator;
void txa(CPU* cpu, StepInfo* info) {
  cpu->A = cpu->X;
  cpu->setZN(cpu->A);
}

// TXS - Transfer X to Stack Pointer;
void txs(CPU* cpu, StepInfo* info) {
  cpu->SP = cpu->X;
}

// TYA - Transfer Y to Accumulator;
void tya(CPU* cpu, StepInfo* info) {
  cpu->A = cpu->Y;
  cpu->setZN(cpu->A);
}

// illegal opcodes below;

void ahx(CPU* cpu, StepInfo* info) {}

void alr(CPU* cpu, StepInfo* info) {}

void anc(CPU* cpu, StepInfo* info) {}

void arr(CPU* cpu, StepInfo* info) {}

void axs(CPU* cpu, StepInfo* info) {}

void dcp(CPU* cpu, StepInfo* info) {}

void isc(CPU* cpu, StepInfo* info) {}

void kil(CPU* cpu, StepInfo* info) {}

void las(CPU* cpu, StepInfo* info) {}

void lax(CPU* cpu, StepInfo* info) {}

void rla(CPU* cpu, StepInfo* info) {}

void rra(CPU* cpu, StepInfo* info) {}

void sax(CPU* cpu, StepInfo* info) {}

void shx(CPU* cpu, StepInfo* info) {}

void shy(CPU* cpu, StepInfo* info) {}

void slo(CPU* cpu, StepInfo* info) {}

void sre(CPU* cpu, StepInfo* info) {}

void tas(CPU* cpu, StepInfo* info) {}

void xaa(CPU* cpu, StepInfo* info) {}

}  // namespace nes;
