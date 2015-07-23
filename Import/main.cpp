
#include "includes.h"
#include "CimportWorker.h"
//#include "Firefox.h"
#include "ShortCutImport.h"


int WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPSTR lpCmdLine, __in int nShowCmd )
{

// 	CimportWorker * impTest = new CimportWorker();
// 
// 	std::wstring sProfilePath;
// 
// 	impTest->GetProfilePath(_Out_ sProfilePath);
// 	if (S_FALSE == impTest->CopyFilesToOurDir(INTERNET_DIR, sProfilePath.c_str())) return -1;


	CShortCutImport * pTemp = new CShortCutImport();

	pTemp->CompareLinks();

	return 0;
}