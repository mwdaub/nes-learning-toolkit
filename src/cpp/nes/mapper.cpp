#include "mapper.h"

#include "console.h"
#include "mapper1.h"
#include "mapper2.h"
#include "mapper225.h"
#include "mapper3.h"
#include "mapper4.h"
#include "mapper7.h"

namespace nes {

Mapper* Mapper::Create(Console* console) {
  Cartridge* cartridge = console->cartridge;
  switch (cartridge->mapper) {
  case 0:
    return new Mapper2(cartridge);
  case 1:
    return new Mapper1(cartridge);
  case 2:
    return new Mapper2(cartridge);
  case 3:
    return new Mapper3(cartridge);
  case 4:
    return new Mapper4(cartridge, console);
  case 7:
    return new Mapper7(cartridge);
  case 225:
    return new Mapper225(cartridge);
  }
  return 0;
}

MapperState* Mapper::Copy(Mapper* mapper) {
  if (mapper) {
    return mapper->Copy();
  }
  return NULL;
}

}  // namespace nes
