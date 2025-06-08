@echo off
setlocal ENABLEEXTENSIONS

:: === PRÜFUNG ===
if not exist "C:\msys64\mingw64\bin\g++.exe" (
    echo [FEHLER] MSYS2 mit MinGW64 fehlt. Bitte installieren.
    exit /b 1
)

:: === ENV SETZEN ===
set "PATH=C:\msys64\mingw64\bin;%PATH%"
set "CC=gcc"
set "CXX=g++"

:: === BUILD ===
if not exist build mkdir build
cd build
cmake .. -G "MinGW Makefiles"
if errorlevel 1 exit /b 1

mingw32-make
if errorlevel 1 exit /b 1
cd ..

:: === DIST-ORDNER ===
set "DISTDIR=dist"
if exist %DISTDIR% rmdir /s /q %DISTDIR%
mkdir %DISTDIR%

:: === EXE KOPIEREN (Name anpassen) ===
copy build\*.exe %DISTDIR%

:: === BENÖTIGTE DLLs KOPIEREN ===
for %%F in (
    libstdc++-6.dll
    libgcc_s_seh-1.dll
    libwinpthread-1.dll
	libcddb-2.dll
	libcdio-19.dll
	libsystre-0.dll
	libtre-5.dll
) do (
    if exist C:\msys64\mingw64\bin\%%F copy C:\msys64\mingw64\bin\%%F %DISTDIR%
)

echo [OK] Distribution liegt in "%DISTDIR%"
exit /b 0
