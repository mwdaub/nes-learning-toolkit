#include "state.h"

namespace nes {

void State::Save(ostream& out) {
  consoleState.Save(out);
  cpuState.Save(out);
  apuState.Save(out);
  pulseState1.Save(out);
  pulseState2.Save(out);
  triangleState.Save(out);
  noiseState.Save(out);
  dmcState.Save(out);
  ppuState.Save(out);
  cartridgeState.Save(out);
  mapperState->Save(out);
}

void State::Load(istream& out) {
  consoleState.Load(out);
  cpuState.Load(out);
  apuState.Load(out);
  pulseState1.Load(out);
  pulseState2.Load(out);
  triangleState.Load(out);
  noiseState.Load(out);
  dmcState.Load(out);
  ppuState.Load(out);
  cartridgeState.Load(out);
  mapperState = MapperState::Create(cartridgeState.mapper);
  mapperState->Load(out);
}

}  // namespace nes
