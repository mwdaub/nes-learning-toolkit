import nes
import numpy as np

def main():
    emu = nes.Emulator('/home/mwdaub/Super_Mario_Bros.nes')
    emu.emulate(120)
    emu.set_input(8)
    emu.emulate(10)
    emu.set_input(0)
    emu.emulate(200)
    pixels = emu.get_pixel_values()
    expected = np.fromfile('expected.arr', dtype=np.uint8).reshape([240,256,3])
    if np.array_equal(pixels, expected):
        print "Pass!"
    else:
        print "Fail..."

if __name__ == "__main__":
    main()
