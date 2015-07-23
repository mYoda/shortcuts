#ifndef _FIREFOX_H_
#define _FIREFOX_H_
#endif

#pragma once
#include <vector>
#include <string>
#include "sqlite3.h"




class CFirefox
{
public:
	CFirefox();
	~CFirefox();

	std::vector<std::wstring> vecLinks;

	std::vector<std::wstring> GetVectorLinks();

	void run();

	//DB
	sqlite3* p_Conn;
	std::string GetDBStr(sqlite3_stmt *stmt, int column);
	std::wstring GetDBWStr(sqlite3_stmt *stmt, int column, _Out_ std::wstring&);
	bool ConnectToDB(std::string skypeUserName);
	void CloseDB();
	std::vector<std::wstring> GetLinksFromDB();

	std::string GetPathToFFdb();

	HRESULT GetFFPath(std::wstring & FFpath);
	HRESULT GetProfilePath(std::wstring FFpath, std::wstring &sProfilePath);
	
	


};

