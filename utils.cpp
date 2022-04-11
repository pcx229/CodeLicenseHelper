
#include "pch.h"
#include "utils.h"

HRESULT wsubstr(LPCWSTR str, LPCWSTR target, LPCWSTR replacement, LPWSTR out)
{
    int varSize = lstrlenW(target);
    LPCWSTR pos = wcsstr(str, target);
    if (pos == NULL)
    {
        lstrcpyW(out, str);
        return S_FALSE;
    }
    lstrcpyn(out, str, pos - str + 1);
    lstrcat(out, replacement);
    lstrcat(out, pos + varSize);
    return S_OK;
}

HRESULT exec(LPWSTR szCmdline, LPWSTR output, UINT maxOutputSize, DWORD* ec, DWORD timeout)
{
    HANDLE g_hChildStd_OUT_Rd = NULL;
    HANDLE g_hChildStd_OUT_Wr = NULL;

    BOOL hr;

    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    hr = CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0);
    if (!hr) 
    {
        return S_FALSE;
    }

    hr = SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);
    if (!hr) 
    {
        return S_FALSE;
    }

    PROCESS_INFORMATION piProcInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    STARTUPINFO siStartInfo;
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = g_hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    siStartInfo.hStdInput = NULL;
    siStartInfo.dwFlags |= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    siStartInfo.wShowWindow = SW_HIDE;

    hr = CreateProcess(NULL, szCmdline, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo);
    if (!hr)
    {
        return S_FALSE;
    }
    else
    {
        DWORD wr = WaitForSingleObject(piProcInfo.hProcess, timeout);
        if (wr != WAIT_OBJECT_0)
        {
            return S_FALSE;
        }
        hr = GetExitCodeProcess(piProcInfo.hProcess, ec);
        if (!hr)
        {
            return S_FALSE;
        }

        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);
        CloseHandle(g_hChildStd_OUT_Wr);
    }

    DWORD dwRead;
    CHAR chBuf[BUFSIZE];
    BOOL bSuccess = FALSE;

    int pos = 0;
    ZeroMemory(output, sizeof(WCHAR) * maxOutputSize);

    while (true)
    {
        bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
        if (!bSuccess || dwRead == 0)
        {
            break;
        }
        int nChars = MultiByteToWideChar(CP_ACP, 0, chBuf, dwRead, NULL, 0);
        if (nChars > maxOutputSize - pos)
        {
            nChars = maxOutputSize - pos;
        }
        MultiByteToWideChar(CP_ACP, 0, chBuf, dwRead, output + pos, nChars);
        pos += nChars;
    }

    return S_OK;
}

int MENU_ITEM_ICON_SIZE_WIDTH = GetSystemMetrics(SM_CXMENUCHECK),
    MENU_ITEM_ICON_SIZE_HEIGHT = GetSystemMetrics(SM_CYMENUCHECK);

HBITMAP LoadMenuIconAsBitmap(HINSTANCE hInst, LPCWSTR name) 
{
    HICON icon = (HICON)LoadImage(hInst, name, IMAGE_ICON, MENU_ITEM_ICON_SIZE_WIDTH, MENU_ITEM_ICON_SIZE_HEIGHT, LR_LOADFROMFILE | LR_DEFAULTSIZE);
    if (icon != NULL) {
        HBITMAP bitmap = LoadMenuIconAsBitmap(hInst, icon);
        DestroyIcon(icon);
        return bitmap;
    }
    return NULL;
}

HBITMAP LoadMenuIconAsBitmap(HINSTANCE hInst, UINT resIconId)
{
    HICON icon = LoadIcon(hInst, MAKEINTRESOURCE(resIconId));
    if (icon != NULL) {
        HBITMAP bitmap = LoadMenuIconAsBitmap(hInst, icon);
        DestroyIcon(icon);
        return bitmap;
    }
    return NULL;
}

HBITMAP LoadMenuIconAsBitmap(HINSTANCE hInst, HICON icon) 
{
    HDC hdc = CreateCompatibleDC(NULL);

    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = MENU_ITEM_ICON_SIZE_WIDTH;
    bmi.bmiHeader.biHeight = MENU_ITEM_ICON_SIZE_HEIGHT;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = MENU_ITEM_ICON_SIZE_WIDTH * MENU_ITEM_ICON_SIZE_HEIGHT * 4;

    VOID* pvBits;
    HBITMAP bitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0x0);
    if (bitmap != NULL) {
        SelectObject(hdc, bitmap);
        DrawIconEx(hdc, 0, 0, icon, MENU_ITEM_ICON_SIZE_WIDTH, MENU_ITEM_ICON_SIZE_HEIGHT, 0, NULL, DI_NORMAL);
    }

    DeleteDC(hdc);
    return bitmap;
}
// DestroyIcon(icon);
// DeleteObject(bitmap);