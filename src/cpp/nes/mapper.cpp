#include "mapper.h"

#include "console.h"
#include "mapper1.h"
#include "mapper2.h"
#include "mapper225.h"
#include "mapper3.h"
#include "mapper4.h"
#include "mapper7.h"

using std::make_unique;

namespace nes {

unique_ptr<Mapper> Mapper::Create(Console* console) {
  shared_ptr<Cartridge> cartridge = console->cartridge;
  switch (cartridge->mapper) {
  case 0:
    return make_unique<Mapper2>(cartridge);
  case 1:
    return make_unique<Mapper1>(cartridge);
  case 2:
    return make_unique<Mapper2>(cartridge);
  case 3:
    return make_unique<Mapper3>(cartridge);
  case 4:
    return make_unique<Mapper4>(cartridge, console);
  case 7:
    return make_unique<Mapper7>(cartridge);
  case 225:
    return make_unique<Mapper225>(cartridge);
  }
  return 0;
}

unique_ptr<MapperState> Mapper::Copy(Mapper* mapper) {
  if (mapper) {
    return mapper->Copy();
  }
  return NULL;
}

}  // namespace nes
