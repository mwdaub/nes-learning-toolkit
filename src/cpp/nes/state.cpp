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

}  // namespace nes
