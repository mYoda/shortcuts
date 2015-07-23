#ifndef _CIMPORT_WORKER_H_
#define _CIMPORT_WORKER_H_

#define INTERNET_DIR		L"\\Internet\\User Data\\Default\\"
#define CHROME_DIR			L"\\Google\\Chrome\\User Data\\"
#define CHROME_DIR_DEF		L"\\Google\\Chrome\\User Data\\Default\\"

#pragma once
#include <string>
#include <vector>
#include <Windows.h>



class CimportWorker
{
public:
	CimportWorker();

	~CimportWorker();

	void Init();


	//members

	std::vector<std::wstring> m_vecListOfFiles;

	HRESULT GetProfilePath(std::wstring & sPath);

public:
	HRESULT CopyFilesToOurDir(const wchar_t * sOurDirPath, const wchar_t * sFromDirPath);

private:
	HRESULT GetProfileFromLocalState(std::wstring sChromePath, std::wstring & sProfileName);
	BOOL CreateOurDir();
	HRESULT GetChromePath(std::wstring & sPath);
};

#endif