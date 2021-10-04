#!/bin/sh

gcc -O2 -Wall -I../../core/include/ -I/usr/local/include/ -I../../core/bgdrtm/include/ -D_REENTRANT -c mod_iap_fake.c
if [ -f mod_iap_fake.o ]; then
        gcc mod_iap_fake.o -O2 -shared -o mod_iap.dll -L../../../../bennugd_bin/ -lbgdrtm -Wl,-soname -Wl,mod_vlc.dll
fi

if [ -f mod_iap.dll ]; then
        rm mod_iap_fake.o
fi
