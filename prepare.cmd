rem I made an equivalent of prepare.sh for Windows user, therefore not necessary for using WSL.

del *.dll /f/s/q
del *.exe /f/s/q
del SorMaker.dat /f/s/q

cd data
..\tools\extract_stub.py SorR.dat
move /y SorR_stripped.dcb SorR.dat
copy /y ..\system.txt mod\system.txt
