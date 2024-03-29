﻿// dllmain.cpp : Implementation of DllMain.

#include "pch.h"
#include "framework.h"
#include "resource.h"
#include "CodeLicenseHelper_i.h"
#include "dllmain.h"
#include "xdlldata.h"

CCodeLicenseHelperModule _AtlModule;

HMODULE hModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(hInstance, dwReason, lpReserved))
		return FALSE;
#endif
	hInstance;
	hModule = hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved);
}
