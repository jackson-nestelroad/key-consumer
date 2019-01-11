# Key Consumer
**Key Consumer** is a Windows command to attach a *low-level keyboard hook* to another running process. 
It is built using **C++** and the `<Windows.h>` header file.

## Aspects
`Keyboard Hook.dll` - Dynamic-link library that attaches and manages the low-level keyboard hook.

`Key Consumer.exe` - Process that injects the DLL file into another process.

`KeyConsumer.cmd` - Bash script to simplify calling the process from the command line.

All three of these files should be placed in the same folder to work as intended. The DLL may live in a separate folder, but the batch script and the executable file can only be separated if the batch script is modified.

## Calling
`Key Consumer.exe` must be called with the following parameters from the command line:
```
"Key Consumer.exe" [Process name.exe] [Absolute path to DLL]
```
Since grabbing the absolute path to the DLL can be a hassle, you may put the script `KeyConsumer.cmd` into the same folder and now call:
```
KeyConsumer [Process name.exe] (Absolute path to DLL)
```
The batch script automatically checks its residing folder for the DLL and builds a path from there if the parameter is not included.
If the DLL must exist in a different folder, the absolute path to the DLL must be included with the batch script.

For example, `KeyConsumer notepad.exe` will look for `Keyboard Hook.dll` in its residing folder and pass it to `Key Consumer.exe`, injecting the keyboard hook into `notepad.exe`.

## Purpose
Key Consumer can be used for a variety of purposes, and it is set up to be quite flexible. 

The current `MessageHandler` supports the following CTRL commands:
* `CTRL + "~"` - Destroy the hook permanently.
* `CTRL + "-"` - Turns off all key interception.
* `CTRL + "+"` - Turns on key input interception.
* `CTRL + "0"` - Turns on key input changing.

To edit the functionality of how messages are handled, you only have to edit the [MessageHandler class](https://github.com/jackson-nestelroad/key-consumer/blob/master/Keyboard%20Hook/Header/MessageHandler.h) to fit your specific use case.
