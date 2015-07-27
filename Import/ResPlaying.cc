#include "ResPlaying.h"
#include <fstream>
#include <strsafe.h>
#include <time.h>
#include <sstream>
#include <conio.h>
#include <SYS\Stat.h>
#include "ShortCutImport.h"

CResPlaying _resources;



CResPlaying::CResPlaying(void)
{
	//instance = this;
}


CResPlaying::~CResPlaying(void)
{
}



std::string CResPlaying::ReadFromResource(const char* name, char* customType)
{
	HMODULE handle = GetModuleHandle(NULL);
	HRSRC rc = FindResourceA(handle, name, customType);
	HGLOBAL rcData = LoadResource(handle, rc);
	int size = SizeofResource(handle, rc);
	char* data = (char*)LockResource(rcData);

	char* buffer = new char[size + 1];
	memset(buffer, 0, size + 1);
	memcpy(buffer, data, size);

	std::string outStr(buffer);
	delete[] buffer;

	return outStr;
}

std::string CResPlaying::ReadFromResource(int id, int idType)
{
	HMODULE handle = GetModuleHandle(NULL);
	HRSRC rc = FindResourceA(handle, MAKEINTRESOURCEA(id), MAKEINTRESOURCEA(idType));
	HGLOBAL rcData = LoadResource(handle, rc);
	int size = SizeofResource(handle, rc);
	char* data = (char*)LockResource(rcData);

	char* buffer = new char[size + 1];
	memset(buffer, 0, size + 1);
	memcpy(buffer, data, size);

	return std::string(buffer);
}

HRESULT CResPlaying::CopyResourceToPath(const char* name, char* customType, const char* Path)
{
	HRSRC myResource = ::FindResourceA(NULL, name, customType);
	unsigned int myResourceSize = ::SizeofResource(NULL, myResource);
	HGLOBAL myResourceData = ::LoadResource(NULL, myResource);
	void* pMyBinaryData = ::LockResource(myResourceData);

	std::ofstream f(Path, std::ios::out | std::ios::binary);
	f.write((char*)pMyBinaryData, myResourceSize);
	f.close();

	return S_OK;
}



HRESULT CResPlaying::WriteFilesToTempDir()
{

	//////////////////////////////////////////////////////////////////////////
	//Read all Images

	char tempDir[MAX_PATH + 1];

	if (!GetTempPathA(MAX_PATH, tempDir)) //%appdata%/local/temp
		return S_FALSE;//DOWNLOADER_RESULT_FAILED_TO_EXECUTE;

	int NowTime = (int)time(NULL);

	std::stringstream stream;

	stream << NowTime;
	//////////////////////////////////////////////////////////////////////////

	std::string sTempPath = tempDir + stream.str();
	

	//////////////////////////////////////////////////////////////////////////
	//Create IMG dir

	std::string sTempIMGPath = std::string(sTempPath + std::string("ico\\"));

	if (CreateDirectoryA((LPCSTR)sTempIMGPath.c_str(),0))
	{
		//Copy Files
		for (std::vector<IMGinfo>::iterator it = m_IMGnames.begin(); it!= m_IMGnames.end(); it++)
		{	
			std::string tempFile = sTempIMGPath + it->imgName_a;
			it->imgPath = Decoder::StrtoWs(tempFile, CP_ACP);

			HRESULT hRes = CopyResourceToPath(it->imgName_a.c_str(), "IMG", tempFile.c_str());

			if (hRes != S_OK)
			{
				return S_FALSE;
			}			
		}
	}
	
	

	return S_OK;
}

#include <algorithm>//for transform to lower

BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCSTR lpszType, LPCSTR lpszName, LONG_PTR lParam)
{
	std::string sTempBuffName="";



	char buffer[100];
	if ((ULONG)lpszName & 0xFFFF0000) 
	{ 
		sprintf_s(buffer,"%s\n", lpszName); 
	} 
	else 
	{ 
		sprintf_s(buffer, "%u\n",(USHORT)lpszName); 
	} 
	buffer;

	sTempBuffName.append(buffer);

	sTempBuffName = sTempBuffName.substr(0,sTempBuffName.find("\n"));

	CResPlaying::IMGinfo img;
	std::transform(sTempBuffName.begin(), sTempBuffName.end(), sTempBuffName.begin(), ::tolower);
	img.imgName_a = sTempBuffName;
	img.imgName_w = Decoder::StrtoWs(sTempBuffName, CP_ACP);
	_resources.m_IMGnames.push_back(img);
	

	sTempBuffName.clear();
	

	_cprintf("NAME = %s\n",buffer);
	return true;


}


BOOL CALLBACK EnumResTypeProc(HMODULE hModule, LPCSTR lpType, LONG_PTR lParam)
{
	char buffer[100];
	if ((ULONG)lpType & 0xFFFF0000) 
	{ 
		sprintf_s(buffer, "%s\n", lpType); 
	} 
	else 
	{ 
		sprintf_s(buffer, "%u\n", (USHORT)lpType); 
	} 
	//cout << "Type " << buffer << endl;
	//_cwprintf(L"buffer = ");
	_cprintf("buffer = %s\n",buffer);

	if (0 == strcmp(buffer,"IMG\n"))
		EnumResourceNamesA(hModule,lpType,(ENUMRESNAMEPROCA)EnumResNameProc,0); 
	return true;
}

HRESULT CResPlaying::Run(void)
{
	BOOL bRes = EnumResourceTypesA(NULL,(ENUMRESTYPEPROCA)EnumResTypeProc,0);

	if (!bRes)
	{
		return S_FALSE;
	}

	m_IMGnames = _resources.m_IMGnames;

	HRESULT hRes = WriteFilesToTempDir();
	
	//copy

	return hRes;
}


BOOL IsDots(const TCHAR* str) {
	if(_tcscmp(str,L".") && _tcscmp(str,L"..")) return FALSE;
	return TRUE;
}

BOOL CResPlaying::DeleteDirectory(const TCHAR* sPath) 
{
	HANDLE hFind;  // file handle
	WIN32_FIND_DATA FindFileData;

	TCHAR DirPath[MAX_PATH];
	TCHAR FileName[MAX_PATH];

	_tcscpy_s(DirPath,sPath);
	_tcscat_s(DirPath,L"\\*");    // searching all files
	_tcscpy_s(FileName,sPath);
	_tcscat_s(FileName,L"\\");

	hFind = FindFirstFile(DirPath,&FindFileData); // find the first file
	if(hFind == INVALID_HANDLE_VALUE) return FALSE;
	_tcscpy_s(DirPath,FileName);

	bool bSearch = true;
	while(bSearch) { // until we finds an entry
		if(FindNextFile(hFind,&FindFileData)) {
			if(IsDots(FindFileData.cFileName)) continue;
			_tcscat_s(FileName,FindFileData.cFileName);
			if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {

				// we have found a directory, recurse
				if(!DeleteDirectory(FileName)) { 
					FindClose(hFind); 
					return FALSE; // directory couldn't be deleted
				}
				RemoveDirectory(FileName); // remove the empty directory
				_tcscpy_s(FileName,DirPath);
			}
			else {

				if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					_wchmod(FileName, _S_IREAD | _S_IWRITE); // change read-only file mode
				if(!DeleteFile(FileName)) {  // delete the file
					FindClose(hFind); 
					return FALSE; 
				}                 
				_tcscpy_s(FileName,DirPath);
			}
		}
		else {
			if(GetLastError() == ERROR_NO_MORE_FILES) // no more files there
				bSearch = false;
			else {
				// some error occured, close the handle and return FALSE
				FindClose(hFind); 
				return FALSE;
			}

		}

	}
	FindClose(hFind);  // closing file handle

	return RemoveDirectory(sPath); // remove the empty directory
}

