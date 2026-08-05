@echo off
setlocal enabledelayedexpansion
rem GPU build: nvcc for GPUEngine.cu, MSVC for the rest, -DWITHGPU throughout.
rem sm_86 = GA107 (RTX 3050 Laptop). Override with: build_gpu.bat 89

set CCAP=%1
if "%CCAP%"=="" set CCAP=86

rem CUDA 13.x may reject the newest MSVC as an unsupported host compiler.
rem MSVC_DIR pins nvcc to a known-good toolset; leave empty to use the default.
set "MSVC_DIR=C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64"

call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 (echo VCVARS FAILED & exit /b 1)

rem Prefer a local extracted toolkit (redist zips, no admin install), else
rem the system install under Program Files.
set "CUDA_PATH=%~dp0..\cuda-toolkit"
if not exist "%CUDA_PATH%\bin\nvcc.exe" (
  for /f "delims=" %%i in ('dir /b /o-n "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v*" 2^>nul') do (
    if not defined CUDA_VER set CUDA_VER=%%i
  )
  if not defined CUDA_VER (echo CUDA TOOLKIT NOT FOUND & exit /b 1)
  set "CUDA_PATH=C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\!CUDA_VER!"
)
if not exist "%CUDA_PATH%\bin\nvcc.exe" (echo nvcc NOT FOUND under %CUDA_PATH% & exit /b 1)
echo Using "%CUDA_PATH%", sm_%CCAP%

rem separate obj dir: the CPU build left non-WITHGPU objects in obj\
if not exist objgpu mkdir objgpu
del /q objgpu\*.obj 2>nul

set CCBIN=
if exist "%MSVC_DIR%\cl.exe" set CCBIN=-ccbin "%MSVC_DIR%"

"%CUDA_PATH%\bin\nvcc.exe" -maxrregcount=0 --ptxas-options=-v --compile %CCBIN% ^
  -m64 -O2 -I. -I"%CUDA_PATH%\include" -DWITHGPU -DWIN64 -D_CRT_SECURE_NO_WARNINGS ^
  -gencode=arch=compute_%CCAP%,code=sm_%CCAP% ^
  -o objgpu\GPUEngine.obj -c GPU\GPUEngine.cu
if errorlevel 1 (echo NVCC FAILED & exit /b 1)

cl /nologo /EHsc /O2 /std:c++14 /DWITHGPU /DWIN64 /D_CRT_SECURE_NO_WARNINGS ^
  /I. /I"%CUDA_PATH%\include" /c ^
  main.cpp Kangaroo.cpp HashTable.cpp Backup.cpp Thread.cpp Check.cpp ^
  Network.cpp Merge.cpp PartMerge.cpp Timer.cpp ^
  SECPK1\Int.cpp SECPK1\IntGroup.cpp SECPK1\IntMod.cpp ^
  SECPK1\Point.cpp SECPK1\Random.cpp SECPK1\SECP256K1.cpp /Foobjgpu\
if errorlevel 1 (echo COMPILE FAILED & exit /b 1)

link /nologo /OUT:kangaroo-gpu.exe objgpu\*.obj ws2_32.lib advapi32.lib ^
  /LIBPATH:"%CUDA_PATH%\lib\x64" cudart.lib
if errorlevel 1 (echo LINK FAILED & exit /b 1)
echo BUILD OK
