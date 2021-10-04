#!/bin/sh

DEPS="bennugd-core bennugd-module-mod_video sdl sdl_mixer"
MODULE="mod_theora"

# Clean previous module files
if [ -f $MODULE.dylib ]; then
	rm $MODULE.dylib
fi

# Compile the source code
llvm-gcc -O2 -m32 -c -Wall -I../bennugd/base/core/include/ -I../bennugd/base/modules/libgrbase/ -I../bennugd/base/modules/libvideo/ -I../bennugd/base/modules/libblit/ -I. -I/opt/local/include -I/sw/include/SDL/ -I/opt/local/include/SDL $MODULE.c theoraplay.c

# Link the module
if [ -f $MODULE.o ]; then
	llvm-gcc -O2 -m32 -o $MODULE.dylib $MODULE.o theoraplay.o -dynamiclib -L../bennugd_bin/lib/ -lrender -lvideo -lblit -lmod_sound -lgrbase -lbgdrtm -ltheora -L/opt/local/lib  -logg -lvorbis -ltheoradec -Wl,-framework,AppKit -lSDL -Wl,-framework,Cocoa -lSDL_mixer
fi

# Strip and remove compilation files
if [ -f $MODULE.dylib ]; then
	rm $MODULE.o
    rm theoraplay.o
    strip $MODULE.dylib
fi

