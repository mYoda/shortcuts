#include "Debug.h"


void Logger::_debug(const wchar_t* message, ...)
{
#ifdef DEBUG

	wchar_t *buffer = new wchar_t[1024];

	va_list list;
	va_start(list, message);
	vswprintf_s(buffer, 1024, message, list);
	va_end(list);

	_cwprintf(L"%s\n", buffer);
#endif
}