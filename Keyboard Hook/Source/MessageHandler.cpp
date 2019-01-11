// Implementation file for MessageHandler class
#include "../Header/MessageHandler.h"
#include "../Header/KeyboardHook.h"
BOOL MessageHandler::ctrlKey;
DWORD MessageHandler::interceptKeys;

// Constructor
MessageHandler::MessageHandler()
{
   ctrlKey = FALSE;
   interceptKeys = KB_INTERCEPT;
   srand((unsigned int)time(NULL));
}

// Infinite loop to run the loop
void MessageHandler::handle()
{
   MSG msg;

   // We can send messages here if we want from KeyboardHook::HookCallBack using ::PostMessage
   while (::GetMessage(&msg, KeyboardHook::m_hWnd, 0, 0))
   {
      // Destroy the hook permanently
      if (msg.wParam == KILL_HOOK)
         break;

      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
   }
}

// Choose what to do with key events
HMESSAGE MessageHandler::handleKey(DWORD key)
{
   // CTRL command is being sent
   if (ctrlKey)
      return MessageHandler::ctrlCommand(key);

   // keyAction() tells us how the key impacts the internal code
   HMESSAGE action = MessageHandler::keyAction(key);

   // We are intercepting keys
   if (interceptKeys != KB_NONE)
   {
      // keyAction() tells us what to do
      if(action.wCode != NA_KEY)
         return action;

      // Intercept the key in a depending on the defined method
      switch (interceptKeys)
      {
         case KB_INTERCEPT: return MessageHandler::interceptKey(key); break;
         case KB_CHANGE: return MessageHandler::changeKey(key); break;
      }
   }
      
   // Send the key as normal
   return HMESSAGE{ PASS_KEY, key };
}

// Function to decide what to do with individual key events
HMESSAGE MessageHandler::keyAction(DWORD key)
{
   switch (key)
   {
      // CTRL (Starts a command)
      case 162: ctrlKey = TRUE; return HMESSAGE{ EAT_KEY, key }; break;

      // Any other key
      default: return HMESSAGE{ NA_KEY, key }; break;
   }
}

// Function to change the key sent
HMESSAGE MessageHandler::changeKey(DWORD key)
{
   if (key >= 65 && key <= 90) 
   {
      DWORD newKey = (((key - 64) % 26) + 65);
      return HMESSAGE{ CHANGE_KEY, newKey };
   }

   return HMESSAGE{ PASS_KEY, key };
}

// Function to intercept the key however we want
HMESSAGE MessageHandler::interceptKey(DWORD key)
{
   // Sleep(100);
   if ((rand() % 2) != 0)
      return HMESSAGE{ EAT_KEY, key };

   return HMESSAGE{ PASS_KEY, key };
}

// Commands prompted by the CTRL key being pressed
HMESSAGE MessageHandler::ctrlCommand(DWORD key)
{
   ctrlKey = FALSE;
   switch (key)
   {
      // ~ (Kills the hook and stops the program)
      case 192: return HMESSAGE{ KILL_KEY, key }; break;

      // - (Stops intercepting keys)
      case 189: interceptKeys = KB_NONE; return HMESSAGE{ EAT_KEY, key }; break;

      // + (Starts intercepting keys)
      case 187: interceptKeys = KB_INTERCEPT; return HMESSAGE{ EAT_KEY, key };  break;

      // 0 (Starts changing keys)
      case 48: interceptKeys = KB_CHANGE; return HMESSAGE{ EAT_KEY, key }; break;

      // Any other key
      default: return HMESSAGE{ PASS_KEY, key }; break;
   }
}