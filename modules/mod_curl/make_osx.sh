#!/bin/sh

gcc -O2 -m32 -Wall -I../../core/include/ -I/usr/local/include/ -I../../core/bgdrtm/include/ -I/opt/local/include/SDL/ -D_REENTRANT -c mod_curl.c
if [ -f mod_curl.o ]; then
        gcc mod_curl.o -O2 -dynamiclib -m32 -o mod_curl.dylib -L../../../../bennugd_bin/lib/ -lbgdrtm -lcurl -L/opt/local/lib -lSDL -install_name mod_curl.dylib -Wl,-single_module
fi

if [ -f mod_curl.dylib ]; then
        rm mod_curl.o
fi
