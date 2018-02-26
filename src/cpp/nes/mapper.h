#ifndef NES_MAPPER_H
#define NES_MAPPER_H

#include <iostream>
#include <memory>

#include "types.h"

using std::istream;
using std::ostream;
using std::unique_ptr;

namespace nes {

class Console;

class MapperState {
  public:
    virtual void Save(ostream& out) = 0;
    virtual void Load(istream& in) = 0;
};

class Mapper : virtual public MapperState {
  public:
    virtual ~Mapper() {};

    virtual void Step() = 0;
    virtual uint8 Read(uint16 address) = 0;
    virtual void Write(uint16 address, uint8 value) = 0;
    virtual unique_ptr<MapperState> Copy() = 0;

    static unique_ptr<Mapper> Create(Console* console);
    static unique_ptr<MapperState> Copy(Mapper* mapper);
};

}  // namespace nes

#endif  // define NES_MAPPER_H
