@echo off
setlocal

set "VICE=C:\Users\nickk\Downloads\GTK3VICE-3.9-win64\GTK3VICE-3.9-win64\bin\x64sc.exe"

if not exist "%VICE%" (
    echo ERROR: VICE not found at:
    echo %VICE%
    pause
    exit /b 1
)

"%VICE%" ^
    -8 "%~dp0assets\pawassets.d64" ^
    -autostartprgmode 1 ^
    -autostart "%~dp0build\pawos.prg"
endlocal