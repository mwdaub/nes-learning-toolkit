#ifndef NES_UTILS_H
#define NES_UTILS_H

#include <array>
#include <iostream>
#include <iterator>
#include <cstring>
#include <vector>

#include "types.h"

using namespace std;

using std::array;
using std::iterator;
using std::vector;

namespace nes {

namespace utils {

bool readBool(istream& in);
void writeBool(ostream& out, bool val);
uint8 readUint8(istream& in);
void writeUint8(ostream& out, uint8 val);
int8 readInt8(istream& in);
void writeInt8(ostream& out, int8 val);
uint16 readUint16(istream& in);
void writeUint16(ostream& out, uint16 val);
int16 readInt16(istream& in);
void writeInt16(ostream& out, int16 val);
uint32 readUint32(istream& in);
void writeUint32(ostream& out, uint32 val);
int32 readInt32(istream& in);
void writeInt32(ostream& out, int32 val);
float32 readFloat32(istream& in);
void writeFloat32(ostream& out, float32 val);
uint64 readUint64(istream& in);
void writeUint64(ostream& out, uint64 val);
int64 readInt64(istream& in);
void writeInt64(ostream& out, int64 val);

template<size_t N> inline void readUint8Array(istream& in, array<uint8, N>& val) {
  for (auto& v : val) {
    v = readUint8(in);
  }
}
template<size_t N> inline void writeUint8Array(ostream& out, array<uint8, N>& val) {
  for (auto v : val) {
    writeUint8(out, v);
  }
}
inline void readUint8Vector(istream& in, vector<uint8>& val) {
  for (auto& v : val) {
    v = readUint8(in);
  }
}
inline void writeUint8Vector(ostream& out, vector<uint8>& val) {
  for (auto v : val) {
    writeUint8(out, v);
  }
}
template<size_t N> inline void readUint32Array(istream& in, array<uint32, N>& val) {
  for (auto& v : val) {
    v = readUint32(in);
  }
}
template<size_t N> inline void writeUint32Array(ostream& out, array<uint32, N>& val) {
  for (auto v : val) {
    writeUint32(out, v);
  }
}
template<size_t N> inline void readInt32Array(istream& in, array<int32, N>& val) {
  for (auto& v : val) {
    v = readInt32(in);
  }
}
template<size_t N> inline void writeInt32Array(ostream& out, array<int32, N>& val) {
  for (auto v : val) {
    writeInt32(out, v);
  }
}
inline void readFloat32Vector(istream& in, vector<float32>& val) {
  for (auto& v : val) {
    v = readFloat32(in);
  }
}
inline void writeFloat32Vector(ostream& out, vector<float32>& val) {
  for (auto v : val) {
    writeFloat32(out, v);
  }
}

}  // namespace utils

}  // namespace nes

#endif  // define NES_UTILS_H
