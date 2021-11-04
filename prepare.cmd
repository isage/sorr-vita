rem I made an equivalent of prepare.sh for Windows user, therefore not necessary for using WSL.
@echo off
cd data

if not %cd:~-4% == data goto error

del *.dll /f/s/q
del *.exe /f/s/q
del SorMaker.dat /f/s/q

..\tools\extract_stub.py SorR.dat
move /y SorR_stripped.dcb SorR.dat
copy /y ..\system.txt mod\system.txt

goto end

:error
echo Error: You need to run this in the parent folder of "data".

:end
