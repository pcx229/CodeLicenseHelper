// dllmain.h : Declaration of module class.

class CCodeLicenseHelperModule : public ATL::CAtlDllModuleT< CCodeLicenseHelperModule >
{
public :
	DECLARE_LIBID(LIBID_CodeLicenseHelperLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_CODELICENSEHELPER, "{13a585ae-6f9c-4fc8-a2fa-c4b7001a52f4}")
};

extern class CCodeLicenseHelperModule _AtlModule;

extern HMODULE hModule;
