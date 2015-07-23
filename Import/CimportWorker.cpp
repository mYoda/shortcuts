#include "CimportWorker.h"//#include "includes.h"

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <Shlobj.h>
#include <fstream>





CimportWorker::CimportWorker()
{
	Init();
}


CimportWorker::~CimportWorker()
{
}


void CimportWorker::Init()
{
	//fill fileNames for copy
	m_vecListOfFiles.push_back(L"Archived History");
	m_vecListOfFiles.push_back(L"Archived History-journal");
	m_vecListOfFiles.push_back(L"Bookmarks");
	m_vecListOfFiles.push_back(L"Bookmarks.bak");
	m_vecListOfFiles.push_back(L"Cookies");
	m_vecListOfFiles.push_back(L"Cookies-journal");
	m_vecListOfFiles.push_back(L"Current Session");
	m_vecListOfFiles.push_back(L"Current Tabs");
	m_vecListOfFiles.push_back(L"Favicons");
	m_vecListOfFiles.push_back(L"Favicons-journal");
	m_vecListOfFiles.push_back(L"History");
	m_vecListOfFiles.push_back(L"History Provider Cache");
	m_vecListOfFiles.push_back(L"History-journal");
	m_vecListOfFiles.push_back(L"Last Session");
	m_vecListOfFiles.push_back(L"Last Tabs");
	m_vecListOfFiles.push_back(L"Top Sites");
	m_vecListOfFiles.push_back(L"Top Sites-journal");
	m_vecListOfFiles.push_back(L"Web Data");
	m_vecListOfFiles.push_back(L"Web Data-journal");

/*	m_vecListOfFiles.push_back(L"");*/
}


HRESULT CimportWorker::GetProfilePath(std::wstring & sPath)
{
	std::wstring sChromePath, sProfileName;
	if (S_FALSE == GetChromePath(sChromePath)) return S_FALSE;
	
	if (S_FALSE == GetProfileFromLocalState(_In_ sChromePath, _Out_ sProfileName)) return S_FALSE;

	sPath = sChromePath + sProfileName.append(L"\\") ;

	if (sPath.size()>0)	return S_OK;

	return S_FALSE;
}


HRESULT CimportWorker::GetChromePath(std::wstring & sPath)
{	
	BOOL bRes = FALSE;
	TCHAR path[MAX_PATH];

	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_LOCAL_APPDATA, FALSE);
	std::wstring sTemp = path;

	sTemp.append(CHROME_DIR);

	if (PathFileExists(sTemp.c_str()))
	{
		sPath = sTemp;
		return S_OK;
	}
	
	return S_FALSE;
}

HRESULT CimportWorker::GetProfileFromLocalState(std::wstring sChromePath, std::wstring & sProfileName)
{
	sChromePath.append(L"Local State");

	//find last_active_profiles

	//read the file
	std::wstring buffer;
	std::wstring temp;
	std::wstring CurrentLine;
	std::wfstream m_File;

	try
	{
		m_File.open(sChromePath.c_str(), std::wfstream::in /*| std::wfstream::app*/);
		if (m_File.is_open())
		{
			do
			{
				std::getline(m_File, buffer);
				temp = buffer;/*Decoder::StrtoWs(buffer, CP_UTF8);*/
				if (temp.empty())
					continue;
				if (temp.find(L"last_used") != -1)
				{
					CurrentLine = temp;
					//get ProfileName from Line
					//"last_active_profiles": [ "Profile 1" ],

					sProfileName = CurrentLine.substr(CurrentLine.find(L"last_used\": \"") + std::wstring(L"last_used\": \"").size());
					sProfileName = sProfileName.erase(sProfileName.find(L"\","));
					//--

					if (sProfileName.size() > 0)
					{
						break;
					}
				}

			} while ((int)m_File.tellg() != -1);
		}

		m_File.close();
	}
	catch (...)
	{
	}

	if (sProfileName.size() > 0)	return S_OK;

	return S_FALSE;
}


HRESULT CimportWorker::CopyFilesToOurDir(const wchar_t * sOurDirPath, const wchar_t * sFromDirPath)
{

	BOOL bRes = FALSE;
	TCHAR path[MAX_PATH];

	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_LOCAL_APPDATA, FALSE);
	std::wstring sTemp = path;
	sTemp.append(INTERNET_DIR);

	if (!PathFileExists(sTemp.c_str()))
	{
		if(!CreateOurDir()) return S_FALSE;
	}

	

	for (std::vector<std::wstring>::iterator it = m_vecListOfFiles.begin(); it != m_vecListOfFiles.end(); it++)
	{
		std::wstring TOpath(sTemp);
		
		TOpath.append(it->c_str());

		std::wstring FROMpath(sFromDirPath);
		FROMpath.append(it->c_str());

		

		if (PathFileExistsW(FROMpath.c_str()))
		{
			bRes = CopyFileW(FROMpath.c_str(), TOpath.c_str(), FALSE);
		}


		
	}
	
	return E_NOTIMPL;
}





BOOL CimportWorker::CreateOurDir()
{
	std::wstring sResultDir, sTemp;

	BOOL bRes = FALSE;
	TCHAR path[MAX_PATH];

	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_LOCAL_APPDATA, FALSE);
	sTemp = path;

	sTemp.append(L"\\Internet\\");
	bRes = CreateDirectory(sTemp.c_str(), NULL);
	if (!bRes) return S_FALSE;

	sTemp.append(L"User Data\\");
	bRes = CreateDirectory(sTemp.c_str(), NULL);
	if (!bRes) return S_FALSE;

	sTemp.append(L"Default\\");
	bRes = CreateDirectory(sTemp.c_str(), NULL);
	if (!bRes) return S_FALSE;

	sResultDir = sTemp;

	return PathFileExists(sResultDir.c_str());
}
