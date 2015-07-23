#ifndef _RESPLAYING_H_
#define _RESPLAYING_H_



#pragma once
#include <Windows.h>
//#include <string>
#include <vector>
#include <tchar.h>
//#include "includes.h"
#include <string>
#include <vector>
#include <map>
class CResPlaying
{
public:
	CResPlaying(void);
	~CResPlaying(void);

	HRESULT		Run(void);
	BOOL		DeleteDirectory(const TCHAR* sPath) ;


	

friend	BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCSTR lpszType, LPCSTR lpszName, LONG_PTR lParam);
friend	BOOL CALLBACK EnumResTypeProc(HMODULE hModule, LPCSTR lpType, LONG_PTR lParam);

	
	std::wstring				m_sDirPath;
	std::wstring				m_sFilePath;
	
	struct IMGinfo
	{
		std::wstring imgName_w;
		std::string imgName_a;
		std::wstring imgPath;
	};

	std::vector<IMGinfo>	m_IMGnames;

private:
	

	std::string ReadFromResource(const char* name, char* customType);
	std::string ReadFromResource(int id, int idType);	
	HRESULT		WriteFilesToTempDir();	
	HRESULT		CopyResourceToPath(const char* name, char* customType, const char* Path);
	

	
	
};

extern CResPlaying _resources;

#endif