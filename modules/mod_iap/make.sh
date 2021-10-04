#!/bin/sh

DEPS="bennugd-core sdl libcurl"
MODULE="mod_curl"

# Clean previous module files
if [ -f $MODULE.so ]; then
	rm $MODULE.so
fi

# Compile the source code
gcc -c -Wall $(pkg-config --cflags $DEPS) $MODULE.c

# Link the module
if [ -f $MODULE.o ]; then
	gcc -o$MODULE.so $MODULE.o -shared $(pkg-config --libs $DEPS) -Wl,-soname -Wl,$MODULE.so
fi

# Strip and remove compilation files
if [ -f $MODULE.so ]; then
	rm $MODULE.o
	strip $MODULE.so
fi

