#include "utils.h"

using namespace std;

namespace nes {

namespace utils {

void read(istream& in, bool& val) {
  char b;
  in.get(b);
  val = b;
}

void write(ostream& out, bool val) {
  out.put(val);
}

void read(istream& in, uint8& val) {
  char b;
  in.get(b);
  val = b;
}

void write(ostream& out, uint8 val) {
  out.put(val);
}

void read(istream& in, int8& val) {
  char b;
  in.get(b);
  val = b;
}

void write(ostream& out, int8 val) {
  out.put(val);
}

void read(istream& in, uint16& val) {
  char buf[2];
  in.read(&buf[0], 2);
  val = (uint16(buf[1]) << 8) | buf[0];
}

void write(ostream& out, uint16 val) {
  out.put(val);
  out.put(val >> 8);
}

void read(istream& in, int16& val) {
  char buf[2];
  in.read(&buf[0], 2);
  val = (int16(buf[1]) << 8) | buf[0];
}

void write(ostream& out, int16 val) {
  out.put(val);
  out.put(val >> 8);
}

void read(istream& in, uint32& val) {
  char buf[4];
  in.read(&buf[0], 4);
  val = (uint32(buf[3]) << 24) | (uint32(buf[2]) << 16) | (uint32(buf[1]) << 8) | buf[0];
}

void write(ostream& out, uint32 val) {
  out.put(val);
  out.put(val >> 8);
  out.put(val >> 16);
  out.put(val >> 24);
}

void read(istream& in, int32& val) {
  char buf[4];
  in.read(&buf[0], 4);
  val = (int32(buf[3]) << 24) | (int32(buf[2]) << 16) | (int32(buf[1]) << 8) | buf[0];
}

void write(ostream& out, int32 val) {
  out.put(val);
  out.put(val >> 8);
  out.put(val >> 16);
  out.put(val >> 24);
}

void read(istream& in, uint64& val) {
  char buf[8];
  in.read(&buf[0], 8);
  val = (uint64(buf[7]) << 56) | (uint64(buf[6]) << 48) | (uint64(buf[5]) << 40) | (uint64(buf[4]) << 32) | (uint64(buf[3]) << 24) | (uint64(buf[2]) << 16) | (uint64(buf[1]) << 8) | buf[0];
}

void write(ostream& out, uint64 val) {
  out.put(val);
  out.put(val >> 8);
  out.put(val >> 16);
  out.put(val >> 24);
  out.put(val >> 32);
  out.put(val >> 40);
  out.put(val >> 48);
  out.put(val >> 56);
}

void read(istream& in, int64& val) {
  char buf[8];
  in.read(&buf[0], 8);
  val = (int64(buf[7]) << 56) | (int64(buf[6]) << 48) | (int64(buf[5]) << 40) | (int64(buf[4]) << 32) | (int64(buf[3]) << 24) | (int64(buf[2]) << 16) | (int64(buf[1]) << 8) | buf[0];
}

void write(ostream& out, int64 val) {
  out.put(val);
  out.put(val >> 8);
  out.put(val >> 16);
  out.put(val >> 24);
  out.put(val >> 32);
  out.put(val >> 40);
  out.put(val >> 48);
  out.put(val >> 56);
}

void read(istream& in, char* val, uint32 length) {
  char* buf = new char[length];
  in.read(buf, length);
  memcpy(val, buf, length);
  delete[] buf;
  cout << length << " bytes read." << endl;
}

void write(ostream& out, char* val, uint32 length) {
  out.write(val, length);
}

}  // namespace utils

}  // namespace nes
