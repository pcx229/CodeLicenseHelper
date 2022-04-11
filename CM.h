// CM.h : Declaration of the CM

#pragma once
#include "resource.h"	 // main symbols

#include <windows.h>
#include <propsys.h>     // Property System APIs and interfaces
#include <shobjidl.h>    // IContextMenu, IShellExtInit
#include <msxml6.h>		 // Contains the latest version of MSXML
#include <Shlobj.h>		 // need to include definitions of constants for files paths

#include "CodeLicenseHelper_i.h"

#include "dllmain.h"

#include "utils.h"

#include <iostream>
#include <map>
#include <vector>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

// CCM

class ATL_NO_VTABLE CCM :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCM, &CLSID_CCM>,
	public IContextMenu,
	public IShellExtInit
{
public:
	CCM() : loadedSettings(false), getTypeCommand(NULL), setTypeCommand(NULL), filterTypes(false)
	{
		path = new WCHAR[MAX_PATH];
		loadSettings();
	}

	~CCM()
	{
		delete[] path;
		for (auto const& i : stringsW) {
			delete[] i.second;
		}
		for (auto const& i : stringsA) {
			delete[] i.second;
		}
		for (auto const& i : icons) {
			DeleteObject(i.second);
		}

		for (auto const& i : allowedFilesNames) {
			SysFreeString(i);
		}

		for (auto const& i : allowedFilesExtensions) {
			SysFreeString(i);
		}

		if (getTypeCommand) {
			SysFreeString(getTypeCommand->command);
			delete getTypeCommand;
		}

		if (setTypeCommand) {
			SysFreeString(setTypeCommand->command);
			delete setTypeCommand;
		}

		for (auto const& i : licenses) {
			SysFreeString(i->name);
			delete i;
		}
	}

DECLARE_REGISTRY_RESOURCEID(106)

BEGIN_COM_MAP(CCM)
	COM_INTERFACE_ENTRY(IContextMenu)
	COM_INTERFACE_ENTRY(IShellExtInit)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

    // IContextMenu
    HRESULT STDMETHODCALLTYPE QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) override;
    HRESULT STDMETHODCALLTYPE InvokeCommand(CMINVOKECOMMANDINFO* pici) override;
    HRESULT STDMETHODCALLTYPE GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pReserved, CHAR* pszName, UINT cchMax) override;

    // IShellExtInit
    IFACEMETHODIMP Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID) override;

private:
	LPWSTR path;

	static const int MAX_STRING_LENGTH = 256;
	static const int MAX_COMMAND_LENGTH = MAX_PATH + MAX_PATH + 2048;
	LPCWSTR SETTING_IN_USER_PROFILE_FILE_PATH = L"code-license.config.xml";

	std::map<UINT, LPWSTR> stringsW;
	std::map<UINT, LPSTR> stringsA;
	std::map<UINT, HBITMAP> icons;

	struct License {
		LPWSTR name;
		BOOL selectable;
	};
	std::vector<License*> licenses;

	struct Command {
		LPWSTR command;
		UINT timeout; // in miliseconds
	};
	Command *getTypeCommand, 
				*setTypeCommand,
					*checkTypeCommand,
						*repairTypeCommand;

	std::vector<LPWSTR> allowedFilesExtensions,
							allowedFilesNames;
	bool filterTypes;
	bool pathIsAllowd;

	bool loadedSettings;

	const enum menuCopyrightItemId { MII_CHECK, MII_REPAIR, MII_ABOUT, MII_LICENSE_TYPE };

	void loadSettings();
	HBITMAP getIcon(UINT resId);
	LPWSTR getString(UINT resId);
	LPWSTR getStringW(UINT resId);
	LPSTR getStringA(UINT resId);
};

OBJECT_ENTRY_AUTO(__uuidof(CCM), CCM)

