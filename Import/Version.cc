#include "Version.h"
#include <strsafe.h>
#pragma comment(lib, "User32.lib")

#pragma comment(lib,"Version.lib")

#include <SYS\Stat.h>

#pragma comment(lib, "iphlpapi.lib")
#include <Iphlpapi.h>

#include <sstream>

//////////////////////////////////////////////////////////////////////////
#include <Windows.h>
#include <winternl.h>




typedef NTSTATUS(WINAPI* _NtQueryInformationProcess)
(_In_ HANDLE ProcessHandle, _In_ PROCESSINFOCLASS ProcessInformationClass,
_Out_ PVOID ProcessInformation, _In_ ULONG ProcessInformationLength, _Out_opt_ PULONG ReturnLength);
_NtQueryInformationProcess NtQueryInformationProcess_;

CVersion::CVersion()
{
}


CVersion::~CVersion()
{
}

CVersion::OSVERSION CVersion::GetVersion()
{
	CVersion::OSVERSION os;
	OSVERSIONINFOEX osvi = CVersion::OSVersion(OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId()));

	switch (osvi.dwMajorVersion)
	{
	case 5:
	{
		return CVersion::OSVERSION::WinXP;
		break;
	}
	case 6:
	{
		if (osvi.dwMinorVersion == 0)
		{
			return CVersion::OSVERSION::WinVista;
		}
		else if (osvi.dwMinorVersion == 1)
		{
			return CVersion::OSVERSION::Win7;
		}
		else if (osvi.dwMinorVersion == 2)
		{
			return CVersion::OSVERSION::Win8;
		}
		else if (osvi.dwMinorVersion == 3)
		{
			return CVersion::OSVERSION::Win8_1;
		}
		
		break;
	}
	case 10:
	{
		return CVersion::OSVERSION::Win10;
		break;
	}
	default:
		return CVersion::OSVERSION::Unknown;
	}

	return CVersion::OSVERSION::Unknown;
}




DWORD GetProcessPEBAddress(HANDLE hProc)
{
	PROCESS_BASIC_INFORMATION peb;
	DWORD tmp;
	NtQueryInformationProcess_ = (_NtQueryInformationProcess)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQueryInformationProcess");
	NtQueryInformationProcess_(hProc, ProcessBasicInformation, &peb, sizeof(PROCESS_BASIC_INFORMATION), &tmp);
	return (DWORD)peb.PebBaseAddress;
}

OSVERSIONINFOEX CVersion::OSVersion(HANDLE handle)
{
	OSVERSIONINFOEX osvi;
	DWORD Major;
	DWORD Minor;
	DWORD Build;
	DWORD pebAddress = GetProcessPEBAddress(handle);
	DWORD OSMajorVersionAddress = pebAddress + 0x0a4;
	DWORD OSMinorVersionAddress = pebAddress + 0x0a8;
	DWORD OSBuildNumberAddress = pebAddress + 0x0ac;
	ReadProcessMemory(handle, (void*)OSMajorVersionAddress, &osvi.dwMajorVersion, sizeof(osvi.dwMajorVersion), 0);
	ReadProcessMemory(handle, (void*)OSMinorVersionAddress, &osvi.dwMinorVersion, sizeof(osvi.dwMinorVersion), 0);
	ReadProcessMemory(handle, (void*)OSBuildNumberAddress, &osvi.dwBuildNumber, sizeof(osvi.dwBuildNumber), 0);

	std::wstringstream stream;
	stream << osvi.dwMajorVersion << "." << osvi.dwMinorVersion << "." << osvi.dwBuildNumber;
	return osvi;

}