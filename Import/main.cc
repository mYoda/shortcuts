
#include "includes.h"
#include "CimportWorker.h"
//#include "Firefox.h"
#include "ShortCutImport.h"
#include <Urlhist.h>
//#pragma comment (lib, "Shdocvw.dll")
#include <comdef.h>
//#include <afxtempl.h>
#include <Objbase.h>
#pragma comment (lib, "Ole32.lib")
#include <shlguid.h> // shell GUIDs
//#include "stdhdrs.h"
#include <ctime>

#include <Shlwapi.h>

#pragma comment (lib, "Shlwapi.lib")

#include "LinksPlaying.h"




int WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{


	std::vector<std::wstring> files;

	


	CimportWorker * impTest = new CimportWorker();

	std::wstring sProfilePath;

	impTest->GetProfilePath(_Out_ sProfilePath);
//	if (S_FALSE == impTest->CopyFilesToOurDir(INTERNET_DIR, sProfilePath.c_str())) return -1;

	
	//openAction->Enabled = ((err == S_OK) || (err == S_FALSE)) && (size > 0);

	
	CLinksPlaying * pLinks = new CLinksPlaying();

//	pLinks->run();
	
	

	CShortCutImport * pTemp = new CShortCutImport();

	pTemp->CompareLinks();

	return 0;
}
