// Definition file MessageHandler class
#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H
#include <Windows.h>
#include <cstdlib>
#include <ctime>

// List of responses to the KeyboardHook
#define KILL_HOOK -1
#define EAT_KEY 0
#define PASS_KEY 1
#define KILL_KEY 2
#define NA_KEY 3
#define CHANGE_KEY 4

// List of ways to intercept the key
#define KB_NONE 0
#define KB_INTERCEPT 1
#define KB_CHANGE 2

// Bundled response 
struct HMESSAGE 
{
   WORD wCode;       // What to do with the message
   DWORD vkCode;     // Virtual key code
};

class MessageHandler
{
   private:
      static BOOL ctrlKey;
         // Flag that is TRUE when next key is part of a CTRL command
         // Causes the ctrlCommand() function to run

      static DWORD interceptKeys;
         // Flag to specify how or if we are intercepting keyboard inputs
         // Can be set to one of the KB_ flags
            // KB_NONE turns all interception off
            // KB_INTERCEPT causes interceptKey() to run
            // KB_CHANGE causes changeKey() to run

      static HMESSAGE keyAction(DWORD key);
         // Checks how the key input changes the MessageHandler's internal code
         // Keys that start commands (such as CTRL) should be run here
         // Returns an HMESSAGE with NA_KEY for non-special keys

      static HMESSAGE ctrlCommand(DWORD key);
         // Runs a CTRL command with the specified key
         // Used to change the interceptKeys flag

      static HMESSAGE interceptKey(DWORD key);
         // Used to intercept keys completely
         // Returns an HMESSAGE with PASS_KEY or EAT_KEY

      static HMESSAGE changeKey(DWORD key);
         // Used to change keyboard input
         // Returns an HMESSAGE with PASS_KEY or CHANGE_KEY

   public:
      MessageHandler();
         // Sets up the default values for the internal member variables
         // Runs before any message handling begins

      static void handle();
         // Starts up the message handler
         // Infinite loop that dispatches messages to their desired location
         // Receives KILL_HOOK from KeyboardHook::HookCallback() to break out of the loop

      static HMESSAGE handleKey(DWORD key);
         // Function called directly from KeyboardHook::HookCallback()
         // Returns the final result of what should be done with the key
};

#endif