#pragma once

#include <windows.h>

#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <strsafe.h>

#define BUFSIZE 4096

HBITMAP LoadMenuIconAsBitmap(HINSTANCE hInst, UINT resIconId);
HBITMAP LoadMenuIconAsBitmap(HINSTANCE hInst, LPCWSTR name);
HBITMAP LoadMenuIconAsBitmap(HINSTANCE hInst, HICON icon);

HRESULT exec(LPWSTR command, LPWSTR output, UINT maxOutputSize, DWORD* ec, DWORD timeout);

HRESULT wsubstr(LPCWSTR str, LPCWSTR target, LPCWSTR replacement, LPWSTR out);