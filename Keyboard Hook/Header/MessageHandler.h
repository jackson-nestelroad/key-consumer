// Definition file MessageHandler class
#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H
#include <Windows.h>
#include <cstdlib>
#include <ctime>

#define KILL_HOOK -1
#define EAT_KEY 0
#define PASS_KEY 1
#define KILL_KEY 2
#define NA_KEY 3

class MessageHandler
{
   private:
      static BOOL ctrlKey;
      static BOOL interceptKeys;
      static WORD keyAction(DWORD key);
      static WORD ctrlCommand(DWORD key);
      static WORD interceptKey(DWORD key);
   public:
      MessageHandler();
      static void handle();
      static WORD handleKey(DWORD key);
};

#endif