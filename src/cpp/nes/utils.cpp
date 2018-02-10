#include "utils.h"

using namespace std;

namespace nes {

namespace utils {

bool readBool(istream& in) {
  char b;
  in.get(b);
  return bool(b);
}

void writeBool(ostream& out, bool val) {
  out.put(val);
}

uint8 readUint8(istream& in) {
  char b;
  in.get(b);
  return uint8(b);
}

void writeUint8(ostream& out, uint8 val) {
  out.put(val);
}

int8 readInt8(istream& in) {
  char b;
  in.get(b);
  return int8(b);
}

void writeInt8(ostream& out, int8 val) {
  out.put(val);
}

uint16 readUint16(istream& in) {
  uint8 v1 = readUint8(in);
  uint8 v2 = readUint8(in);
  return (uint16(v2) << 8) | v1;
}

void writeUint16(ostream& out, uint16 val) {
  writeUint8(out, uint8(val));
  writeUint8(out, uint8(val >> 8));
}

int16 readInt16(istream& in) {
  uint16 v = readUint16(in);
  return int16(v);
}

void writeInt16(ostream& out, int16 val) {
  writeUint16(out, uint16(val));
}

uint32 readUint32(istream& in) {
  uint16 v1 = readUint16(in);
  uint16 v2 = readUint16(in);
  return (uint32(v2) << 16) | v1;
}

void writeUint32(ostream& out, uint32 val) {
  writeUint16(out, uint16(val));
  writeUint16(out, uint16(val >> 16));
}

int32 readInt32(istream& in) {
  uint32 v = readUint32(in);
  return int32(v);
}

void writeInt32(ostream& out, int32 val) {
  writeUint32(out, uint32(val));
}

uint64 readUint64(istream& in) {
  uint32 v1 = readUint32(in);
  uint32 v2 = readUint32(in);
  return (uint64(v2) << 32) | v1;
}

void writeUint64(ostream& out, uint64 val) {
  writeUint32(out, uint32(val));
  writeUint32(out, uint32(val >> 32));
}

int64 readInt64(istream& in) {
  uint64 v = readUint64(in);
  return int64(v);
}

void writeInt64(ostream& out, int64 val) {
  writeUint64(out, uint64(val));
}

void readUint8Array(istream& in, uint8* val, uint32 length) {
  for (uint32 i = 0; i < length; i++) {
    *val = readUint8(in);
    val++;
  }
}

void writeUint8Array(ostream& out, uint8* val, uint32 length) {
  for (uint32 i = 0; i < length; i++) {
    writeUint8(out, *val);
    val++;
  }
}

void readUint32Array(istream& in, uint32* val, uint32 length) {
  for (uint32 i = 0; i < length; i++) {
    *val = readUint32(in);
    val++;
  }
}

void writeUint32Array(ostream& out, uint32* val, uint32 length) {
  for (uint32 i = 0; i < length; i++) {
    writeUint32(out, *val);
    val++;
  }
}

void readInt32Array(istream& in, int32* val, uint32 length) {
  for (uint32 i = 0; i < length; i++) {
    *val = readInt32(in);
    val++;
  }
}

void writeInt32Array(ostream& out, int32* val, uint32 length) {
  for (uint32 i = 0; i < length; i++) {
    writeInt32(out, *val);
    val++;
  }
}


}  // namespace utils

}  // namespace nes
