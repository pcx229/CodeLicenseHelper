
#include "pch.h"
#include "CM.h"
#include "resource.h"
#include <msxml6.h>

HRESULT STDMETHODCALLTYPE CCM::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
    if (!(CMF_DEFAULTONLY & uFlags) && loadedSettings && pathIsAllowd)
    {
        /*
        menu:
            Code License ->
                        [ ] license 1
                        [x] license 2
                        [ ] licence 3
                        [ ] ignore
                        [ ] //unknown//
                        --------------
                        Check
                        Repair
                        About
        */

        HMENU licenseSubMenu = CreatePopupMenu();
        
		USHORT licenseSubMenuIndex = 0,
				licenseItemIdCmd = MII_LICENSE_TYPE;

        // check file license type
        static const int BUFFER_SIZE = 4096;
        WCHAR licenseItemSelected[BUFFER_SIZE];
        if (getTypeCommand != NULL)
        {
            DWORD ec = 0;
            HRESULT hr = exec(getTypeCommand->command, licenseItemSelected, BUFFER_SIZE, &ec, getTypeCommand->timeout);
            if (FAILED(hr) || ec != 0)
            {
                return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NULL, 0);
            }
        }
		else
		{
			return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NULL, 0);
		}

        // avilable licenses list
        bool foundSelected = false;
        for (auto const& l : licenses)
        {
            UINT flags = MF_STRING | MF_BYPOSITION;
            if (l->selectable) 
            {
                flags |= MF_ENABLED;
            }
            else
            {
                flags |= MF_DISABLED | MF_GRAYED;
            }
            if (licenseItemSelected != NULL && wcsncmp(licenseItemSelected, l->name, MAX_STRING_LENGTH) == 0)
            {
                flags |= MF_CHECKED;
                foundSelected = true;
            }
            else
            {
                flags |= MF_UNCHECKED;
            }
            InsertMenu(licenseSubMenu, ++licenseSubMenuIndex, flags, idCmdFirst + ++licenseItemIdCmd, l->name);
        }

        if (!foundSelected) {
            return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NULL, 0);
        }

        InsertMenu(licenseSubMenu, licenseSubMenuIndex, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
        InsertMenu(licenseSubMenu, ++licenseSubMenuIndex, MF_STRING | MF_BYPOSITION, idCmdFirst + MII_CHECK, getString(IDS_CONTEXT_MENU_CHECK));
        SetMenuItemBitmaps(licenseSubMenu, licenseSubMenuIndex, MF_BYPOSITION, getIcon(IDI_CONTEXT_MENU_CHECK), NULL);
        InsertMenu(licenseSubMenu, ++licenseSubMenuIndex, MF_STRING | MF_BYPOSITION, idCmdFirst + MII_REPAIR, getString(IDS_CONTEXT_MENU_REPAIR));
        SetMenuItemBitmaps(licenseSubMenu, licenseSubMenuIndex, MF_BYPOSITION, getIcon(IDI_CONTEXT_MENU_REPAIR), NULL);
        InsertMenu(licenseSubMenu, ++licenseSubMenuIndex, MF_STRING | MF_BYPOSITION, idCmdFirst + MII_ABOUT, getString(IDS_CONTEXT_MENU_ABOUT));
        SetMenuItemBitmaps(licenseSubMenu, licenseSubMenuIndex, MF_BYPOSITION, getIcon(IDI_CONTEXT_MENU_ABOUT), NULL);

        InsertMenu(hmenu, indexMenu, MF_STRING | MF_POPUP | MF_BYPOSITION, (UINT_PTR)licenseSubMenu, getString(IDS_CONTEXT_MENU_CODE_LICENSE));
        SetMenuItemBitmaps(hmenu, indexMenu, MF_BYPOSITION, getIcon(IDI_CONTEXT_MENU_CODE_LICENSE), NULL);
        
        return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, idCmdFirst + licenseItemIdCmd + 1);
    }

    return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, USHORT(0));
}

HRESULT STDMETHODCALLTYPE CCM::InvokeCommand(CMINVOKECOMMANDINFO* pici)
{
    UINT idOffset = LOWORD(pici->lpVerb);

	WCHAR cmd[MAX_COMMAND_LENGTH],
		temp[MAX_COMMAND_LENGTH];
	LPWSTR command = NULL;

	switch (idOffset)
	{
	case MII_CHECK:
		if (checkTypeCommand != NULL)
		{
			command = checkTypeCommand->command;
		}
		wsubstr(command, L"$1", path, temp);
		break;
	case MII_REPAIR:
		if (repairTypeCommand != NULL)
		{
			command = repairTypeCommand->command;
		}
		wsubstr(command, L"$1", path, temp);
		break;
	case MII_ABOUT:
		MessageBox(pici->hwnd, getString(IDS_CONTEXT_MENU_ABOUT_MESSAGE_BOX_MESSAGE), getString(IDS_CONTEXT_MENU_ABOUT_MESSAGE_BOX_CAPTION), MB_OK | MB_ICONINFORMATION);
		break;
	}
	if (idOffset >= MII_LICENSE_TYPE)
	{
		command = setTypeCommand->command;
		LPWSTR licenseType = licenses[idOffset - MII_LICENSE_TYPE]->name;
		wsubstr(command, L"$1", path, temp);
		wsubstr(temp, L"$2", licenseType, cmd);
		lstrcpyW(temp, cmd);
	}
	if (command)
	{
		_swprintf_p(cmd, MAX_COMMAND_LENGTH, L"/k \"%s\"", temp);
		ShellExecute(pici->hwnd, NULL, L"cmd.exe", cmd, NULL, SW_HIDE);
	}
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CCM::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT* pReserved, CHAR* pszName, UINT cchMax)
{
    if (uFlags & GCS_HELPTEXT)
    {
        // Windows NT will ask for Unicode strings
        switch (idCmd)
        {
        case MII_CHECK:
			if (uFlags & GCS_VERBA)
			{
				lstrcpynA(pszName, getStringA(IDS_CONTEXT_MENU_CHECK_COMMAND_HELP), cchMax);
			}
			else
			{
				lstrcpynW((LPWSTR)pszName, getStringW(IDS_CONTEXT_MENU_CHECK_COMMAND_HELP), cchMax);
			}
            break;
        case MII_REPAIR:
			if (uFlags & GCS_VERBA)
			{
				lstrcpynA(pszName, getStringA(IDS_CONTEXT_MENU_REPAIR_COMMAND_HELP), cchMax);
			}
			else
			{
				lstrcpynW((LPWSTR)pszName, getStringW(IDS_CONTEXT_MENU_REPAIR_COMMAND_HELP), cchMax);
			}
            break;
		case MII_ABOUT:
			if (uFlags & GCS_VERBA)
			{
				lstrcpynA(pszName, getStringA(IDS_CONTEXT_MENU_ABOUT_COMMAND_HELP), cchMax);
			}
			else
			{
				lstrcpynW((LPWSTR)pszName, getStringW(IDS_CONTEXT_MENU_ABOUT_COMMAND_HELP), cchMax);
			}
			break;
        }
		if (idCmd >= MII_LICENSE_TYPE)
		{
			if (uFlags & GCS_VERBA)
			{
				lstrcpynA(pszName, getStringA(IDS_CONTEXT_MENU_CODE_LICENSE_COMMAND_HELP), cchMax);
			}
			else
			{
				lstrcpynW((LPWSTR)pszName, getStringW(IDS_CONTEXT_MENU_CODE_LICENSE_COMMAND_HELP), cchMax);
			}
		}
    }

    return S_OK;
}

HRESULT CCM::Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject* lpdobj, HKEY hkeyProgID)
{
    if (lpdobj == NULL)
        return E_INVALIDARG;

    STGMEDIUM medium;
    FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    HRESULT hr = lpdobj->GetData(&fe, &medium);
    if (FAILED(hr))
        return E_INVALIDARG;

    DragQueryFile(reinterpret_cast<HDROP>(medium.hGlobal), 0, path, MAX_PATH);

	if (filterTypes)
	{
		pathIsAllowd = false;
		LPCWSTR name = fileName(path);
		LPCWSTR extension = fileExtension(path);
		if (extension != NULL)
		{
			for (const auto& t : allowedFilesExtensions)
			{
				if (wcsncmp(extension, t, MAX_PATH) == 0)
				{
					pathIsAllowd = true;
				}
			}
		}
		if (name != NULL)
		{
			for (const auto& t : allowedFilesNames)
			{
				if (wcsncmp(name, t, MAX_PATH) == 0)
				{
					pathIsAllowd = true;
				}
			}
		}
	}
	else
	{
		pathIsAllowd = true;
	}

    ReleaseStgMedium(&medium);
    return hr;
}


void CCM::loadSettings() {
	HRESULT hr;

	WCHAR settingsFilePath[MAX_PATH];
	TCHAR userProfilePath[MAX_PATH];
	hr = SHGetSpecialFolderPath(NULL, userProfilePath, CSIDL_PROFILE, FALSE);
	if (FAILED(hr)) {
		return;
	}
	wcsncpy_s(settingsFilePath, userProfilePath, MAX_PATH);
	wcsncat_s(settingsFilePath, (const wchar_t*)"\\", MAX_PATH);
	wcsncat_s(settingsFilePath, SETTING_IN_USER_PROFILE_FILE_PATH, MAX_PATH);

	hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		CComPtr<IXMLDOMDocument> iXMLDoc;
		hr = iXMLDoc.CoCreateInstance(__uuidof(DOMDocument));
		if (SUCCEEDED(hr))
		{

			// licenses

			VARIANT_BOOL vfSuccess = VARIANT_FALSE;
			hr = iXMLDoc->load(CComVariant(settingsFilePath), &vfSuccess);
			if (SUCCEEDED(hr) && vfSuccess == VARIANT_TRUE)
			{
				CComPtr<IXMLDOMNodeList> lValueList;
				hr = iXMLDoc->selectNodes(CComBSTR(L"Settings/Licenses/Type"), &lValueList);
				if (hr == S_OK)
				{
					long cValues = 0;
					hr = lValueList->get_length(&cValues);
					if (SUCCEEDED(hr))
					{
						for (long i = 0; i < cValues; i++)
						{
							CComPtr<IXMLDOMNode> lValue;
							hr = lValueList->get_item(i, &lValue);
							if (SUCCEEDED(hr))
							{
								CComPtr<IXMLDOMElement> pElement;
								hr = lValue->QueryInterface(IID_PPV_ARGS(&pElement));
								if (SUCCEEDED(hr))
								{
									CComBSTR name;
									hr = lValue->get_text(&name);
									if (SUCCEEDED(hr))
									{
										License* license = new License();
										license->name = (LPWSTR)name.Copy();
										license->selectable = true;

										CComBSTR sDesc("Selectable");
										CComVariant deVal(VT_EMPTY);
										hr = pElement->getAttribute(sDesc, &deVal);
										if (hr == S_OK)
										{
											if (deVal == CComVariant(L"false")) {
												license->selectable = false;
											}
										}
										licenses.push_back(license);
									}
								}
							}
						}
					}
				}

				// commands

				auto getCommand = [iXMLDoc](LPCOLESTR loc) {
					CComPtr<IXMLDOMNode> gtcValue;
					HRESULT hr = iXMLDoc->selectSingleNode(CComBSTR(loc), &gtcValue);
					if (hr == S_OK)
					{
						CComPtr<IXMLDOMElement> pElement;
						hr = gtcValue->QueryInterface(IID_PPV_ARGS(&pElement));
						if (SUCCEEDED(hr))
						{
							CComBSTR cmd;
							hr = gtcValue->get_text(&cmd);
							if (SUCCEEDED(hr))
							{
								Command* c = new Command();
								c->command = (LPWSTR)cmd.Copy();
								c->timeout = INFINITE;

								CComBSTR tValue;
								CComBSTR tDesc("Timeout");
								CComVariant deVal(VT_EMPTY);
								hr = pElement->getAttribute(tDesc, &deVal);
								if (hr == S_OK)
								{
									deVal.CopyTo(&tValue);
									if (SUCCEEDED(hr))
									{
										c->timeout = wcstol((LPWSTR)tValue.m_str, nullptr, 10);
									}
								}

								return c;
							}
						}
					}
					return (Command*)NULL;
				};

				getTypeCommand = getCommand(L"Settings/Get-Command");
				setTypeCommand = getCommand(L"Settings/Set-Command");
				checkTypeCommand = getCommand(L"Settings/Check-Command");
				repairTypeCommand = getCommand(L"Settings/Repair-Command");

				// fileTypes

				CComPtr<IXMLDOMNode> tfExValue;
				hr = iXMLDoc->selectSingleNode(CComBSTR(L"Settings/Files"), &tfExValue);
				if (hr == S_OK)
				{
					filterTypes = true;

					CComPtr<IXMLDOMNodeList> fteValueList;
					hr = iXMLDoc->selectNodes(CComBSTR(L"Settings/Files/Extension"), &fteValueList);
					if (hr == S_OK)
					{
						long cValues = 0;
						hr = fteValueList->get_length(&cValues);
						if (SUCCEEDED(hr))
						{
							for (long i = 0; i < cValues; i++)
							{
								CComPtr<IXMLDOMNode> lValue;
								hr = fteValueList->get_item(i, &lValue);
								if (SUCCEEDED(hr))
								{
									CComBSTR name;
									hr = lValue->get_text(&name);
									if (SUCCEEDED(hr))
									{
										allowedFilesExtensions.push_back((LPWSTR)name.Copy());
									}
								}
							}
						}
					}

					CComPtr<IXMLDOMNodeList> ftnValueList;
					hr = iXMLDoc->selectNodes(CComBSTR(L"Settings/Files/Name"), &ftnValueList);
					if (hr == S_OK)
					{
						long cValues = 0;
						hr = ftnValueList->get_length(&cValues);
						if (SUCCEEDED(hr))
						{
							for (long i = 0; i < cValues; i++)
							{
								CComPtr<IXMLDOMNode> lValue;
								hr = ftnValueList->get_item(i, &lValue);
								if (SUCCEEDED(hr))
								{
									CComBSTR name;
									hr = lValue->get_text(&name);
									if (SUCCEEDED(hr))
									{
										allowedFilesNames.push_back((LPWSTR)name.Copy());
									}
								}
							}
						}
					}
				}

				loadedSettings = true;
			}
		}
	}
	CoUninitialize();
}

HBITMAP CCM::getIcon(UINT resId) {
	if (!icons.count(resId)) {
		icons[resId] = LoadMenuIconAsBitmap(hModule, resId);
	}
	return icons[resId];
}

LPSTR CCM::getStringA(UINT resId) {
	if (!icons.count(resId)) {
		LPSTR str = new CHAR[MAX_STRING_LENGTH];
		LoadStringA(hModule, resId, str, MAX_STRING_LENGTH);
		stringsA[resId] = str;
	}
	return stringsA[resId];
}

LPWSTR CCM::getStringW(UINT resId) {
	if (!icons.count(resId)) {
		LPWSTR str = new WCHAR[MAX_STRING_LENGTH];
		LoadString(hModule, resId, str, MAX_STRING_LENGTH);
		stringsW[resId] = str;
	}
	return stringsW[resId];
}

LPWSTR CCM::getString(UINT resId) {
	return getStringW(resId);
}
