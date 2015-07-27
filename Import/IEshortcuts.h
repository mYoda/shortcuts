
#include <Urlhist.h>

class IExplorer
{
public:
	IExplorer();
	~IExplorer();

	struct Urls
	{
		std::wstring url;
		std::wstring title;
		STATURL statUrl;

		//Urls(LPWSTR url, LPWSTR title) : url(url ? url : L""), title(title ? title : L"")
		//{}
	};

	static BOOL getLinksIE(std::vector<std::wstring> & sVecLinks);
};

