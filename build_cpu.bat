@echo off
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 (echo VCVARS FAILED & exit /b 1)
if not exist obj mkdir obj
cl /nologo /EHsc /O2 /std:c++14 /DWIN64 /D_CRT_SECURE_NO_WARNINGS /I. /c ^
  main.cpp Kangaroo.cpp HashTable.cpp Backup.cpp Thread.cpp Check.cpp ^
  Network.cpp Merge.cpp PartMerge.cpp Timer.cpp ^
  SECPK1\Int.cpp SECPK1\IntGroup.cpp SECPK1\IntMod.cpp ^
  SECPK1\Point.cpp SECPK1\Random.cpp SECPK1\SECP256K1.cpp /Foobj\
if errorlevel 1 (echo COMPILE FAILED & exit /b 1)
link /nologo /OUT:kangaroo.exe obj\*.obj ws2_32.lib advapi32.lib
if errorlevel 1 (echo LINK FAILED & exit /b 1)
echo BUILD OK
