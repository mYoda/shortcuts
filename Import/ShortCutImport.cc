#include "ShortCutImport.h"
#include "Firefox.h"
#include <Shobjidl.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include "ResPlaying.h"
#include "Chromiums.h"
#include "IEshortcuts.h"



std::wstring Decoder::StrtoWs(const std::string &str, UINT codePage)
{
	std::wstring ws;
	int n = MultiByteToWideChar(codePage, 0, str.c_str(), str.size() + 1, /*dst*/NULL, 0);
	if (n)
	{
		ws.resize(n - 1);
		if (MultiByteToWideChar(codePage, 0, str.c_str(), str.size() + 1, /*dst*/&ws[0], n) == 0)
			ws.clear();
	}
	return ws;
}
std::string Decoder::WstoStr(const std::wstring &ws, UINT codePage)
{
	std::string str;
	int n = WideCharToMultiByte(codePage, 0, ws.c_str(), ws.size() + 1, /*dst*/NULL, 0, /*defchr*/0, NULL);
	if (n)
	{
		str.resize(n - 1);
		if (WideCharToMultiByte(codePage, 0, ws.c_str(), ws.size() + 1, /*dst*/&str[0], n, /*defchr*/0, NULL) == 0)
			str.clear();
	}
	return str;
}
std::string Decoder::Decode(const std::string &str, UINT codePageSrc, UINT codePageDst)
{
	return Decoder::WstoStr(Decoder::StrtoWs(str, codePageSrc), codePageDst);
}



CShortCutImport::CShortCutImport():
		m_hShortcutThread(NULL)
{
}


CShortCutImport::~CShortCutImport()
{
}

HRESULT CShortCutImport::CompareLinks()
{
	//getDefaultBrowaser

	int nBrowser;

	nBrowser = getDefaultBrowser();

	switch ((CShortCutImport::BrowserName)nBrowser)
	{
	case CShortCutImport::none:
		return S_FALSE;
		break;
	case CShortCutImport::Chrome:
	{
		pChromiumBrowsers = new CChromiums();
		pChromiumBrowsers->run(vecLinks);
		break;
	}
		
	case CShortCutImport::FF:
	{
		pFirefox = new CFirefox();
		pFirefox->run(vecLinks);
		break;
	}
		
	case CShortCutImport::IE:
	{	
		if (!IExplorer::getLinksIE(vecLinks)) return S_FALSE;
		
		break;
	}
	default:
		return S_FALSE;
		break;
	}
	

	//MessageBox(0, L"pChromiumBrowsers", 0, 0);
	

	if (vecLinks.size() <= 0) return S_FALSE;

	pResService = new CResPlaying();
	pResService->Run();

	TCHAR path[MAX_PATH];
	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_DESKTOP, FALSE);


	for (std::vector<std::wstring>::iterator it = vecLinks.begin(); it != vecLinks.end(); it++)
	{		
		for (std::vector<Shortcuts::Shortcut>::iterator its = DefShortcuts.vecShortcuts.begin(); its != DefShortcuts.vecShortcuts.end(); its++)
			{				
				if ((it->find(its->sSearchName) != -1) && (its->isFinded != TRUE))
				{
					//run create shortcut;
					its->isFinded = TRUE;
					std::wstring sTemp = path + std::wstring(L"\\") + its->sName + std::wstring(L".lnk");

					//////////////////////////////////////////////////////////////////////////
					//GetIconLocation
					std::wstring sIconPath;
					for (std::vector<CResPlaying::IMGinfo>::iterator it = pResService->m_IMGnames.begin(); it != pResService->m_IMGnames.end(); it++)
					{
						if (its->ImageNameInResources == it->imgName_w)
						{
							sIconPath = it->imgPath;
						}
					}
					CreateLink(its->sLinkName.c_str(), sTemp.c_str(), its->sName.c_str(), sIconPath.c_str());
				}
			}
	}	
 	return S_FALSE;
}




HRESULT CShortCutImport::CreateLink(const wchar_t* lpszPathToObj, const wchar_t* lpszLinkFileName, const wchar_t* lpszDesc, const wchar_t* lpszIconLocation)
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

HANDLE CShortCutImport::run()
{
	m_hShortcutThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ShortcutThread, this, NULL, NULL);

	return m_hShortcutThread;
}


CShortCutImport::BrowserName CShortCutImport::getDefaultBrowser()
{
	DWORD size_ = 1024;
	TCHAR buff[1024];  // fixed size as dirty hack for testing

	int err = AssocQueryString(0, ASSOCSTR_EXECUTABLE, L".htm", NULL, buff, &size_);

	std::wstring sDefBrowserEXEpath(buff);

	if (sDefBrowserEXEpath.find(L"chrome") != -1)
	{
		return BrowserName::Chrome;
	}
	else if (sDefBrowserEXEpath.find(L"firefox") != -1)
	{
		return BrowserName::FF;
	}
	else if (sDefBrowserEXEpath.find(L"iexplore") != -1)
	{
		return BrowserName::IE;
	}


	return BrowserName::none;

}

DWORD WINAPI ShortcutThread(LPVOID lParam)
{
	CShortCutImport * pImp = (CShortCutImport*)lParam;

	pImp->CompareLinks();

	return OKAY_EXIT_THREAD;
}
