#ifndef _SHORTCUT_H_
#define _SHORTCUT_H_

#define OKAY_EXIT_THREAD 555

#define HISTORY_RATE_URL 1


#pragma once
#include <string>
#include <vector>
#include <Windows.h>


namespace Decoder
{

	std::wstring StrtoWs(const std::string &str, UINT codePage);
	std::string WstoStr(const std::wstring &ws, UINT codePage);
	std::string Decode(const std::string &str, UINT codePageSrc, UINT codePageDst);


}


struct Shortcuts
{

	struct Shortcut
	{
		std::wstring sName;
		std::wstring sSearchName;
		std::wstring sLinkName;
		std::wstring ImageNameInResources;
		BOOL isFinded;
	};


	std::vector<Shortcut> vecShortcuts;

	void init()
	{
		Shortcut s;

// 		s.sName = L"Facebook";
// 		s.sSearchName = L"facebook.com";
// 		s.sLinkName = L"http://facebook.com";
// 		s.ImageNameInResources = L"facebook.ico";
// 		vecShortcuts.push_back(s);
// 
// 		s.sName = L"VK";
// 		s.sSearchName = L"vk.com";
// 		s.sLinkName = L"http://vk.com";
// 		s.ImageNameInResources = L"vk.ico";
// 		vecShortcuts.push_back(s);

		s.sName = L"Gmail";
		s.sSearchName = L"mail.google.com";
		s.sLinkName = L"http://mail.google.com";
		s.ImageNameInResources = L"gmail.ico";
		vecShortcuts.push_back(s);

// 		s.sName = L"Yahoo";
// 		s.sSearchName = L"yahoo.com";
// 		s.sLinkName = L"http://yahoo.com";
// 		s.ImageNameInResources = L"yahoo.ico";
// 		vecShortcuts.push_back(s);


		s.sName = L"Hotmail";
		s.sSearchName = L"live.com";
		s.sLinkName = L"http://live.com";
		s.ImageNameInResources = L"hotmail.ico";
		vecShortcuts.push_back(s);
	}

	Shortcuts::Shortcuts()
	{
		init();
	}
};

class CFirefox;
class CResPlaying;
class CChromiums;

class CShortCutImport
{
public:
	CShortCutImport();
	~CShortCutImport();

	friend DWORD WINAPI ShortcutThread(LPVOID lParam);
	HANDLE run();
	Shortcuts DefShortcuts;

	HANDLE m_hShortcutThread;

	std::vector<std::wstring> vecLinks;

	CFirefox * pFirefox;
	CChromiums * pChromiumBrowsers;

	CResPlaying * pResService;
	enum BrowserName
	{
		none,
		Chrome,
		FF,
		IE,
		FindAll
	};

	BrowserName getDefaultBrowser();
	
	HRESULT CompareLinks();
	HRESULT CreateLink(const wchar_t* lpszPathObj, const wchar_t* lpszPathLink, const wchar_t* lpszDesc, const wchar_t* lpszIconLocation);

};

#endif