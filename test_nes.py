import nes
import numpy as np
import time

def test_emulate():
    emu = nes.Emulator('/Users/michaeldaub/Downloads/Super_Mario_Bros.nes')
    emu.emulate(120)
    emu.set_input(8)
    emu.emulate(10)
    emu.set_input(0)
    emu.emulate(200)
    pixels = emu.get_pixel_values()
    expected = np.fromfile('expected.arr', dtype=np.uint8).reshape([240,256,3])
    np.testing.assert_array_equal(pixels, expected)

def test_save_state():
    emu = nes.Emulator('/Users/michaeldaub/Downloads/Super_Mario_Bros.nes')
    emu.emulate(120)
    emu.set_input(8)
    emu.emulate(10)
    emu.set_input(0)
    emu.emulate(200)
    emu.save_state('mario.savestate')
    emu1 = nes.Emulator('/Users/michaeldaub/Downloads/Super_Mario_Bros.nes')
    emu1.load_state('mario.savestate')
    emu.emulate(200)
    emu1.emulate(200)
    pixels = emu.get_pixel_values()
    pixels1 = emu1.get_pixel_values()
    np.testing.assert_array_equal(pixels, pixels1)
    
