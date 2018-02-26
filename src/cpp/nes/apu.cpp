#include "apu.h"

#include "console.h"
#include "utils.h"

namespace nes {

uint8 lengthTable[32] = {
  10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
  12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
};

uint8 dutyTable[4][8] = {
  {0, 1, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 0, 0, 0, 0, 0},
  {0, 1, 1, 1, 1, 0, 0, 0},
  {1, 0, 0, 1, 1, 1, 1, 1}
};

uint8 triangleTable[32] = {
  15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};

uint16 noiseTable[16] = {
  4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068,
};

uint8 dmcTable[16] = {
  214, 190, 170, 160, 143, 127, 113, 107, 95, 80, 71, 64, 53, 42, 36, 27
};

float32 pulseTable [31] = {
  95.52f / (8128.0f/float32(0) + 100),
  95.52f / (8128.0f/float32(1) + 100),
  95.52f / (8128.0f/float32(2) + 100),
  95.52f / (8128.0f/float32(3) + 100),
  95.52f / (8128.0f/float32(4) + 100),
  95.52f / (8128.0f/float32(5) + 100),
  95.52f / (8128.0f/float32(6) + 100),
  95.52f / (8128.0f/float32(7) + 100),
  95.52f / (8128.0f/float32(8) + 100),
  95.52f / (8128.0f/float32(9) + 100),
  95.52f / (8128.0f/float32(10) + 100),
  95.52f / (8128.0f/float32(11) + 100),
  95.52f / (8128.0f/float32(12) + 100),
  95.52f / (8128.0f/float32(13) + 100),
  95.52f / (8128.0f/float32(14) + 100),
  95.52f / (8128.0f/float32(15) + 100),
  95.52f / (8128.0f/float32(16) + 100),
  95.52f / (8128.0f/float32(17) + 100),
  95.52f / (8128.0f/float32(18) + 100),
  95.52f / (8128.0f/float32(19) + 100),
  95.52f / (8128.0f/float32(20) + 100),
  95.52f / (8128.0f/float32(21) + 100),
  95.52f / (8128.0f/float32(22) + 100),
  95.52f / (8128.0f/float32(23) + 100),
  95.52f / (8128.0f/float32(24) + 100),
  95.52f / (8128.0f/float32(25) + 100),
  95.52f / (8128.0f/float32(26) + 100),
  95.52f / (8128.0f/float32(27) + 100),
  95.52f / (8128.0f/float32(28) + 100),
  95.52f / (8128.0f/float32(29) + 100),
  95.52f / (8128.0f/float32(30) + 100),
};

float32 tndTable[203] = {
  163.67f / (24329.0f/float32(0) + 100),
  163.67f / (24329.0f/float32(1) + 100),
  163.67f / (24329.0f/float32(2) + 100),
  163.67f / (24329.0f/float32(3) + 100),
  163.67f / (24329.0f/float32(4) + 100),
  163.67f / (24329.0f/float32(5) + 100),
  163.67f / (24329.0f/float32(6) + 100),
  163.67f / (24329.0f/float32(7) + 100),
  163.67f / (24329.0f/float32(8) + 100),
  163.67f / (24329.0f/float32(9) + 100),
  163.67f / (24329.0f/float32(10) + 100),
  163.67f / (24329.0f/float32(11) + 100),
  163.67f / (24329.0f/float32(12) + 100),
  163.67f / (24329.0f/float32(13) + 100),
  163.67f / (24329.0f/float32(14) + 100),
  163.67f / (24329.0f/float32(15) + 100),
  163.67f / (24329.0f/float32(16) + 100),
  163.67f / (24329.0f/float32(17) + 100),
  163.67f / (24329.0f/float32(18) + 100),
  163.67f / (24329.0f/float32(19) + 100),
  163.67f / (24329.0f/float32(20) + 100),
  163.67f / (24329.0f/float32(21) + 100),
  163.67f / (24329.0f/float32(22) + 100),
  163.67f / (24329.0f/float32(23) + 100),
  163.67f / (24329.0f/float32(24) + 100),
  163.67f / (24329.0f/float32(25) + 100),
  163.67f / (24329.0f/float32(26) + 100),
  163.67f / (24329.0f/float32(27) + 100),
  163.67f / (24329.0f/float32(28) + 100),
  163.67f / (24329.0f/float32(29) + 100),
  163.67f / (24329.0f/float32(30) + 100),
  163.67f / (24329.0f/float32(31) + 100),
  163.67f / (24329.0f/float32(32) + 100),
  163.67f / (24329.0f/float32(33) + 100),
  163.67f / (24329.0f/float32(34) + 100),
  163.67f / (24329.0f/float32(35) + 100),
  163.67f / (24329.0f/float32(36) + 100),
  163.67f / (24329.0f/float32(37) + 100),
  163.67f / (24329.0f/float32(38) + 100),
  163.67f / (24329.0f/float32(39) + 100),
  163.67f / (24329.0f/float32(40) + 100),
  163.67f / (24329.0f/float32(41) + 100),
  163.67f / (24329.0f/float32(42) + 100),
  163.67f / (24329.0f/float32(43) + 100),
  163.67f / (24329.0f/float32(44) + 100),
  163.67f / (24329.0f/float32(45) + 100),
  163.67f / (24329.0f/float32(46) + 100),
  163.67f / (24329.0f/float32(47) + 100),
  163.67f / (24329.0f/float32(48) + 100),
  163.67f / (24329.0f/float32(49) + 100),
  163.67f / (24329.0f/float32(50) + 100),
  163.67f / (24329.0f/float32(51) + 100),
  163.67f / (24329.0f/float32(52) + 100),
  163.67f / (24329.0f/float32(53) + 100),
  163.67f / (24329.0f/float32(54) + 100),
  163.67f / (24329.0f/float32(55) + 100),
  163.67f / (24329.0f/float32(56) + 100),
  163.67f / (24329.0f/float32(57) + 100),
  163.67f / (24329.0f/float32(58) + 100),
  163.67f / (24329.0f/float32(59) + 100),
  163.67f / (24329.0f/float32(60) + 100),
  163.67f / (24329.0f/float32(61) + 100),
  163.67f / (24329.0f/float32(62) + 100),
  163.67f / (24329.0f/float32(63) + 100),
  163.67f / (24329.0f/float32(64) + 100),
  163.67f / (24329.0f/float32(65) + 100),
  163.67f / (24329.0f/float32(66) + 100),
  163.67f / (24329.0f/float32(67) + 100),
  163.67f / (24329.0f/float32(68) + 100),
  163.67f / (24329.0f/float32(69) + 100),
  163.67f / (24329.0f/float32(70) + 100),
  163.67f / (24329.0f/float32(71) + 100),
  163.67f / (24329.0f/float32(72) + 100),
  163.67f / (24329.0f/float32(73) + 100),
  163.67f / (24329.0f/float32(74) + 100),
  163.67f / (24329.0f/float32(75) + 100),
  163.67f / (24329.0f/float32(76) + 100),
  163.67f / (24329.0f/float32(77) + 100),
  163.67f / (24329.0f/float32(78) + 100),
  163.67f / (24329.0f/float32(79) + 100),
  163.67f / (24329.0f/float32(80) + 100),
  163.67f / (24329.0f/float32(81) + 100),
  163.67f / (24329.0f/float32(82) + 100),
  163.67f / (24329.0f/float32(83) + 100),
  163.67f / (24329.0f/float32(84) + 100),
  163.67f / (24329.0f/float32(85) + 100),
  163.67f / (24329.0f/float32(86) + 100),
  163.67f / (24329.0f/float32(87) + 100),
  163.67f / (24329.0f/float32(88) + 100),
  163.67f / (24329.0f/float32(89) + 100),
  163.67f / (24329.0f/float32(90) + 100),
  163.67f / (24329.0f/float32(91) + 100),
  163.67f / (24329.0f/float32(92) + 100),
  163.67f / (24329.0f/float32(93) + 100),
  163.67f / (24329.0f/float32(94) + 100),
  163.67f / (24329.0f/float32(95) + 100),
  163.67f / (24329.0f/float32(96) + 100),
  163.67f / (24329.0f/float32(97) + 100),
  163.67f / (24329.0f/float32(98) + 100),
  163.67f / (24329.0f/float32(99) + 100),
  163.67f / (24329.0f/float32(100) + 100),
  163.67f / (24329.0f/float32(101) + 100),
  163.67f / (24329.0f/float32(102) + 100),
  163.67f / (24329.0f/float32(103) + 100),
  163.67f / (24329.0f/float32(104) + 100),
  163.67f / (24329.0f/float32(105) + 100),
  163.67f / (24329.0f/float32(106) + 100),
  163.67f / (24329.0f/float32(107) + 100),
  163.67f / (24329.0f/float32(108) + 100),
  163.67f / (24329.0f/float32(109) + 100),
  163.67f / (24329.0f/float32(110) + 100),
  163.67f / (24329.0f/float32(111) + 100),
  163.67f / (24329.0f/float32(112) + 100),
  163.67f / (24329.0f/float32(113) + 100),
  163.67f / (24329.0f/float32(114) + 100),
  163.67f / (24329.0f/float32(115) + 100),
  163.67f / (24329.0f/float32(116) + 100),
  163.67f / (24329.0f/float32(117) + 100),
  163.67f / (24329.0f/float32(118) + 100),
  163.67f / (24329.0f/float32(119) + 100),
  163.67f / (24329.0f/float32(120) + 100),
  163.67f / (24329.0f/float32(121) + 100),
  163.67f / (24329.0f/float32(122) + 100),
  163.67f / (24329.0f/float32(123) + 100),
  163.67f / (24329.0f/float32(124) + 100),
  163.67f / (24329.0f/float32(125) + 100),
  163.67f / (24329.0f/float32(126) + 100),
  163.67f / (24329.0f/float32(127) + 100),
  163.67f / (24329.0f/float32(128) + 100),
  163.67f / (24329.0f/float32(129) + 100),
  163.67f / (24329.0f/float32(130) + 100),
  163.67f / (24329.0f/float32(131) + 100),
  163.67f / (24329.0f/float32(132) + 100),
  163.67f / (24329.0f/float32(133) + 100),
  163.67f / (24329.0f/float32(134) + 100),
  163.67f / (24329.0f/float32(135) + 100),
  163.67f / (24329.0f/float32(136) + 100),
  163.67f / (24329.0f/float32(137) + 100),
  163.67f / (24329.0f/float32(138) + 100),
  163.67f / (24329.0f/float32(139) + 100),
  163.67f / (24329.0f/float32(140) + 100),
  163.67f / (24329.0f/float32(141) + 100),
  163.67f / (24329.0f/float32(142) + 100),
  163.67f / (24329.0f/float32(143) + 100),
  163.67f / (24329.0f/float32(144) + 100),
  163.67f / (24329.0f/float32(145) + 100),
  163.67f / (24329.0f/float32(146) + 100),
  163.67f / (24329.0f/float32(147) + 100),
  163.67f / (24329.0f/float32(148) + 100),
  163.67f / (24329.0f/float32(149) + 100),
  163.67f / (24329.0f/float32(150) + 100),
  163.67f / (24329.0f/float32(151) + 100),
  163.67f / (24329.0f/float32(152) + 100),
  163.67f / (24329.0f/float32(153) + 100),
  163.67f / (24329.0f/float32(154) + 100),
  163.67f / (24329.0f/float32(155) + 100),
  163.67f / (24329.0f/float32(156) + 100),
  163.67f / (24329.0f/float32(157) + 100),
  163.67f / (24329.0f/float32(158) + 100),
  163.67f / (24329.0f/float32(159) + 100),
  163.67f / (24329.0f/float32(160) + 100),
  163.67f / (24329.0f/float32(161) + 100),
  163.67f / (24329.0f/float32(162) + 100),
  163.67f / (24329.0f/float32(163) + 100),
  163.67f / (24329.0f/float32(164) + 100),
  163.67f / (24329.0f/float32(165) + 100),
  163.67f / (24329.0f/float32(166) + 100),
  163.67f / (24329.0f/float32(167) + 100),
  163.67f / (24329.0f/float32(168) + 100),
  163.67f / (24329.0f/float32(169) + 100),
  163.67f / (24329.0f/float32(170) + 100),
  163.67f / (24329.0f/float32(171) + 100),
  163.67f / (24329.0f/float32(172) + 100),
  163.67f / (24329.0f/float32(173) + 100),
  163.67f / (24329.0f/float32(174) + 100),
  163.67f / (24329.0f/float32(175) + 100),
  163.67f / (24329.0f/float32(176) + 100),
  163.67f / (24329.0f/float32(177) + 100),
  163.67f / (24329.0f/float32(178) + 100),
  163.67f / (24329.0f/float32(179) + 100),
  163.67f / (24329.0f/float32(180) + 100),
  163.67f / (24329.0f/float32(181) + 100),
  163.67f / (24329.0f/float32(182) + 100),
  163.67f / (24329.0f/float32(183) + 100),
  163.67f / (24329.0f/float32(184) + 100),
  163.67f / (24329.0f/float32(185) + 100),
  163.67f / (24329.0f/float32(186) + 100),
  163.67f / (24329.0f/float32(187) + 100),
  163.67f / (24329.0f/float32(188) + 100),
  163.67f / (24329.0f/float32(189) + 100),
  163.67f / (24329.0f/float32(190) + 100),
  163.67f / (24329.0f/float32(191) + 100),
  163.67f / (24329.0f/float32(192) + 100),
  163.67f / (24329.0f/float32(193) + 100),
  163.67f / (24329.0f/float32(194) + 100),
  163.67f / (24329.0f/float32(195) + 100),
  163.67f / (24329.0f/float32(196) + 100),
  163.67f / (24329.0f/float32(197) + 100),
  163.67f / (24329.0f/float32(198) + 100),
  163.67f / (24329.0f/float32(199) + 100),
  163.67f / (24329.0f/float32(200) + 100),
  163.67f / (24329.0f/float32(201) + 100),
  163.67f / (24329.0f/float32(202) + 100)
};

void APUState::Save(ostream& out) {
  utils::writeUint64(out, cycle);
  utils::writeUint8(out, framePeriod);
  utils::writeUint8(out, frameValue);
  utils::writeBool(out, frameIRQ);
}

void APUState::Load(istream& in) {
  cycle = utils::readUint64(in);
  framePeriod = utils::readUint8(in);
  frameValue = utils::readUint8(in);
  frameIRQ = utils::readBool(in);
}

void APU::Step() {
  uint64 cycle1 = cycle;
  cycle++;
  uint64 cycle2 = cycle;
  stepTimer();
  int32 f1 = int32(float64(cycle1) / frameCounterRate);
  int32 f2 = int32(float64(cycle2) / frameCounterRate);
  if (f1 != f2) {
    stepFrameCounter();
  }
  int32 s1 = int32(float64(cycle1) / sampleRate);
  int32 s2 = int32(float64(cycle2) / sampleRate);
  if (s1 != s2) {
    sendSample();
  }
}

void APU::SetSampleRate(float64 rate) {
  // Convert samples per second to cpu steps per sample
  sampleRate = CPU::Frequency / rate;
  filterChain.SetSampleRate(float32(rate));
}

void APU::sendSample() {
  float32 out = filterChain.Step(output());
  if (channel) {
    channel->Write(out);
  } 
}

float32 APU::output() {
  uint8 p1 = pulse1.output();
  uint8 p2 = pulse2.output();
  uint8 t = triangle.output();
  uint8 n = noise.output();
  uint8 d = dmc.output();
  float32 pulseOut = pulseTable[p1+p2];
  float32 tndOut = tndTable[3*t+2*n+d];
  return pulseOut + tndOut;
}

// mode 0:    mode 1:       function;
// ---------  -----------  -----------------------------;
//  - - - f    - - - - -    IRQ (if bit 6 is clear);
//  - l - l    l - l - -    Length counter and sweep;
//  e e e e    e e e e -    Envelope and linear counter;
void APU::stepFrameCounter() {
  switch (framePeriod) {
  case 4:
    frameValue = (frameValue + 1) % 4;
    switch (frameValue) {
    case 0:
      stepEnvelope();
      break;
    case 1:
      stepEnvelope();
      stepSweep();
      stepLength();
      break;
    case 2:
      stepEnvelope();
      break;
    case 3:
      stepEnvelope();
      stepSweep();
      stepLength();
      fireIRQ();
      break;
    }
    break;
  case 5:
    frameValue = (frameValue + 1) % 5;
    switch (frameValue) {
    case 0:
      stepEnvelope();
      stepSweep();
      stepLength();
      break;
    case 1:
      stepEnvelope();
      break;
    case 2:
      stepEnvelope();
      stepSweep();
      stepLength();
      break;
    case 3:
      stepEnvelope();
      break;
    }
    break;
  }
}

void APU::stepTimer() {
  if (cycle%2 == 0) {
    pulse1.stepTimer();
    pulse2.stepTimer();
    noise.stepTimer();
    dmc.stepTimer();
  }
  triangle.stepTimer();
}

void APU::stepEnvelope() {
  pulse1.stepEnvelope();
  pulse2.stepEnvelope();
  triangle.stepCounter();
  noise.stepEnvelope();
}

void APU::stepSweep() {
  pulse1.stepSweep();
  pulse2.stepSweep();
}

void APU::stepLength() {
  pulse1.stepLength();
  pulse2.stepLength();
  triangle.stepLength();
  noise.stepLength();
}

void APU::fireIRQ() {
  if (frameIRQ) {
    console->cpu->triggerIRQ();
  }
}

uint8 APU::readRegister(uint16 address) {
  switch (address) {
  case 0x4015:
    return readStatus();
  // default:
    // TODO: add logging.
    // log.Fatalf("unhandled apu register read at address: 0x%04X", address);
  }
  return 0;
}

void APU::writeRegister(uint16 address, uint8 value) {
  switch (address) {
  case 0x4000:
    pulse1.writeControl(value);
    break;
  case 0x4001:
    pulse1.writeSweep(value);
    break;
  case 0x4002:
    pulse1.writeTimerLow(value);
    break;
  case 0x4003:
    pulse1.writeTimerHigh(value);
    break;
  case 0x4004:
    pulse2.writeControl(value);
    break;
  case 0x4005:
    pulse2.writeSweep(value);
    break;
  case 0x4006:
    pulse2.writeTimerLow(value);
    break;
  case 0x4007:
    pulse2.writeTimerHigh(value);
    break;
  case 0x4008:
    triangle.writeControl(value);
    break;
  case 0x4009:
    break;
  case 0x4010:
    dmc.writeControl(value);
    break;
  case 0x4011:
    dmc.writeValue(value);
    break;
  case 0x4012:
    dmc.writeAddress(value);
    break;
  case 0x4013:
    dmc.writeLength(value);
    break;
  case 0x400A:
    triangle.writeTimerLow(value);
    break;
  case 0x400B:
    triangle.writeTimerHigh(value);
    break;
  case 0x400C:
    noise.writeControl(value);
    break;
  case 0x400D:
    break;
  case 0x400E:
    noise.writePeriod(value);
    break;
  case 0x400F:
    noise.writeLength(value);
    break;
  case 0x4015:
    writeControl(value);
    break;
  case 0x4017:
    writeFrameCounter(value);
    // default:
    //   log.Fatalf("unhandled apu register write at address: 0x%04X", address);
  }
}

uint8 APU::readStatus() {
  uint8 result = 0;
  if (pulse1.lengthValue > 0) {
    result |= 1;
  }
  if (pulse2.lengthValue > 0) {
    result |= 2;
  }
  if (triangle.lengthValue > 0) {
    result |= 4;
  }
  if (noise.lengthValue > 0) {
    result |= 8;
  }
  if (dmc.currentLength > 0) {
    result |= 16;
  }
  return result;
}

void APU::writeControl(uint8 value) {
  pulse1.enabled = (value&1) == 1;
  pulse2.enabled = (value&2) == 2;
  triangle.enabled = (value&4) == 4;
  noise.enabled = (value&8) == 8;
  dmc.enabled = (value&16) == 16;
  if (!pulse1.enabled) {
    pulse1.lengthValue = 0;
  }
  if (!pulse2.enabled) {
    pulse2.lengthValue = 0;
  }
  if (!triangle.enabled) {
    triangle.lengthValue = 0;
  }
  if (!noise.enabled) {
    noise.lengthValue = 0;
  }
  if (!dmc.enabled) {
    dmc.currentLength = 0;
  } else {
    if (dmc.currentLength == 0) {
      dmc.restart();
    }
  }
}

void APU::writeFrameCounter(uint8 value) {
  framePeriod = 4 + ((value>>7)&1);
  frameIRQ = ((value>>6)&1) == 0;
  // frameValue = 0;
  if (framePeriod == 5) {
    stepEnvelope();
    stepSweep();
    stepLength();
  }
}

// Pulse;

void PulseState::Save(ostream& out) {
  utils::writeBool(out, enabled);
  utils::writeUint8(out, channel);
  utils::writeBool(out, lengthEnabled);
  utils::writeUint8(out, lengthValue);
  utils::writeUint16(out, timerPeriod);
  utils::writeUint16(out, timerValue);
  utils::writeUint8(out, dutyMode);
  utils::writeUint8(out, dutyValue);
  utils::writeBool(out, sweepReload);
  utils::writeBool(out, sweepEnabled);
  utils::writeBool(out, sweepNegate);
  utils::writeUint8(out, sweepShift);
  utils::writeUint8(out, sweepPeriod);
  utils::writeUint8(out, sweepValue);
  utils::writeBool(out, envelopeEnabled);
  utils::writeBool(out, envelopeLoop);
  utils::writeBool(out, envelopeStart);
  utils::writeUint8(out, envelopePeriod);
  utils::writeUint8(out, envelopeValue);
  utils::writeUint8(out, envelopeVolume);
  utils::writeUint8(out, constantVolume);
}

void PulseState::Load(istream& in) {
  enabled = utils::readBool(in);
  channel = utils::readUint8(in);
  lengthEnabled = utils::readBool(in);
  lengthValue = utils::readUint8(in);
  timerPeriod = utils::readUint16(in);
  timerValue = utils::readUint16(in);
  dutyMode = utils::readUint8(in);
  dutyValue = utils::readUint8(in);
  sweepReload = utils::readBool(in);
  sweepEnabled = utils::readBool(in);
  sweepNegate = utils::readBool(in);
  sweepShift = utils::readUint8(in);
  sweepPeriod = utils::readUint8(in);
  sweepValue = utils::readUint8(in);
  envelopeEnabled = utils::readBool(in);
  envelopeLoop = utils::readBool(in);
  envelopeStart = utils::readBool(in);
  envelopePeriod = utils::readUint8(in);
  envelopeValue = utils::readUint8(in);
  envelopeVolume = utils::readUint8(in);
  constantVolume = utils::readUint8(in);
}

void Pulse::writeControl(uint8 value) {
  dutyMode = (value >> 6) & 3;
  lengthEnabled = ((value>>5)&1) == 0;
  envelopeLoop = ((value>>5)&1) == 1;
  envelopeEnabled = ((value>>4)&1) == 0;
  envelopePeriod = value & 15;
  constantVolume = value & 15;
  envelopeStart = true;
}

void Pulse::writeSweep(uint8 value) {
  sweepEnabled = ((value>>7)&1) == 1;
  sweepPeriod = ((value>>4)&7) + 1;
  sweepNegate = ((value>>3)&1) == 1;
  sweepShift = value & 7;
  sweepReload = true;
}

void Pulse::writeTimerLow(uint8 value) {
  timerPeriod = (timerPeriod & 0xFF00) | uint16(value);
}

void Pulse::writeTimerHigh(uint8 value) {
  lengthValue = lengthTable[value>>3];
  timerPeriod = (timerPeriod & 0x00FF) | (uint16(value&7) << 8);
  envelopeStart = true;
  dutyValue = 0;
}

void Pulse::stepTimer() {
  if (timerValue == 0) {
    timerValue = timerPeriod;
    dutyValue = (dutyValue + 1) % 8;
  } else {
    timerValue--;
  }
}

void Pulse::stepEnvelope() {
  if (envelopeStart) {
    envelopeVolume = 15;
    envelopeValue = envelopePeriod;
    envelopeStart = false;
  } else if (envelopeValue > 0) {
    envelopeValue--;
  } else {
    if (envelopeVolume > 0) {
      envelopeVolume--;
    } else if (envelopeLoop) {
      envelopeVolume = 15;
    }
    envelopeValue = envelopePeriod;
  }
}

void Pulse::stepSweep() {
  if (sweepReload) {
    if (sweepEnabled && sweepValue == 0) {
      sweep();
    }
    sweepValue = sweepPeriod;
    sweepReload = false;
  } else if (sweepValue > 0) {
    sweepValue--;
  } else {
    if (sweepEnabled) {
      sweep();
    }
    sweepValue = sweepPeriod;
  }
}

void Pulse::stepLength() {
  if (lengthEnabled && lengthValue > 0) {
    lengthValue--;
  }
}

void Pulse::sweep() {
  uint16 delta = timerPeriod >> sweepShift;
  if (sweepNegate) {
    timerPeriod -= delta;
    if (channel == 1) {
      timerPeriod--;
    }
  } else {
    timerPeriod += delta;
  }
}

uint8 Pulse::output() {
  if (!enabled) {
    return 0;
  }
  if (lengthValue == 0) {
    return 0;
  }
  if (dutyTable[dutyMode][dutyValue] == 0) {
    return 0;
  }
  if (timerPeriod < 8 || timerPeriod > 0x7FF) {
    return 0;
  }
  // if (!sweepNegate && timerPeriod+(timerPeriod>>sweepShift) > 0x7FF) {
  //   return 0;
  // }
  if (envelopeEnabled) {
    return envelopeVolume;
  } else {
    return constantVolume;
  }
}

// Triangle;

void TriangleState::Save(ostream& out) {
  utils::writeBool(out, enabled);
  utils::writeBool(out, lengthEnabled);
  utils::writeUint8(out, lengthValue);
  utils::writeUint16(out, timerPeriod);
  utils::writeUint16(out, timerValue);
  utils::writeUint8(out, dutyValue);
  utils::writeUint8(out, counterPeriod);
  utils::writeUint8(out, counterValue);
  utils::writeBool(out, counterReload);
}

void TriangleState::Load(istream& in) {
  enabled = utils::readBool(in);
  lengthEnabled = utils::readBool(in);
  lengthValue = utils::readUint8(in);
  timerPeriod = utils::readUint16(in);
  timerValue = utils::readUint16(in);
  dutyValue = utils::readUint8(in);
  counterPeriod = utils::readUint8(in);
  counterValue = utils::readUint8(in);
  counterReload = utils::readBool(in);
}

void Triangle::writeControl(uint8 value) {
  lengthEnabled = ((value>>7)&1) == 0;
  counterPeriod = value & 0x7F;
}

void Triangle::writeTimerLow(uint8 value) {
  timerPeriod = (timerPeriod & 0xFF00) | uint16(value);
}

void Triangle::writeTimerHigh(uint8 value) {
  lengthValue = lengthTable[value>>3];
  timerPeriod = (timerPeriod & 0x00FF) | (uint16(value&7) << 8);
  timerValue = timerPeriod;
  counterReload = true;
}

void Triangle::stepTimer() {
  if (timerValue == 0) {
    timerValue = timerPeriod;
    if (lengthValue > 0 && counterValue > 0) {
      dutyValue = (dutyValue + 1) % 32;
    }
  } else {
    timerValue--;
  }
}

void Triangle::stepLength() {
  if (lengthEnabled && lengthValue > 0) {
    lengthValue--;
  }
}

void Triangle::stepCounter() {
  if (counterReload) {
    counterValue = counterPeriod;
  } else if (counterValue > 0) {
    counterValue--;
  }
  if (lengthEnabled) {
    counterReload = false;
  }
}

uint8 Triangle::output() {
  if (!enabled) {
    return 0;
  }
  if (lengthValue == 0) {
    return 0;
  }
  if (counterValue == 0) {
    return 0;
  }
  return triangleTable[dutyValue];
}

// Noise;

void NoiseState::Save(ostream& out) {
  utils::writeBool(out, enabled);
  utils::writeBool(out, mode);
  utils::writeUint16(out, shiftRegister);
  utils::writeBool(out, lengthEnabled);
  utils::writeUint8(out, lengthValue);
  utils::writeUint16(out, timerPeriod);
  utils::writeUint16(out, timerValue);
  utils::writeBool(out, envelopeEnabled);
  utils::writeBool(out, envelopeLoop);
  utils::writeBool(out, envelopeStart);
  utils::writeUint8(out, envelopePeriod);
  utils::writeUint8(out, envelopeValue);
  utils::writeUint8(out, envelopeVolume);
  utils::writeUint8(out, constantVolume);
}

void NoiseState::Load(istream& in) {
  enabled = utils::readBool(in);
  mode = utils::readBool(in);
  shiftRegister = utils::readUint16(in);
  lengthEnabled = utils::readBool(in);
  lengthValue = utils::readUint8(in);
  timerPeriod = utils::readUint16(in);
  timerValue = utils::readUint16(in);
  envelopeEnabled = utils::readBool(in);
  envelopeLoop = utils::readBool(in);
  envelopeStart = utils::readBool(in);
  envelopePeriod = utils::readUint8(in);
  envelopeValue = utils::readUint8(in);
  envelopeVolume = utils::readUint8(in);
  constantVolume = utils::readUint8(in);
}

void Noise::writeControl(uint8 value) {
  lengthEnabled = ((value>>5)&1) == 0;
  envelopeLoop = ((value>>5)&1) == 1;
  envelopeEnabled = ((value>>4)&1) == 0;
  envelopePeriod = value & 15;
  constantVolume = value & 15;
  envelopeStart = true;
}

void Noise::writePeriod(uint8 value) {
  mode = (value&0x80) == 0x80;
  timerPeriod = noiseTable[value&0x0F];
}

void Noise::writeLength(uint8 value) {
  lengthValue = lengthTable[value>>3];
  envelopeStart = true;
}

void Noise::stepTimer() {
  if (timerValue == 0) {
    timerValue = timerPeriod;
    uint8 shift = 0;
    if (mode) {
      shift = 6;
    } else {
      shift = 1;
    }
    uint16 b1 = shiftRegister & 1;
    uint16 b2 = (shiftRegister >> shift) & 1;
    shiftRegister >>= 1;
    shiftRegister |= (b1 ^ b2) << 14;
  } else {
    timerValue--;
  }
}

void Noise::stepEnvelope() {
  if (envelopeStart) {
    envelopeVolume = 15;
    envelopeValue = envelopePeriod;
    envelopeStart = false;
  } else if (envelopeValue > 0) {
    envelopeValue--;
  } else {
    if (envelopeVolume > 0) {
      envelopeVolume--;
    } else if (envelopeLoop) {
      envelopeVolume = 15;
    }
    envelopeValue = envelopePeriod;
  }
}

void Noise::stepLength() {
  if (lengthEnabled && lengthValue > 0) {
    lengthValue--;
  }
}

uint8 Noise::output() {
  if (!enabled) {
    return 0;
  }
  if (lengthValue == 0) {
    return 0;
  }
  if ((shiftRegister&1) == 1) {
    return 0;
  }
  if (envelopeEnabled) {
    return envelopeVolume;
  } else {
    return constantVolume;
  }
}

// DMC;

void DMCState::Save(ostream& out) {
  utils::writeBool(out, enabled);
  utils::writeUint8(out, value);
  utils::writeUint16(out, sampleAddress);
  utils::writeUint16(out, sampleLength);
  utils::writeUint16(out, currentAddress);
  utils::writeUint16(out, currentLength);
  utils::writeUint8(out, shiftRegister);
  utils::writeUint8(out, bitCount);
  utils::writeUint8(out, tickPeriod);
  utils::writeUint8(out, tickValue);
  utils::writeBool(out, loop);
  utils::writeBool(out, irq);
}

void DMCState::Load(istream& in) {
  enabled = utils::readBool(in);
  value = utils::readUint8(in);
  sampleAddress = utils::readUint16(in);
  sampleLength = utils::readUint16(in);
  currentAddress = utils::readUint16(in);
  currentLength = utils::readUint16(in);
  shiftRegister = utils::readUint8(in);
  bitCount = utils::readUint8(in);
  tickPeriod = utils::readUint8(in);
  tickValue = utils::readUint8(in);
  loop = utils::readBool(in);
  irq = utils::readBool(in);
}

void DMC::writeControl(uint8 value) {
  irq = (value&0x80) == 0x80;
  loop = (value&0x40) == 0x40;
  tickPeriod = dmcTable[value&0x0F];
}

void DMC::writeValue(uint8 value) {
  value = value & 0x7F;
}

void DMC::writeAddress(uint8 value) {
  // Sample address = %11AAAAAA.AA000000;
  sampleAddress = 0xC000 | (uint16(value) << 6);
}

void DMC::writeLength(uint8 value) {
  // Sample length = %0000LLLL.LLLL0001;
  sampleLength = (uint16(value) << 4) | 1;
}

void DMC::restart() {
  currentAddress = sampleAddress;
  currentLength = sampleLength;
}

void DMC::stepTimer() {
  if (!enabled) {
    return;
  }
  stepReader();
  if (tickValue == 0) {
    tickValue = tickPeriod;
    stepShifter();
  } else {
    tickValue--;
  }
}

void DMC::stepReader() {
  if (currentLength > 0 && bitCount == 0) {
    console->cpu->stall += 4;
    shiftRegister = console->cpu->Read(currentAddress);
    bitCount = 8;
    currentAddress++;
    if (currentAddress == 0) {
      currentAddress = 0x8000;
    }
    currentLength--;
    if (currentLength == 0 && loop) {
      restart();
    }
  }
}

void DMC::stepShifter() {
  if (bitCount == 0) {
    return;
  }
  if ((shiftRegister&1) == 1) {
    if (value <= 125) {
      value += 2;
    }
  } else {
    if (value >= 2) {
      value -= 2;
    }
  }
  shiftRegister >>= 1;
  bitCount--;
}

uint8 DMC::output() {
  return value;
}

}  // namespace nes;
