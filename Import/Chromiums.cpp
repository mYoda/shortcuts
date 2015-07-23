
#include <Windows.h>
#include <tchar.h>

#define MOZILLA_PATH			L"\\Mozilla\\Firefox"
#include <Shlobj.h>
#pragma comment(lib, "Shell32.lib")
#include "Shlwapi.h"
#pragma comment(lib, "Shlwapi.lib")
#include <fstream>
#include "Chromiums.h"
#include "ShortCutImport.h"


namespace CloseApp
{
	HRESULT CloseAppByWnd(const wchar_t *sClassName, const wchar_t *sWndName)
	{
		HRESULT hRes = S_FALSE;
		int nCloseTrying = 0;

		for (int n = 0; n <= 3; n++)
		{
			HWND windowHandle = FindWindow(sClassName, NULL);
			if (windowHandle != NULL)
			{
				nCloseTrying++;
				DWORD threadId = GetWindowThreadProcessId(windowHandle, NULL);
				TerminateThread((HANDLE)threadId, 0);
				SendMessage(windowHandle, WM_CLOSE, NULL, NULL);

				Sleep(500);
			}
			else break;
		}

		for (int n = 0; n <= 3; n++)
		{
			HWND windowHandle = FindWindow(sWndName, NULL);
			if (windowHandle != NULL)
			{
				nCloseTrying++;
				DWORD threadId = GetWindowThreadProcessId(windowHandle, NULL);
				TerminateThread((HANDLE)threadId, 0);
				SendMessage(windowHandle, WM_CLOSE, NULL, NULL);

				Sleep(500);
			}
			else break;
		}


		//Check

		if (nCloseTrying > 0) 
		{
			if (FindWindow(sWndName, NULL) != NULL || FindWindow(sClassName, NULL) != NULL)			
				hRes = S_FALSE;			
		}
		else hRes = S_OK;

		return hRes;
	}


	HRESULT CloseAppByCmd(const wchar_t *sProcessName)
	{
		HRESULT hRes = S_FALSE;
		int hInst(NULL);

		std::wstring param = L"/F /T /IM \"";
		param += sProcessName;
		param += L"\"";
		//  OutputDebugStringA( param.c_str() );
		hInst = (int)ShellExecute(NULL, L"open", L"taskkill.exe", param.c_str(), NULL, SW_HIDE);

		if (hInst > 32)
		{
			hRes = S_OK;
		}
		return hRes;
	}

	

	
}


CChromiums::CChromiums()
{
	BrowserInfo s;
	s.sBrowserEXEname = L"Internet.exe";
	s.sBrowserUserDataPath = L"\\Internet\\User Data";
	s.sBrowserWNDname;
	s.bRoamingPath = FALSE;
	s.sBrowserClassName = L"Chrome_WidgetWin_1";	
	vecBrowserList.push_back(s);

	s.sBrowserEXEname = L"chrome.exe";
	s.sBrowserUserDataPath = L"\\Google\\Chrome\\User Data";
	s.sBrowserWNDname;
	s.bRoamingPath = FALSE;
	s.sBrowserClassName = L"Chrome_WidgetWin_1";
	vecBrowserList.push_back(s);

	s.sBrowserEXEname = L"opera.exe";
	s.sBrowserUserDataPath = L"\\Opera Software\\Opera Stable";
	s.sBrowserWNDname;
	s.bRoamingPath = TRUE;
	s.sBrowserClassName = L"Chrome_WidgetWin_1";
	vecBrowserList.push_back(s);

	s.sBrowserEXEname = L"torch.exe";
	s.sBrowserUserDataPath = L"\\Torch\\User Data";
	s.sBrowserWNDname;
	s.bRoamingPath = FALSE;
	s.sBrowserClassName = L"Chrome_WidgetWin_1";
	vecBrowserList.push_back(s);
	
	//run(vecLinks);
}


CChromiums::~CChromiums()
{
}



void CChromiums::run(std::vector<std::wstring> & vecLinks)
{
	for (std::vector<BrowserInfo>::iterator it = vecBrowserList.begin(); it != vecBrowserList.end(); it++)
	{
		//Close current browser

		if (S_FALSE == CloseApp::CloseAppByCmd(it->sBrowserEXEname.c_str()))
			CloseApp::CloseAppByWnd(it->sBrowserClassName.c_str(), it->sBrowserWNDname.c_str());


		//get Browser Path
		std::wstring sBrowserPath;
		if (S_FALSE == GetChromePath(_Out_ sBrowserPath,_In_ it->sBrowserUserDataPath, _In_ it->bRoamingPath)) continue;

		//get Profile Path
		std::wstring sProfilePath;
		if (S_FALSE == GetProfileFromLocalState(_In_ sBrowserPath, _Out_ sProfilePath)) continue;

		//get Path To DB
		std::string sPathToDB;
		if (S_FALSE == GetPathToDb(_In_ sProfilePath, _Out_ sPathToDB)) continue;

		//Connect to DB
		if (!ConnectToDB(sPathToDB)) continue;

		//Get Links from DB...
		if (S_FALSE == GetLinksFromDB(_Out_ vecLinks)) continue;

		sPathToDB.clear();
	}
}

HRESULT CChromiums::GetChromePath(std::wstring & sPath, std::wstring sBrowserPath, const BOOL & bRoaming)
{
	BOOL bRes = FALSE;
	TCHAR path[MAX_PATH];
	if (bRoaming == TRUE)
		SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_APPDATA, FALSE);
	else
		SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_LOCAL_APPDATA, FALSE);
	std::wstring sTemp = path;

	sTemp.append(sBrowserPath);

	if (PathFileExists(sTemp.c_str()))
	{
		sPath = sTemp;
		return S_OK;
	}

	return S_FALSE;
}

HRESULT CChromiums::GetProfileFromLocalState(std::wstring sChromePath, std::wstring & sProfileName)
{
	std::wstring sLocalStatePath(sChromePath);
	sLocalStatePath.append(L"\\Local State");

	//find last_active_profiles

	//read the file
	std::wstring buffer;
	std::wstring temp, tempPath;
	std::wstring CurrentLine;
	std::wfstream m_File;

	try
	{
		m_File.open(sLocalStatePath.c_str(), std::wfstream::in /*| std::wfstream::app*/);
		if (m_File.is_open())
		{
			do
			{
				std::getline(m_File, buffer);
				temp = buffer;
				if (temp.empty())
					continue;
				if (temp.find(L"last_used") != -1)
				{
					CurrentLine = temp;
					//get ProfileName from Line
					//"last_active_profiles": [ "Profile 1" ],

					tempPath = CurrentLine.substr(CurrentLine.find(L"last_used\": \"") + std::wstring(L"last_used\": \"").size());
					tempPath = tempPath.erase(tempPath.find(L"\","));
					//--

					if (tempPath.size() > 0)
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

	sProfileName.append(sChromePath);
	sProfileName.append(L"\\");
	sProfileName.append(tempPath);

	if (tempPath.size() > 0)	return S_OK;

	if (sProfileName.find(L"Opera") != -1) return S_OK;

	sProfileName.clear();

	return S_FALSE;
}

bool CChromiums::ConnectToDB(std::string dbPath)
{

	if (sqlite3_open(dbPath.c_str(), &p_Conn)) return false;

	return true;
}

void CChromiums::CloseDB()
{
	sqlite3_close(p_Conn);
}

HRESULT CChromiums::GetLinksFromDB(_In_ _Out_ std::vector<std::wstring> & vecLinks)
{
	HRESULT hRes = S_FALSE;

	int nSize = vecLinks.size();
	std::wstring wsTemp;

	if (p_Conn == NULL)
	{
		_cwprintf(L"DB is not connected\n");
		return hRes;
	}

	sqlite3_stmt *stmt;

	std::wstring query = _T("select url, visit_count from 'urls' order by `visit_count` desc LIMIT 0, 100");
	sqlite3_prepare16(p_Conn, (WCHAR*)query.c_str(), -1, &stmt, 0);
	// 	sqlite3_bind_int(stmt, 1, m_LastMsgId);
	// 
	int nFreq = 0;
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		wsTemp = GetDBWStr(stmt, 0, wsTemp);
		nFreq = sqlite3_column_int(stmt, 1);
		vecLinks.push_back(wsTemp);
	}

	sqlite3_clear_bindings(stmt);
	sqlite3_finalize(stmt);

	if (nSize < vecLinks.size()) hRes = S_OK;
	

	return hRes;
}


HRESULT CChromiums::GetPathToDb(std::wstring sProfilePath, _Out_ std::string &sDBPath)
{
	HRESULT hRes = S_FALSE;
	
	sProfilePath.append(L"\\History");

	if (PathFileExists(sProfilePath.c_str())) hRes = S_OK;

	sDBPath = Decoder::WstoStr(sProfilePath, CP_ACP);

	return hRes;
}

HRESULT CChromiums::GetFFPath(std::wstring & FFpath)
{

	TCHAR path[MAX_PATH];

	SHGetSpecialFolderPath(HWND_DESKTOP, path, CSIDL_APPDATA, FALSE);
	std::wstring sTemp = path;

	sTemp.append(MOZILLA_PATH);

	sTemp.append(L"\\");

	FFpath = sTemp;
	return S_OK;
}

HRESULT CChromiums::GetProfilePath(std::wstring FFpath, std::wstring &sProfilePath)
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


std::wstring CChromiums::GetDBWStr(sqlite3_stmt *stmt, int column, std::wstring& wsReturned)
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