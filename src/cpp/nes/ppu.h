#ifndef NES_PPU_H
#define NES_PPU_H

#include <iostream>
#include <utility>

#include "types.h"
#include "memory.h"
#include "palette.h"
#include "utils.h"

using namespace std;

namespace nes {

class Console;

class Screen {
  public:
    Screen() : pixels{} {};

    uint8 pixels[4*240*256];

    void SetRGBA(int32 x, int32 y, RGBA* rgba) {
      int i = (y<<10) + (x<<2);
      pixels[i] = rgba->R;
      pixels[i+1] = rgba->G;
      pixels[i+2] = rgba->B;
      pixels[i+3] = rgba->A;
    };

    void Save(ostream& out) {
      cout << "Writing " << sizeof(pixels) << "..." << endl;
      utils::write(out, reinterpret_cast<char*>(&pixels[0]), sizeof(pixels));
    }
};

class PPU {
  public:
    PPU(Console* console) :
        console(console),
        cycle(0),
        scanline(0),
        frame(0),
        paletteData{},
        nameTableData{},
        oamData{},
        v(0),
        t(0),
        x(0),
        w(0),
        f(0),
        reg(0),
        nmiOccurred(false),
        nmiOutput(false),
        nmiPrevious(false),
        nmiDelay(0),
        nameTableByte(0),
        attributeTableByte(0),
        lowTileByte(0),
        highTileByte(0),
        tileData(0),
        spriteCount(0),
        spritePatterns{},
        spritePositions{},
        spritePriorities{},
        spriteIndexes{},
        flagNameTable(0),
        flagIncrement(0),
        flagSpriteTable(0),
        flagBackgroundTable(0),
        flagSpriteSize(0),
        flagMasterSlave(0),
        flagGrayscale(0),
        flagShowLeftBackground(0),
        flagShowLeftSprites(0),
        flagShowBackground(0),
        flagShowSprites(0),
        flagRedTint(0),
        flagGreenTint(0),
        flagBlueTint(0),
        flagSpriteZeroHit(0),
        flagSpriteOverflow(0),
        oamAddress(0),
        bufferedData(0) {
      front = new Screen();
      back = new Screen();
      Reset();
    };

    ~PPU() {
      delete front;
      front = NULL;
      delete back;
      back = NULL;
    }

    Console* console; // parent Console

    int32 cycle;    // 0-340
    int32 scanline; // 0-261, 0-239=visible, 240=post, 241-260=vblank, 261=pre
    uint64 frame;   // frame counter

    // storage variables
    uint8 paletteData[32];
    uint8 nameTableData[2048];
    uint8 oamData[256];
    Screen* front;
    Screen* back;

    // PPU registers
    uint16 v; // current vram address (15 bit)
    uint16 t; // temporary vram address (15 bit)
    uint8 x;  // fine x scroll (3 bit)
    uint8 w;  // write toggle (1 bit)
    uint8 f;  // even/odd frame flag (1 bit)

    uint8 reg;

    // NMI flags
    bool nmiOccurred;
    bool nmiOutput;
    bool nmiPrevious;
    uint8 nmiDelay;

    // background temporary variables
    uint8 nameTableByte;
    uint8 attributeTableByte;
    uint8 lowTileByte;
    uint8 highTileByte;
    uint64 tileData;

    // sprite temporary variables
    int spriteCount;
    uint32 spritePatterns[8];
    uint8 spritePositions[8];
    uint8 spritePriorities[8];
    uint8 spriteIndexes[8];

    // $2000 PPUCTRL
    uint8 flagNameTable;       // 0: $2000; 1: $2400; 2: $2800; 3: $2C00
    uint8 flagIncrement;       // 0: add 1; 1: add 32
    uint8 flagSpriteTable;     // 0: $0000; 1: $1000; ignored in 8x16 mode
    uint8 flagBackgroundTable; // 0: $0000; 1: $1000
    uint8 flagSpriteSize;      // 0: 8x8; 1: 8x16
    uint8 flagMasterSlave;     // 0: read EXT; 1: write EXT

    // $2001 PPUMASK
    uint8 flagGrayscale;          // 0: color; 1: grayscale
    uint8 flagShowLeftBackground; // 0: hide; 1: show
    uint8 flagShowLeftSprites;    // 0: hide; 1: show
    uint8 flagShowBackground;     // 0: hide; 1: show
    uint8 flagShowSprites;        // 0: hide; 1: show
    uint8 flagRedTint;            // 0: normal; 1: emphasized
    uint8 flagGreenTint;          // 0: normal; 1: emphasized
    uint8 flagBlueTint;           // 0: normal; 1: emphasized

    // $2002 PPUSTATUS
    uint8 flagSpriteZeroHit;
    uint8 flagSpriteOverflow;

    // $2003 OAMADDR
    uint8 oamAddress;

    // $2007 PPUDATA
    uint8 bufferedData; // for buffered reads

    void Save(ostream& out);
    void Load(istream& in);
    uint8 Read(uint16 address);
    void Write(uint16 address, uint8 val);
    void Reset();
    void Step();

    uint8 readPalette(uint16 address);
    void writePalette(uint16 address, uint8 value);
    uint8 readRegister(uint16 address);
    void writeRegister(uint16 address, uint8 value);
    void writeControl(uint8 value);
    void writeMask(uint8 value);
    uint8 readStatus();
    void writeOAMAddress(uint8 value);
    uint8 readOAMData();
    void writeOAMData(uint8 value);
    void writeScroll(uint8 value);
    void writeAddress(uint8 value);
    uint8 readData();
    void writeData(uint8 value);
    void writeDMA(uint8 value);
    void incrementX();
    void incrementY();
    void copyX();
    void copyY();
    void nmiChange();
    void setVerticalBlank();
    void clearVerticalBlank();
    void fetchNameTableByte();
    void fetchAttributeTableByte();
    void fetchLowTileByte();
    void fetchHighTileByte();
    void storeTileData();
    uint32 fetchTileData();
    uint8 backgroundPixel();
    void spritePixel(pair<uint8, uint8>* pixels);
    void renderPixel();
    uint32 fetchSpritePattern(int32 i, int32 row);
    void evaluateSprites();
    void tick();
};



}  // namespace nes

#endif  // define NES_PPU_H
