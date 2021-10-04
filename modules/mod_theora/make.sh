#!/bin/sh

DEPS="bennugd-core bennugd-module-mod_video sdl theora vorbis"
MODULE="mod_theora"

# Clean previous module files
if [ -f $MODULE.so ]; then
	rm $MODULE.so
fi

# Compile the source code
gcc -c -Wall $(pkg-config --cflags $DEPS) -I. $MODULE.c theoraplay.c

# Link the module
if [ -f $MODULE.o ]; then
	gcc -olib$MODULE.so $MODULE.o theoraplay.o -shared $(pkg-config --libs $DEPS) -Wl,-soname -Wl,lib$MODULE.so -logg -lvorbis -ltheoradec 
fi

# Strip and remove compilation files
if [ -f lib$MODULE.so ]; then
    rm $MODULE.o
    rm theoraplay.o
    strip lib$MODULE.so
fi

