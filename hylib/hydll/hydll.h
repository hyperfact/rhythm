
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the HYDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// HYDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef HYDLL_EXPORTS
#define HYDLL_API __declspec(dllexport)
#else
#define HYDLL_API __declspec(dllimport)
#endif

// This class is exported from the hydll.dll
class HYDLL_API CHydll {
public:
	CHydll(void);
	// TODO: add your methods here.
};

extern HYDLL_API int nHydll;

HYDLL_API int fnHydll(void);

