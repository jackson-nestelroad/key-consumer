// Implementation file for KeyboardHook class
#include "../Header/KeyboardHook.h"
HHOOK KeyboardHook::m_hHook;
HWND KeyboardHook::m_hWnd;

// Constructor
KeyboardHook::KeyboardHook()
{
   m_hHook = NULL;
   m_hWnd = NULL;
}

// Get module from address
HMODULE WINAPI KeyboardHook::ModuleFromAddress(PVOID pv)
{
   MEMORY_BASIC_INFORMATION mbi;
   if (::VirtualQuery(pv, &mbi, sizeof(mbi)) != 0)
      return (HMODULE) mbi.AllocationBase;
   else
      return NULL;
}

// Hook callback function
LRESULT CALLBACK KeyboardHook::HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
   if(nCode < 0 || nCode == HC_NOREMOVE)
      return ::CallNextHookEx(m_hHook, nCode, wParam, lParam);

   // Check the previous key state
   if (lParam & 0x40000000)
      return ::CallNextHookEx(m_hHook, nCode, wParam, lParam);

   // This structure contains the data we received from callback
   KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT*) lParam;

   // Check the message is from the keyboard or is synthesized by SendInput API
  /* if ((pkbhs->flags & LLKHF_INJECTED))
      return ::CallNextHookEx(m_hHook, nCode, wParam, lParam);*/

   // Only send keydown actions
   if (wParam == WM_KEYDOWN)
   {
      // Handle the key according to its code
      // We can intercept the message here if we want
      switch (MessageHandler::handleKey(pkbhs->vkCode))
      {
         case PASS_KEY: return ::CallNextHookEx(m_hHook, nCode, wParam, lParam); break;
         case KILL_KEY: ::PostMessage(m_hWnd, WM_KEYINPUT, KILL_HOOK, KILL_HOOK);
         case EAT_KEY: return 1; break;
      }
      // Post private messages to main function
      // wParam specifies the virtual key code
      // lParam specifies the scan code
      // ::PostMessage(m_hWnd, WM_KEYINPUT, pkbhs->vkCode, pkbhs->scanCode);
   }

   // Call the next hook in the hook chain
   return ::CallNextHookEx(m_hHook, nCode, wParam, lParam);
}

BOOL WINAPI KeyboardHook::SetLowKeyboardHook(BOOL bInstall, DWORD dwThreadId, HWND hWndCaller)
{
   BOOL bOk;
   m_hWnd = hWndCaller;
   // LRESULT(__stdcall KeyboardHook::*callback) (int, WPARAM, LPARAM) = &KeyboardHook::HookCallback;
   // HOOKPROC callback = { this->HookCallback };

   // Set the hook
   if (bInstall)
   {
      // this->m_hHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, this->HookCallback, this->ModuleFromAddress(this->HookCallback), dwThreadId);
      m_hHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, this->HookCallback, NULL, 0);
      bOk = (m_hHook != NULL);
   }

   // Remove the hook
   else
   {
      bOk = ::UnhookWindowsHookEx(m_hHook);
      m_hHook = NULL;
   }

   return bOk;
}

// Set the hook
BOOL __stdcall KeyboardHook::setHook()
{
   if (!this->SetLowKeyboardHook(TRUE, 0, ::GetActiveWindow()))
      return false;
   else
      return true;
}

// Remove the hook
BOOL __stdcall KeyboardHook::removeHook()
{
   if (!this->SetLowKeyboardHook(FALSE, 0, ::GetActiveWindow()))
      return false;
   else
      return true;
}
