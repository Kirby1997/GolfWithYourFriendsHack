// HoleInWhatever.cpp : Defines the entry point for the console application.
//


#include<iostream>
#include<Windows.h>
#include<string>
#include<ctime>
#include<TlHelp32.h>
#include<tchar.h>




void writeToMemory(HANDLE procHandle);
DWORD findAddress(int pointerLevel, HANDLE procHandle, DWORD offsets[], DWORD baseAddress);
uintptr_t GetModuleBaseAddress(DWORD dwProcID, const TCHAR *szModuleName);


using namespace std;

LPCSTR gameName = "Golf With Your Friends";
TCHAR moduleName[] = TEXT("UnityPlayer.dll");
string gameStatus;


bool strokeStatus;
bool potStatus;
bool powerStatus;
bool zoomStatus;
int strokeValue;
int potValue;
float powerValue = 0x2904;
float zoomValue = 100;


bool isGameAvail;
bool updateOnNextRun;
//DWORD dllBase = (DWORD)GetModuleHandleA("UnityPlayer.dll");
//DWORD strokeBaseAddress = 0x0F330000 + 0x00FF7820;
//DWORD strokeBaseAddress = 0x0442AFC0;
uintptr_t playerdllBaseAddress = 0;
DWORD strokeBaseAddress;
DWORD zoomBaseAddress;
DWORD strokeOffsets[] = { 0x4C, 0x10, 0x1C, 0x54, 0x50, 0x8, 0x1C, 0xC, 0x18, 0x18, 0x114 }; // FROM BOTTOM TO TOP

DWORD zoomOffsets[] = { 0xC, 0x24, 0xB4, 0x10, 0x40, 0x1C, 0x1C, 0x14, 0x18, 0x30, 0xB0 };



int main()
{

	HWND gameWindow = NULL;
	int timeSinceLastUpdate = clock();
	int gameAvailTMR = clock();
	int onePressTMR;
	DWORD procID = NULL;
	HANDLE procHandle = NULL;
	updateOnNextRun = true;
	string sStrokeStatus;
	string sPotStatus;
	string sPowerStatus;
	string sZoomStatus;
	onePressTMR = clock();




	while (!GetAsyncKeyState(VK_INSERT)) // Exit key
	{
		if (clock() - gameAvailTMR > 100)
		{
			gameAvailTMR = clock();
			isGameAvail = false;
			gameWindow = FindWindowA(NULL, gameName);
			if (gameWindow)
			{
				GetWindowThreadProcessId(gameWindow, &procID);
				playerdllBaseAddress = GetModuleBaseAddress(procID, moduleName);
				strokeBaseAddress = playerdllBaseAddress + 0x01097C44; // THE STUFF IN THE FIRST BOX FOR THE POINTER
				zoomBaseAddress = playerdllBaseAddress + 0x01096130;
				if (procID != 0)
				{
					procHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
					if (procHandle == INVALID_HANDLE_VALUE || procHandle == NULL)
					{
						gameStatus == "Failed to open process for valid handle";
					}
					else
					{
						gameStatus = "Golf With Your Friends is ready to hack";
						isGameAvail = true;
					}
				}
				else gameStatus = "Failed to obtain process ID";
			}
			else gameStatus = "Golf With Your Friends not found";


			if (updateOnNextRun || clock() - timeSinceLastUpdate > 5000)
			{
				system("cls");
				cout << "----------------------------------------------------" << endl;
				cout << "        Lose All Your Friends With Golf" << std::endl;
				cout << "            For version 1.108.9H1" << std::endl;
				cout << "----------------------------------------------------" << endl << endl;
				cout << "GAME STATUS: " << gameStatus << "   " << endl << endl;
				cout << "[F1] Hole in one   -> " << sStrokeStatus << " <-" << endl << endl;
				cout << "[F2] Pot ball   -> " << sPotStatus << " <-" << endl << endl;
				cout << "[F3] Unlimited power   -> " << sPowerStatus << " <-" << endl << endl;
				cout << "[F4] Unlimited zoom   -> " << sZoomStatus << " <-" << endl << endl;
				cout << "[INSERT] Exit" << endl;

				cout << "YouTube: Kirby - Hacks and Exploits" << endl;
				updateOnNextRun = false;
				timeSinceLastUpdate = clock();
			}

			if (isGameAvail)
			{
				writeToMemory(procHandle);
			}
		}

		if (clock() - onePressTMR > 400)
		{
			if (isGameAvail)
			{
				if (GetAsyncKeyState(VK_F1))
				{
					onePressTMR = clock();
					strokeStatus = !strokeStatus;
					updateOnNextRun = true;
					if (strokeStatus)
					{
						sStrokeStatus = "ON";
						strokeValue = 1;
					}
					else
					{
						sStrokeStatus = "OFF";
					}
				}
				else if (GetAsyncKeyState(VK_F2))
				{
					onePressTMR = clock();
					potStatus = !potStatus;
					updateOnNextRun = true;
					if (potStatus)
					{
						sPotStatus = "ON";
						potValue = 1;
					}
					else
					{
						sPotStatus = "OFF";
						potValue = 0;
					}
				}
				else if (GetAsyncKeyState(VK_F3))
				{
					onePressTMR = clock();
					powerStatus = !powerStatus;
					updateOnNextRun = true;
					if (powerStatus)
					{
						sPowerStatus = "ON";
						powerValue = 0x7F7FFFFF;
					}
					else
					{
						sPowerStatus = "OFF";
						powerValue = 0x2904;
					}
				}
				else if (GetAsyncKeyState(VK_F4))
				{
					onePressTMR = clock();
					zoomStatus = !zoomStatus;
					updateOnNextRun = true;
					if (zoomStatus)
					{
						sZoomStatus = "ON";
						zoomValue = 10000;
					}
					else
					{
						sZoomStatus = "OFF";
						zoomValue = 100;
					}
				}
			}
		}
	}
	DestroyWindow(gameWindow);

	return 0;
}

DWORD findAddress(int pointerLevel, HANDLE procHandle, DWORD offsets[], DWORD baseAddress)
{
	DWORD pointer = baseAddress;

	DWORD pTemp;

	DWORD pointerAddr;
	for (int i = 0; i < pointerLevel; i++)
	{
		if (i == 0)
		{
			ReadProcessMemory(procHandle, (LPCVOID)pointer, &pTemp, 4, NULL);
		}
		pointerAddr = pTemp + offsets[i];


		ReadProcessMemory(procHandle, (LPCVOID)pointerAddr, &pTemp, 4, NULL);
	}
	return pointerAddr;
}



void writeToMemory(HANDLE procHandle)
{

	if (strokeStatus) {
		int pointerLevel = sizeof(strokeOffsets) / sizeof((strokeOffsets)[0]);
		//cout << pointerLevel << endl;
		//cout <<  "start\npointer level: " << pointerLevel << "\n procHandle: " << procHandle << "\n strokeOffsets: " << strokeOffsets << "\n strokeBaseAddress: " << strokeBaseAddress << "\nEND\n";
		DWORD strokeAddressToWrite = findAddress(pointerLevel, procHandle, strokeOffsets, strokeBaseAddress);

		//cout << strokeAddressToWrite << endl;

		//cout << &strokeValue;
		WriteProcessMemory(procHandle, (BYTE*)strokeAddressToWrite, &strokeValue, sizeof(strokeValue), NULL);

	}

	if (potStatus || !potStatus) {
		int pointerLevel = sizeof(strokeOffsets) / sizeof((strokeOffsets)[0]);
		DWORD strokeAddressToWrite = findAddress(pointerLevel, procHandle, strokeOffsets, strokeBaseAddress);
		DWORD potAddress = strokeAddressToWrite - 0x24;
		WriteProcessMemory(procHandle, (BYTE*)potAddress, &potValue, sizeof(potValue), NULL);
	}

	if (powerStatus || !powerStatus) {
		int pointerLevel = sizeof(strokeOffsets) / sizeof((strokeOffsets)[0]);
		DWORD strokeAddressToWrite = findAddress(pointerLevel, procHandle, strokeOffsets, strokeBaseAddress);
		DWORD maxPowerAddress = strokeAddressToWrite + 0x2C;
		WriteProcessMemory(procHandle, (BYTE*)maxPowerAddress, &powerValue, sizeof(powerValue), NULL);
	}

	if (zoomStatus || !zoomStatus) {
		int pointerLevel = sizeof(zoomOffsets) / sizeof((zoomOffsets)[0]);
		DWORD zoomAddressToWrite = findAddress(pointerLevel, procHandle, zoomOffsets, zoomBaseAddress);
		DWORD maxZoomAddress = zoomAddressToWrite + 0x8;
		WriteProcessMemory(procHandle, (BYTE*)maxZoomAddress, &zoomValue, sizeof(zoomValue), NULL);
	}

}

uintptr_t GetModuleBaseAddress(DWORD dwProcID, const TCHAR *szModuleName)
{
	uintptr_t ModuleBaseAddress = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 ModuleEntry32;
		ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnapshot, &ModuleEntry32))
		{
			do
			{
				if (_tcsicmp(ModuleEntry32.szModule, szModuleName) == 0)
				{
					ModuleBaseAddress = (uintptr_t)ModuleEntry32.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnapshot, &ModuleEntry32));
		}
		CloseHandle(hSnapshot);
	}
	return ModuleBaseAddress;
}
