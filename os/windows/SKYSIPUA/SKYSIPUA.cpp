// SKYSIPUA.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "SKYSIPUA.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


// This is an example of an exported variable
SKYSIPUA_API int nSKYSIPUA=0;

// This is an example of an exported function.
SKYSIPUA_API int fnSKYSIPUA(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see SKYSIPUA.h for the class definition
CSKYSIPUA::CSKYSIPUA()
{ 
	return; 
}

