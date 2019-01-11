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
To alter the code for your purpose, you only have to edit the [MessageHandler class](https://github.com/jackson-nestelroad/key-consumer/blob/master/Keyboard%20Hook/Header/MessageHandler.h) to fit your specific use case.
The current `MessageHandler` supports CTRL commands and intercepts keys based on a random number generator.
To edit the functionality of how messages are handled, you can use the following as a guide to what each function is designed for.
```class MessageHandler {
  public: 
  
    MessageHandler();
      // Sets static member variables to their default values.
      // Runs before keyboard hook message handling begins.
      
    static void handle();
      // Infinite loop that receives messages sent from KeyboardHook::HookCallback().
      // The only message that is currently sent is KILL_HOOK, which is used to remove the keyboard hook and stop the program.
      // You probably don't need to touch this unless you need to alter how the hook ends.
      // Once this loop finishes, the program ends.
      
    static WORD handleKey(DWORD key);
      // Function that is called directly from KeyboardHook::HookCallback() to decide what is done with the key.
      // Current options are EAT_KEY or PASS_KEY.
      
  private:
  
    static BOOL ctrlKey;
      // FALSE by default.
      // Set to TRUE only if CTRL key was just pressed and a CTRL command has been started.
      
    static BOOL interceptKeys;
      // TRUE by default.
      // The MessageHandler will not change anything about the key pressed if this value is set to FALSE.
      
    static WORD keyAction(DWORD key);
      // Decides how key impacts internal code of the class.
      // Keys that start commands (such as CTRL) should be added here.
      // Add cases for keys you want to start a command.
      // Returns NA_KEY if key is not a command key.
      
    static WORD ctrlCommand(DWORD key);
      // Returns the result of initiating a CTRL command.
      // Only runs if ctrlKey is set to TRUE, but ctrlKey is set to FALSE in the function to avoid running more than once.
      // Add cases for the key that follows the CTRL key in your command.
      
    static WORD interceptKey(DWORD key);
      // Intercepts the key however you want.
      // Currently is a simple function that decides whether to return EAT_KEY or PASS_KEY.
```
