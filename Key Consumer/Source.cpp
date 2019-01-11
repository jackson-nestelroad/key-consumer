// This program injects a DLL into a running process

#include <iostream>
#include <string>
#include <Windows.h>
#include <Shlwapi.h>
#include <TlHelp32.h>

// Needed for PathFileExists()
#pragma comment(lib, "Shlwapi.lib")

using namespace std;

// Function prototypes
void Help();
INT64 WINAPI GetProcId(char* ProcName);
bool InjectDll(const DWORD ProcessID, const char* const PathToDll);

// Program entry point
int main(int argc, char* argv[])
{
   // Argument not given
   if (argc < 3)
   {
      Help();
      return EXIT_FAILURE;
   }

   // Give help
   if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
   {
      Help();
      return EXIT_FAILURE;
   }

   // Path to DLL does not exist
   if (PathFileExists(argv[2]) == FALSE)
   {
      cout << "ERROR: Path \"" << argv[2] << "\" does not exist." << endl;
      return EXIT_FAILURE;
   }
   
   // Get process ID for given process name
   INT64 ProcId = GetProcId(argv[1]);

   // GetProcId failed
   if (ProcId < 0)
   {
      switch (ProcId)
      {
         case -1: cout << "ERROR: Failed to create process snapshot." << endl; break;
         case -2: cout << "ERROR: Could not find process \"" << argv[1] << "\"." << endl; break;
         default: cout << "ERROR: An unknown error occured." << endl; break;
      }
      return EXIT_FAILURE;
   }

   cout << "Attaching low-level keyboard hook to \"" << argv[1] << "\" (id " << ProcId << ")..." << endl;

   // The injection worked!
   if (InjectDll((DWORD)ProcId, argv[2]))
   {
      cout << "Successfully injected low-level keyboard hook into \"" << argv[1] << "\"." << endl;
   }
   // Something went wrong
   else
   {
      cout << "Failed to inject low-level keyboard hook into \"" << argv[1] << "\"." << endl;
      cout << endl;
      cout << "Please contact your friendly-neighborhood Microsoft technician for support." << endl;
   }

   return EXIT_SUCCESS;
}

// Function that actually injects the DLL
bool InjectDll(const DWORD ProcessID, const char* const PathToDll)
{
   // Open our process via ID
   HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
   
   // Failed to open target process
   if (hProc == NULL)
   {
      cout << "ERROR: Failed to open target process" << endl;
      return false;
   }

   // Allocate memory for the path to the DLL
   LPVOID AllocatedDllPathAddress = VirtualAllocEx(hProc, NULL, strlen(PathToDll), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

   // Failed to allocate memory
   if (AllocatedDllPathAddress == NULL)
   {
      cout << "ERROR: Failed to allocate memory in target process." << endl;
      return false;
   }

   // Write the path to the DLL to the memory of the target process
   BOOL WriteSuccess = WriteProcessMemory(hProc, AllocatedDllPathAddress, PathToDll, strlen(PathToDll), NULL);

   // Memory write was unsuccessful
   if (!WriteSuccess)
   {
      cout << "ERROR: Failed to write to the target process' memory." << endl;
      return false;
   }

   // Get address of LoadLibrary() function
   LPTHREAD_START_ROUTINE LoadLibraryAddress = (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibrary("kernel32.dll"), "LoadLibraryA");

   // LoadLibrary() not found
   if (LoadLibraryAddress == NULL)
   {
      cout << "ERROR: Failed to find LoadLibrary()." << endl;
      return false;
   }

   // Create remote thread in target process
   HANDLE RemoteThread = CreateRemoteThread(hProc, NULL, NULL, LoadLibraryAddress, AllocatedDllPathAddress, NULL, NULL);
   if (RemoteThread == NULL)
   {
      cout << "ERROR: Failed to create remote thread in target process." << endl;
      cout << endl;
      cout << "    Error Code: " << GetLastError() << endl;
      return false;
   }

   return true;
}

// Get process ID by process name
INT64 WINAPI GetProcId(char* ProcName)
{
   // List of processes
   PROCESSENTRY32 Process;

   // Set this or Process32First will fail
   Process.dwSize = sizeof(PROCESSENTRY32);

   // Take snapshot of all proceses in system
   HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

   // Snapshot failed
   if (hSnapshot == INVALID_HANDLE_VALUE)
      return -1;

   // Returns false if error occurs in copying process to buffer
   if (Process32First(hSnapshot, &Process))
   {
      do
      {
         // cout << pe32.szExeFile << endl;
         // Process names match
         if (strcmp(Process.szExeFile, ProcName) == 0)
            break;
      } while (Process32Next(hSnapshot, &Process));
   }

   // Close handle once we are finished
   if (hSnapshot != INVALID_HANDLE_VALUE)
      CloseHandle(hSnapshot);

   // Error occured or we did not find the process
   if (GetLastError() == ERROR_NO_MORE_FILES)
      return -2;

   // Return process ID
   return Process.th32ProcessID;
}

// Info about this program
void Help()
{
   cout << "Injects keyboard hook to a named process." << endl;
   cout << endl;
   cout << "KeyConsumer [Process name] [Path to DLL]" << endl;
   cout << endl;
   cout << "    Process name MUST include .exe file suffix." << endl;
   cout << endl;
   cout << "    If path to DLL is left blank, the batch script will look in the folder it is located in." << endl;
   cout << endl;
   cout << "KeyConsumer Commands" << endl;
   cout << endl;
   cout << "    Ctrl + \"~\"    DESTROYs keyboard hook permanently." << endl;
   cout << "    Ctrl + \"-\"    Turns all key interception OFF." << endl;
   cout << "    Ctrl + \"+\"    Turns key interception ON." << endl;
   cout << "    Ctrl + \"0\"    Turns key changing ON." << endl;
}