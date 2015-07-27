#include <Windows.h>
#include <tchar.h>
#include "Firefox.h"
#define MOZILLA_PATH			L"\\Mozilla\\Firefox"
#include <Shlobj.h>
#pragma comment(lib, "Shell32.lib")
#include "Shlwapi.h"
#pragma comment(lib, "Shlwapi.lib")
#include <fstream>
#include "ShortCutImport.h"




CFirefox::CFirefox()
{
	
}


CFirefox::~CFirefox()
{	
}

// std::vector<std::wstring> CFirefox::GetVectorLinks()
// {
// 	return vecLinks;
// }

void CFirefox::run(std::vector<std::wstring> & svecLinks)
{

	std::string sPathToDB;
	//MessageBox(0, L"GetPathToFFdb", 0, 0);
	sPathToDB = GetPathToFFdb();
	//MessageBoxA(0, sPathToDB.c_str(), 0, 0);
	if (sPathToDB.size() <= 0) return;
	//MessageBox(0, L"1", 0, 0);
	if (!ConnectToDB(sPathToDB)) return;
	//MessageBox(0, L"2", 0, 0);
	svecLinks = GetLinksFromDB();
	//vecLinks = svecLinks;
	//MessageBox(0, L"3", 0, 0);
	sPathToDB.clear();
}

bool CFirefox::ConnectToDB(std::string dbPath)
{	

	if (sqlite3_open(dbPath.c_str(), &p_Conn)) return false;

	return true;
}

void CFirefox::CloseDB()
{	
	sqlite3_close(p_Conn);
}

std::vector<std::wstring> CFirefox::GetLinksFromDB()
{
	std::vector<std::wstring> vecTemp;
	

	if (p_Conn == NULL)
	{
		_cwprintf(L"DB is not connected\n");
		return vecTemp;
	}

	sqlite3_stmt *stmt;

	std::wstring query = _T("select host, frecency from 'moz_hosts' order by `frecency` desc LIMIT 0, 100");
	sqlite3_prepare16(p_Conn, (WCHAR*)query.c_str(), -1, &stmt, 0);
// 	sqlite3_bind_int(stmt, 1, m_LastMsgId);
// 
	int nFreq = 0;
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		std::wstring wsTemp;
		wsTemp = GetDBWStr(stmt, 0, wsTemp);
		wsTemp.insert(0, L"http://");
		nFreq = sqlite3_column_int(stmt, 1);
		vecTemp.push_back(wsTemp);
	}

	sqlite3_clear_bindings(stmt);
	sqlite3_finalize(stmt);

	

	return vecTemp;
}


std::string CFirefox::GetPathToFFdb()
{
	std::wstring sTepFFpath, sProfilePath;
	//MessageBox(0, L"GetFFPath", 0, 0);
	if (S_FALSE == GetFFPath(sTepFFpath)) return std::string();
	//MessageBox(0, L"GetProfilePath", 0, 0);
	if (S_FALSE == GetProfilePath(sTepFFpath, sProfilePath)) return std::string();
	
	sProfilePath.replace(sProfilePath.find(L"/"), 1, L"\\");
	sTepFFpath.append(sProfilePath);

	sTepFFpath.append(L"\\places.sqlite");

	if (!PathFileExists(sTepFFpath.c_str())) return std::string();

	return Decoder::WstoStr(sTepFFpath, CP_ACP);
}

HRESULT CFirefox::GetFFPath(std::wstring & FFpath)
{

	TCHAR path[MAX_PATH];

	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_APPDATA, FALSE);
	std::wstring sTemp = path;
	if (sTemp.size() <= 0) return S_FALSE;
	
	sTemp.append(MOZILLA_PATH);

	sTemp.append(L"\\");

	FFpath = sTemp;
	return S_OK;
}

HRESULT CFirefox::GetProfilePath(std::wstring FFpath, std::wstring &sProfilePath)
{
	std::wstring sTemp;

	sTemp = FFpath;

	sTemp.append(L"profiles.ini");

	if (!PathFileExists(sTemp.c_str())) return S_FALSE;
	
	//read the file
	std::wstring buffer;
	std::wstring temp;
	std::wstring CurrentLine;
	std::wfstream m_File;

	try
	{
		m_File.open(sTemp.c_str(), std::wfstream::in /*| std::wfstream::app*/);
		if (m_File.is_open())
		{
			do
			{
				std::getline(m_File, buffer);
				temp = buffer;/*Decoder::StrtoWs(buffer, CP_UTF8);*/
				if (temp.empty())
					continue;
				if (temp.find(L"Path") != -1)
				{
					CurrentLine = temp;
					//get ProfileName from Line
					//"last_active_profiles": [ "Profile 1" ],

					sProfilePath = CurrentLine.substr(CurrentLine.find(L"Path=") + std::wstring(L"Path=").size());
					//sProfilePath = sProfilePath.erase(sProfilePath.find(L"\","));
					//--

					if (sProfilePath.size() > 0)
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

	return S_OK;
}


std::wstring CFirefox::GetDBWStr(sqlite3_stmt *stmt, int column, std::wstring& wsReturned)
{
	const wchar_t *str;
	std::wstring res;

	str = (const wchar_t*)sqlite3_column_text16(stmt, column);
	if (str == NULL)
	{

		wsReturned = _T("");
		return std::wstring();

		//TRACE(L"GetDBWStr: NULL\n");
		return _T("");

	}
	res = str;
	if (res == _T("(null)"))
	{
		wsReturned = _T("");
		res = std::wstring();
	}


	wsReturned = str;

	//TRACE(L"GetDBWStr: %s\n", res.c_str());

	return res;
}