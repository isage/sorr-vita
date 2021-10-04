#!/bin/sh

gcc -O2 -m32 -Wall -I../../core/include/ -I/usr/local/include/ -I../../core/bgdrtm/include/ -I/opt/local/include/SDL/ -D_REENTRANT -c mod_iap_fake.c
if [ -f mod_iap_fake.o ]; then
        gcc mod_iap_fake.o -O2 -dynamiclib -m32 -o mod_iap.dylib -L../../../../bennugd_bin/lib/ -lbgdrtm -L/opt/local/lib -install_name mod_iap.dylib -Wl,-single_module
fi

if [ -f mod_iap.dylib ]; then
        rm mod_iap_fake.o
fi
