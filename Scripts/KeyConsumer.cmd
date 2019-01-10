@ECHO OFF

REM This script allows you to call the .exe without specifying the long DLL path

REM Check if DLL exited properly
REM tasklist /m "Keyboard Hook.dll"

IF "%~2" == "" (
    GOTO :AUTO
)

CALL "Key Consumer.exe" %1 "%~2"
GOTO :END

:AUTO 
CALL "Key Consumer.exe" %1 "%~dp0Keyboard Hook.dll"

:END