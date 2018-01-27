#ifndef NES_MAPPER_H
#define NES_MAPPER_H

#include <iostream>

#include "types.h"

using namespace std;

namespace nes {

class Console;

class Mapper {
  public:
    virtual ~Mapper() {};

    virtual uint8 Read(uint16 address) = 0;
    virtual void Write(uint16 address, uint8 value) = 0;
    virtual void Step() = 0;
    virtual void Save(ostream& out) = 0;
    virtual void Load(istream& in) = 0;

    static Mapper* create(Console* console);
};

}  // namespace nes

#endif  // define NES_MAPPER_H
