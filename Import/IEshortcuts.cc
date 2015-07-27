#include "CimportWorker.h"

#include <Urlhist.h>

#include <comdef.h>
//#include <afxtempl.h>
#include <Objbase.h>
#pragma comment (lib, "Ole32.lib")
#include <shlguid.h> // shell GUIDs
//#include "stdhdrs.h"/*
#include <ctime>

#include <Shlwapi.h>

#pragma comment (lib, "Shlwapi.lib")
#include "IEshortcuts.h"

IExplorer::IExplorer()
{
}

IExplorer::~IExplorer()
{
}

BOOL IExplorer::getLinksIE(std::vector<std::wstring> & sVecLinks)
{
	CoInitialize(NULL);


	IUrlHistoryStg* ieHistory = NULL;
	HRESULT hr = CoCreateInstance(CLSID_CUrlHistory, NULL, CLSCTX_ALL, IID_IUrlHistoryStg, (LPVOID *)(&ieHistory));

	if (!SUCCEEDED(hr))
		return 1;

	IEnumSTATURL* urlEnumer;
	hr = ieHistory->EnumUrls(&urlEnumer);

	if (!SUCCEEDED(hr))
		return 2;

	if (!SUCCEEDED(urlEnumer->SetFilter(L"", STATURL_QUERYFLAG_ISCACHED | STATURL_QUERYFLAG_TOPLEVEL | STATURLFLAG_ISTOPLEVEL | STATURLFLAG_ISCACHED)))
		return 4;

	if (!SUCCEEDED(urlEnumer->Reset()))
		return 3;

	unsigned long correct = 0;
	STATURL result = {};

	time_t t = time(0);   // get time now
	struct tm * now = localtime(&t);


	std::vector<Urls> urls;
	//std::vector<std::wstring> sVecLinks;
	int nIters = 0;
	do
	{
		result.cbSize = sizeof(result);
		if (SUCCEEDED(urlEnumer->Next(1, &result, &correct)))
		{
			Urls url;
			nIters++;
			if (result.pwcsUrl)
			{
				url.url = (result.pwcsUrl);
			}
			else continue;

			if (result.pwcsTitle)
			{
				url.title = result.pwcsTitle;
			}
			else continue;

			url.statUrl = result;

			urls.push_back(url);
			sVecLinks.push_back(url.url);
		}
	} while (correct == 1);

	size_t size = urls.size();
	ieHistory->Release();
	urlEnumer->Release();

	CoUninitialize();

	if (sVecLinks.size() <= 0) return FALSE;
	

	//////////////////////////////////////////////////////////////////////////

	return TRUE;
}
