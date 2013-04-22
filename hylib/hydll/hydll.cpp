// hydll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "hydll.h"

#ifndef _USING_MFC
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
#endif // _USING_MFC

// This is an example of an exported variable
HYDLL_API int nHydll=0;

// This is an example of an exported function.
HYDLL_API int fnHydll(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see hydll.h for the class definition
CHydll::CHydll()
{ 
	return; 
}

