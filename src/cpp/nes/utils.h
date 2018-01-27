#ifndef NES_UTILS_H
#define NES_UTILS_H

#include <iostream>
#include <cstring>

#include "types.h"

using namespace std;

namespace nes {

namespace utils {

void read(istream& in, bool& val);
void write(ostream& out, bool val);
void read(istream& in, uint8& val);
void write(ostream& out, uint8 val);
void read(istream& in, int8& val);
void write(ostream& out, int8 val);
void read(istream& in, uint16& val);
void write(ostream& out, uint16 val);
void read(istream& in, int16& val);
void write(ostream& out, int16 val);
void read(istream& in, uint32& val);
void write(ostream& out, uint32 val);
void read(istream& in, int32& val);
void write(ostream& out, int32 val);
void read(istream& in, uint64& val);
void write(ostream& out, uint64 val);
void read(istream& in, int64& val);
void write(ostream& out, int64 val);
void read(istream& in, char* val, uint32 length);
void write(ostream& out, char* val, uint32 length);

}  // namespace utils

}  // namespace nes

#endif  // define NES_UTILS_H
