
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SKYSIPUA_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SKYSIPUA_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef SKYSIPUA_EXPORTS
#define SKYSIPUA_API __declspec(dllexport)
#else
#define SKYSIPUA_API __declspec(dllimport)
#endif

// This class is exported from the SKYSIPUA.dll
class SKYSIPUA_API CSKYSIPUA {
public:
	CSKYSIPUA(void);
	// TODO: add your methods here.
};

extern SKYSIPUA_API int nSKYSIPUA;

SKYSIPUA_API int fnSKYSIPUA(void);

