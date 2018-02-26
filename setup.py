from distutils.core import setup, Extension
import glob

sources = ['src/cpp/module.cpp'] + glob.glob('src/cpp/nes/*.cpp')

module1 = Extension('nes',
        sources = sources,
        extra_compile_args=['-std=c++14'])

setup(name = 'NES',
        version = '1.0',
        description = 'Module for emulating NES games in python',
        ext_modules = [module1])
