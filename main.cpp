#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

using namespace std;

DWORD getPID(const char* procName)
{
	DWORD pid = 0;
	HANDLE pProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (pProcess != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);
		do
		{
			if (!_stricmp(procEntry.szExeFile, procName))
			{
				pid = procEntry.th32ProcessID;
				cout << procEntry.szExeFile << " found - PID: " << pid << endl;
				CloseHandle(pProcess);
			}
		} while (Process32Next(pProcess, &procEntry));
	}
	return pid;
}

bool InjectDLL(DWORD procId, const char* dllName)
{
	//Definations
	char buff[MAX_PATH];
	//Get Full Path
	if (!procId) return false;
	GetFullPathName(dllName, strlen(buff) + 1, buff, 0); // Get Full Path Of The File
	cout << "Dll Path: " << buff << endl;
	//Injecting
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, procId); //Open targer process
	LPVOID loc = VirtualAllocEx(hProcess, 0, strlen(buff) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); // Create a virtual memory in targer process
	WriteProcessMemory(hProcess, loc, buff, strlen(buff) + 1, 0); //Write Dll Path into &loc
	HANDLE hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0); // Load DLL
	return true;
}

int main(int agrc,char* agrv[]) {

	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTitleA("C-Inject v1.2");
	SetConsoleTextAttribute(hOutput, 14);
	cout << "|---- Developed by CoderDuc ----| \n\n";
	Sleep(1000);
	SetConsoleTextAttribute(hOutput, 15);
	if (!InjectDLL(getPID(agrv[1]), agrv[2])) {
		cout << "An error while injecting dll !!!\n";
	}
	else {
		cout << "DLL Injected !!!\n";
	}
	system("pause");
	return 0;
}