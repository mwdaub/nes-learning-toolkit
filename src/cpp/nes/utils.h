#ifndef NES_UTILS_H
#define NES_UTILS_H

#include <iostream>
#include <cstring>

#include "types.h"

using namespace std;

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
uint64 readUint64(istream& in);
void writeUint64(ostream& out, uint64 val);
int64 readInt64(istream& in);
void writeInt64(ostream& out, int64 val);

void readUint8Array(istream& in, uint8* val, uint32 length);
void writeUint8Array(ostream& out, uint8* val, uint32 length);
void readUint32Array(istream& in, uint32* val, uint32 length);
void writeUint32Array(ostream& out, uint32* val, uint32 length);
void readInt32Array(istream& in, int32* val, uint32 length);
void writeInt32Array(ostream& out, int32* val, uint32 length);


}  // namespace utils

}  // namespace nes

#endif  // define NES_UTILS_H
