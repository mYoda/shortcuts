#ifndef _VERSION_H_
#define _VERSION_H_

#define BUFSIZE				256

#pragma once
#include "includes.h"
class CVersion
{
public:
	CVersion();
	~CVersion();

	static enum OSVERSION
	{
		Unknown,
		WinXP,
		WinVista,
		Win7,
		Win8,
		Win8_1,
		Win10
	};


	static OSVERSION GetVersion();

	static OSVERSIONINFOEX OSVersion(HANDLE handle);


	static CVersion * GeInstance()
	{
		CVersion * instance = new CVersion();
		return instance;
	}

};

#endif