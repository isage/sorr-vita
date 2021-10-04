#!/bin/bash
find data/ -type f -name "*.dll" -exec rm -f {} \;
find data/ -type f -name "*.exe" -exec rm -f {} \;
find data/ -type f -name "SorMaker.dat" -exec rm -f {} \;
cd data
../tools/extract_stub.py SorR.dat
mv SorR_stripped.dcb SorR.dat
cp ../system.txt mod/system.txt
cd ..

