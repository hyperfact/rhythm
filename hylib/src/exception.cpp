/*  
*   Copyright (c) 2002-2003, author: canco, email: canco_99@sina.com.   

You are free to use it, study it, trash it, modify it, sell it with or without changes,   
or do whatever else you want, with one restrictions and one polite requests:  
Restriction 1: It's all your fault.  
These samples are _samples_. They may do something useful, or may help in understanding   
a problem; or they may not. If they do not, I would appreciate if you let me know, so   
I can fix the sample; but I recognise no obligation on my part for doing that. As I   
said, it's all your fault.  
Polite request 1: Give credit.  
If you appropriate one of my samples, I don't even ask to be named: but I do ask that you   
don't claim credit for my work.  

NOTE: Code of stack trace comes from StackWalk program written by Felix Kasza. Felix Kasza's  
web address is http://www.mvps.org/win32/.  

*/   

#include "stdafx.h"
#include "Exception.h"   
#include <iostream>   
#include <time.h>
#include <io.h>
using namespace std;   


#define gle (GetLastError())   
#define lenof(a) (sizeof(a) / sizeof((a)[0]))   
#define MAXNAMELEN 1024 // max name length for found symbols   
#define IMGSYMLEN ( sizeof IMAGEHLP_SYMBOL )   
#define TTBUFLEN 65536 // for a temp buffer   

HINSTANCE hImagehlpDll = NULL;

// SymCleanup()   
typedef BOOL (__stdcall *tSC)( IN HANDLE hProcess );   
tSC pSC = NULL;   

// SymFunctionTableAccess()   
typedef PVOID (__stdcall *tSFTA)( HANDLE hProcess, DWORD AddrBase );   
tSFTA pSFTA = NULL;   

// SymGetLineFromAddr()   
typedef BOOL (__stdcall *tSGLFA)( IN HANDLE hProcess, IN DWORD dwAddr,   
								 OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE Line );   
tSGLFA pSGLFA = NULL;   

// SymGetModuleBase()   
typedef DWORD (__stdcall *tSGMB)( IN HANDLE hProcess, IN DWORD dwAddr );   
tSGMB pSGMB = NULL;   

// SymGetModuleInfo()   
typedef BOOL (__stdcall *tSGMI)( IN HANDLE hProcess, IN DWORD dwAddr, OUT PIMAGEHLP_MODULE ModuleInfo );   
tSGMI pSGMI = NULL;   

// SymGetOptions()   
typedef DWORD (__stdcall *tSGO)( VOID );   
tSGO pSGO = NULL;   

// SymGetSymFromAddr()   
typedef BOOL (__stdcall *tSGSFA)( IN HANDLE hProcess, IN DWORD dwAddr,   
								 OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_SYMBOL Symbol );   
tSGSFA pSGSFA = NULL;   

// SymInitialize()   
typedef BOOL (__stdcall *tSI)( IN HANDLE hProcess, IN PSTR UserSearchPath, IN BOOL fInvadeProcess );   
tSI pSI = NULL;   

// SymLoadModule()   
typedef DWORD (__stdcall *tSLM)( IN HANDLE hProcess, IN HANDLE hFile,   
								IN PSTR ImageName, IN PSTR ModuleName, IN DWORD BaseOfDll, IN DWORD SizeOfDll );   
tSLM pSLM = NULL;   

// SymSetOptions()   
typedef DWORD (__stdcall *tSSO)( IN DWORD SymOptions );   
tSSO pSSO = NULL;   

// StackWalk()   
typedef BOOL (__stdcall *tSW)( DWORD MachineType, HANDLE hProcess,   
							  HANDLE hThread, LPSTACKFRAME StackFrame, PVOID ContextRecord,   
							  PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine,   
							  PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine,   
							  PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine,   
							  PTRANSLATE_ADDRESS_ROUTINE TranslateAddress );   
tSW pSW = NULL;   

// UnDecorateSymbolName()   
typedef DWORD (__stdcall WINAPI *tUDSN)( PCSTR DecoratedName, PSTR UnDecoratedName,   
										DWORD UndecoratedLength, DWORD Flags );   
tUDSN pUDSN = NULL;

//******************************************************************************
TCHAR szAppPath[_MAX_PATH] = {0};
TCHAR szAppBaseName[_MAX_PATH] = {0};
HINSTANCE hDbgHelpDll = NULL;
// MiniDumpWriteDump
typedef BOOL (WINAPI *tMDWD)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
										 CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
										 CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
										 CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
										 );
tMDWD pMDWD = NULL;

struct ModuleEntry   
{   
	std::string imageName;   
	std::string moduleName;   
	DWORD baseAddress;   
	DWORD size;   
};   
typedef std::vector< ModuleEntry > ModuleList;   
typedef ModuleList::iterator ModuleListIter;   

// miscellaneous toolhelp32 declarations; we cannot #include the header   
// because not all systems may have it   
#define MAX_MODULE_NAME32 255   
#define TH32CS_SNAPMODULE   0x00000008   
#pragma pack( push, 8 )   
typedef struct tagMODULEENTRY32   
{   
	DWORD   dwSize;   
	DWORD   th32ModuleID;       // This module   
	DWORD   th32ProcessID;      // owning process   
	DWORD   GlblcntUsage;       // Global usage count on the module   
	DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context   
	BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context   
	DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr   
	HMODULE hModule;            // The hModule of this module in th32ProcessID's context   
	char    szModule[MAX_MODULE_NAME32 + 1];   
	char    szExePath[MAX_PATH];   
} MODULEENTRY32;   
typedef MODULEENTRY32 *  PMODULEENTRY32;   
typedef MODULEENTRY32 *  LPMODULEENTRY32;   
#pragma pack( pop )   

namespace dbg_util {

static void ShowStack( HANDLE hThread, CONTEXT& c ); // dump a stack   
static DWORD Filter( EXCEPTION_POINTERS *ep );   
static void enumAndLoadModuleSymbols( HANDLE hProcess, DWORD pid );   
static bool fillModuleList( ModuleList& modules, DWORD pid, HANDLE hProcess );   
static bool fillModuleListTH32( ModuleList& modules, DWORD pid );   
static bool fillModuleListPSAPI( ModuleList& modules, DWORD pid, HANDLE hProcess );   


void ShowStack( HANDLE hThread, CONTEXT& c )   
{   
	// normally, call ImageNtHeader() and use machine info from PE header   
	DWORD imageType = IMAGE_FILE_MACHINE_I386;   
	HANDLE hProcess = GetCurrentProcess(); // hProcess normally comes from outside   
	int frameNum; // counts walked frames   
	DWORD offsetFromSymbol; // tells us how far from the symbol we were   
	DWORD symOptions; // symbol handler settings   
	IMAGEHLP_SYMBOL *pSym = (IMAGEHLP_SYMBOL *) malloc( IMGSYMLEN + MAXNAMELEN );   
	char undName[MAXNAMELEN]; // undecorated name   
	char undFullName[MAXNAMELEN]; // undecorated name with all shenanigans   
	IMAGEHLP_MODULE Module;   
	IMAGEHLP_LINE Line;   
	std::string symSearchPath;   
	char *tt = 0, *p;   

	cout << "-------------------### Begin to trace stack ###----------------------" <<endl;   

	STACKFRAME s; // in/out stackframe   
	memset( &s, '\0', sizeof s );   

	// NOTE: normally, the exe directory and the current directory should be taken   
	// from the target process. The current dir would be gotten through injection   
	// of a remote thread; the exe fir through either ToolHelp32 or PSAPI.   

	tt = new char[TTBUFLEN]; // this is a _sample_. you can do the error checking yourself.   

	// build symbol search path from:   
	symSearchPath = "";   
	// current directory   
	if ( GetCurrentDirectory( TTBUFLEN, tt ) )   
		symSearchPath += tt + std::string( ";" );   
	// dir with executable   
	if ( GetModuleFileName( 0, tt, TTBUFLEN ) )   
	{   
		for ( p = tt + strlen( tt ) - 1; p >= tt; -- p )   
		{   
			// locate the rightmost path separator   
			if ( *p == '\\' || *p == '/' || *p == ':' )   
				break;   
		}   
		// if we found one, p is pointing at it; if not, tt only contains   
		// an exe name (no path), and p points before its first byte   
		if ( p != tt ) // path sep found?   
		{   
			if ( *p == ':' ) // we leave colons in place   
				++ p;   
			*p = '\0'; // eliminate the exe name and last path sep   
			symSearchPath += tt + std::string( ";" );   
		}   
	}   
	// environment variable _NT_SYMBOL_PATH   
	if ( GetEnvironmentVariable( "_NT_SYMBOL_PATH", tt, TTBUFLEN ) )   
		symSearchPath += tt + std::string( ";" );   
	// environment variable _NT_ALTERNATE_SYMBOL_PATH   
	if ( GetEnvironmentVariable( "_NT_ALTERNATE_SYMBOL_PATH", tt, TTBUFLEN ) )   
		symSearchPath += tt + std::string( ";" );   
	// environment variable SYSTEMROOT   
	if ( GetEnvironmentVariable( "SYSTEMROOT", tt, TTBUFLEN ) )   
		symSearchPath += tt + std::string( ";" );   

	if ( symSearchPath.size() > 0 ) // if we added anything, we have a trailing semicolon   
		symSearchPath = symSearchPath.substr( 0, symSearchPath.size() - 1 );   

	cout <<  "symbols path: " << symSearchPath << endl;   

	// why oh why does SymInitialize() want a writeable string?   
	strncpy( tt, symSearchPath.c_str(), TTBUFLEN );   
	tt[TTBUFLEN - 1] = '\0'; // if strncpy() overruns, it doesn't add the null terminator   

	// init symbol handler stuff (SymInitialize())   
	if ( ! pSI( hProcess, tt, false ) )   
	{   
		cout << "SymInitialize(): gle = " << gle << endl;   
		goto cleanup;   
	}   

	// SymGetOptions()   
	symOptions = pSGO();   
	symOptions |= SYMOPT_LOAD_LINES;   
	symOptions &= ~SYMOPT_UNDNAME;   
	pSSO( symOptions ); // SymSetOptions()   

	// Enumerate modules and tell imagehlp.dll about them.   
	// On NT, this is not necessary, but it won't hurt.   
	enumAndLoadModuleSymbols( hProcess, GetCurrentProcessId() );   

	// init STACKFRAME for first call   
	// Notes: AddrModeFlat is just an assumption. I hate VDM debugging.   
	// Notes: will have to be #ifdef-ed for Alphas; MIPSes are dead anyway,   
	// and good riddance.   
	s.AddrPC.Offset = c.Eip;   
	s.AddrPC.Mode = AddrModeFlat;   
	s.AddrFrame.Offset = c.Ebp;   
	s.AddrFrame.Mode = AddrModeFlat;   

	memset( pSym, '\0', IMGSYMLEN + MAXNAMELEN );   
	pSym->SizeOfStruct = IMGSYMLEN;   
	pSym->MaxNameLength = MAXNAMELEN;   

	memset( &Line, '\0', sizeof Line );   
	Line.SizeOfStruct = sizeof Line;   

	memset( &Module, '\0', sizeof Module );   
	Module.SizeOfStruct = sizeof Module;   

	offsetFromSymbol = 0;   

	cout << "\n--# FV EIP----- RetAddr- FramePtr StackPtr Symbol\n" ;   
	for ( frameNum = 0; ; ++ frameNum )   
	{   
		// get next stack frame (StackWalk(), SymFunctionTableAccess(), SymGetModuleBase())   
		// if this returns ERROR_INVALID_ADDRESS (487) or ERROR_NOACCESS (998), you can   
		// assume that either you are done, or that the stack is so hosed that the next   
		// deeper frame could not be found.   
		if ( ! pSW( imageType, hProcess, hThread, &s, &c, NULL,   
			pSFTA, pSGMB, NULL ) )   
			break;   

		// display its contents   
		cout <<  "\n"   
			<< frameNum << " " << (s.Far? 'F': '.') << (s.Virtual? 'V': '.')   
			<< " ";   
		cout.width( 8 ); cout.fill( '0' );   
		cout << hex << s.AddrPC.Offset << " " ;   
		cout.width( 8 ); cout.fill( '0' );   
		cout << hex << s.AddrReturn.Offset << " " ;   
		cout.width( 8 ); cout.fill( '0' );   
		cout << hex << s.AddrFrame.Offset << " " ;   
		cout.width( 8 ); cout.fill( '0' );   
		cout << hex<< s.AddrStack.Offset ;   
		cout << dec;   

		if ( s.AddrPC.Offset == 0 )   
		{   
			cout <<  "(-no symbols- PC == 0)\n" ;   
		}   
		else   
		{ // we seem to have a valid PC   
			// show procedure info (SymGetSymFromAddr())   
			if ( ! pSGSFA( hProcess, s.AddrPC.Offset, &offsetFromSymbol, pSym ) )   
			{   
				if ( gle != 487 )   
					cout << "SymGetSymFromAddr(): gle = " << gle << endl;   
			}   
			else   
			{   
				// UnDecorateSymbolName()   
				pUDSN( pSym->Name, undName, MAXNAMELEN, UNDNAME_NAME_ONLY );   
				pUDSN( pSym->Name, undFullName, MAXNAMELEN, UNDNAME_COMPLETE );   

				cout << " " << undFullName ;   

				if ( offsetFromSymbol != 0 )   
					cout << " + " << (long) offsetFromSymbol << " bytes" << endl;   
				//              putchar( '\n' );   
				cout << "    Sig:  " << pSym->Name << endl ;   
				cout << "    Decl: " << undFullName << endl;   
			}   

			// show line number info, NT5.0-method (SymGetLineFromAddr())   
			if ( pSGLFA != NULL )   
			{ // yes, we have SymGetLineFromAddr()   
				if ( ! pSGLFA( hProcess, s.AddrPC.Offset, &offsetFromSymbol, &Line ) )   
				{   
					if ( gle != 487 )   
						cout << "SymGetLineFromAddr(): gle = " << gle << endl;   

					DWORD dError = gle;   
				}   
				else   
				{   
					cout <<  "    Line: " << Line.FileName << "("   
						<< Line.LineNumber << ") + " <<  offsetFromSymbol   
						<< " bytes" << endl;   

				}   
			} // yes, we have SymGetLineFromAddr()   

			// show module info (SymGetModuleInfo())   
			if ( ! pSGMI( hProcess, s.AddrPC.Offset, &Module ) )   
			{   
				cout <<  "SymGetModuleInfo): gle = " << gle << endl;   
			}   
			else   
			{ // got module info OK   
				char ty[80];   
				switch ( Module.SymType )   
				{   
				case SymNone:   
					strcpy( ty, "-nosymbols-" );   
					break;   
				case SymCoff:   
					strcpy( ty, "COFF" );   
					break;   
				case SymCv:   
					strcpy( ty, "CV" );   
					break;   
				case SymPdb:   
					strcpy( ty, "PDB" );   
					break;   
				case SymExport:   
					strcpy( ty, "-exported-" );   
					break;   
				case SymDeferred:   
					strcpy( ty, "-deferred-" );   
					break;   
				case SymSym:   
					strcpy( ty, "SYM" );   
					break;   
				default:   
					_snprintf( ty, sizeof ty, "symtype=%ld", (long) Module.SymType );   
					break;   
				}   

				cout << "    Mod:  " << Module.ModuleName << "["   
					<< Module.ImageName << "], base: 0x";   
				cout.width( 8 ) ;   
				cout.fill( '0' );   
				cout << hex << Module.BaseOfImage   
					<< "h" << endl;   
				cout << dec;   

				cout << "    Sym:  type: " << ty << ", file: " <<    
					Module.LoadedImageName << endl;   
			} // got module info OK   
		} // we seem to have a valid PC   

		// no return address means no deeper stackframe   
		if ( s.AddrReturn.Offset == 0 )   
		{   
			// avoid misunderstandings in the printf() following the loop   
			SetLastError( 0 );   
			break;   
		}   

	} // for ( frameNum )   

	if ( gle != 0 )   
		cout << "\nStackWalk(): gle = " << gle << endl;   

	cout << "-------------------### End to trace stack ###----------------------" <<endl;   

cleanup:   
	ResumeThread( hThread );   
	// de-init symbol handler etc. (SymCleanup())   
	pSC( hProcess );   
	free( pSym );   
	delete [] tt;   
}   



void enumAndLoadModuleSymbols( HANDLE hProcess, DWORD pid )   
{   
	ModuleList modules;   
	ModuleListIter it;   
	char *img, *mod;   

	// fill in module list   
	fillModuleList( modules, pid, hProcess );   

	for ( it = modules.begin(); it != modules.end(); ++ it )   
	{   
		// unfortunately, SymLoadModule() wants writeable strings   
		img = new char[(*it).imageName.size() + 1];   
		strcpy( img, (*it).imageName.c_str() );   
		mod = new char[(*it).moduleName.size() + 1];   
		strcpy( mod, (*it).moduleName.c_str() );   

		if ( pSLM( hProcess, 0, img, mod, (*it).baseAddress, (*it).size ) == 0 )   
			cout << "Error " << gle << " loading symbols for " <<    
			(*it).moduleName << endl;   
		else   
			cout << "Symbols loaded: " << (*it).moduleName << endl;   

		delete [] img;   
		delete [] mod;   
	}   
}   



bool fillModuleList( ModuleList& modules, DWORD pid, HANDLE hProcess )   
{   
	// try toolhelp32 first   
	if ( fillModuleListTH32( modules, pid ) )   
		return true;   
	// nope? try psapi, then   
	return fillModuleListPSAPI( modules, pid, hProcess );   
}   






bool fillModuleListTH32( ModuleList& modules, DWORD pid )   
{   
	// CreateToolhelp32Snapshot()   
	typedef HANDLE (__stdcall *tCT32S)( DWORD dwFlags, DWORD th32ProcessID );   
	// Module32First()   
	typedef BOOL (__stdcall *tM32F)( HANDLE hSnapshot, LPMODULEENTRY32 lpme );   
	// Module32Next()   
	typedef BOOL (__stdcall *tM32N)( HANDLE hSnapshot, LPMODULEENTRY32 lpme );   

	// I think the DLL is called tlhelp32.dll on Win9X, so we try both   
	const char *dllname[] = { "kernel32.dll", "tlhelp32.dll" };   
	HINSTANCE hToolhelp;   
	tCT32S pCT32S;   
	tM32F pM32F;   
	tM32N pM32N;   

	HANDLE hSnap;   
	MODULEENTRY32 me = { sizeof me };   
	bool keepGoing;   
	ModuleEntry e;   
	int i;   

	for ( i = 0; i < lenof( dllname ); ++ i )   
	{   
		hToolhelp = LoadLibrary( dllname[i] );   
		if ( hToolhelp == 0 )   
			continue;   
		pCT32S = (tCT32S) GetProcAddress( hToolhelp, "CreateToolhelp32Snapshot" );   
		pM32F = (tM32F) GetProcAddress( hToolhelp, "Module32First" );   
		pM32N = (tM32N) GetProcAddress( hToolhelp, "Module32Next" );   
		if ( pCT32S != 0 && pM32F != 0 && pM32N != 0 )   
			break; // found the functions!   
		FreeLibrary( hToolhelp );   
		hToolhelp = 0;   
	}   

	if ( hToolhelp == 0 ) // nothing found?   
		return false;   

	hSnap = pCT32S( TH32CS_SNAPMODULE, pid );   
	if ( hSnap == (HANDLE) -1 )   
		return false;   

	keepGoing = !!pM32F( hSnap, &me );   
	while ( keepGoing )   
	{   
		// here, we have a filled-in MODULEENTRY32   
		cout << "0x";   
		cout.width( 8 );   
		cout.fill( '0' );   
		cout <<  hex << (int)me.modBaseAddr;   
		cout << "h";   
		cout << dec;   

		//      printf( "%08lXh %6lu %-15.15s %s\n", me.modBaseAddr,    
		//          me.modBaseSize, me.szModule, me.szExePath );   
		cout << " " << me.modBaseSize << " " << me.szModule << " "   
			<< me.szExePath << endl;   
		e.imageName = me.szExePath;   
		e.moduleName = me.szModule;   
		e.baseAddress = (DWORD) me.modBaseAddr;   
		e.size = me.modBaseSize;   
		modules.push_back( e );   
		keepGoing = !!pM32N( hSnap, &me );   
	}   

	CloseHandle( hSnap );   

	FreeLibrary( hToolhelp );   

	return modules.size() != 0;   
}   



// miscellaneous psapi declarations; we cannot #include the header   
// because not all systems may have it   
typedef struct _MODULEINFO {   
	LPVOID lpBaseOfDll;   
	DWORD SizeOfImage;   
	LPVOID EntryPoint;   
} MODULEINFO, *LPMODULEINFO;   



bool fillModuleListPSAPI( ModuleList& modules, DWORD pid, HANDLE hProcess )   
{   
	// EnumProcessModules()   
	typedef BOOL (__stdcall *tEPM)( HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded );   
	// GetModuleFileNameEx()   
	typedef DWORD (__stdcall *tGMFNE)( HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize );   
	// GetModuleBaseName() -- redundant, as GMFNE() has the same prototype, but who cares?   
	typedef DWORD (__stdcall *tGMBN)( HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize );   
	// GetModuleInformation()   
	typedef BOOL (__stdcall *tGMI)( HANDLE hProcess, HMODULE hModule, LPMODULEINFO pmi, DWORD nSize );   

	HINSTANCE hPsapi;   
	tEPM pEPM;   
	tGMFNE pGMFNE;   
	tGMBN pGMBN;   
	tGMI pGMI;   

	int i;   
	ModuleEntry e;   
	DWORD cbNeeded;   
	MODULEINFO mi;   
	HMODULE *hMods = 0;   
	char *tt = 0;   

	hPsapi = LoadLibrary( "psapi.dll" );   
	if ( hPsapi == 0 )   
		return false;   

	modules.clear();   

	pEPM = (tEPM) GetProcAddress( hPsapi, "EnumProcessModules" );   
	pGMFNE = (tGMFNE) GetProcAddress( hPsapi, "GetModuleFileNameExA" );   
	pGMBN = (tGMFNE) GetProcAddress( hPsapi, "GetModuleBaseNameA" );   
	pGMI = (tGMI) GetProcAddress( hPsapi, "GetModuleInformation" );   
	if ( pEPM == 0 || pGMFNE == 0 || pGMBN == 0 || pGMI == 0 )   
	{   
		// yuck. Some API is missing.   
		FreeLibrary( hPsapi );   
		return false;   
	}   

	hMods = new HMODULE[TTBUFLEN / sizeof HMODULE];   
	tt = new char[TTBUFLEN];   
	// not that this is a sample. Which means I can get away with   
	// not checking for errors, but you cannot. :)   

	if ( ! pEPM( hProcess, hMods, TTBUFLEN, &cbNeeded ) )   
	{   
		cout <<  "EPM failed, gle = " << gle  << endl;   
		goto cleanup;   
	}   

	if ( cbNeeded > TTBUFLEN )   
	{   
		cout << "More than " << lenof( hMods) << " module handles. Huh?" << endl;   
		goto cleanup;   
	}   

	for ( i = 0; i < cbNeeded / sizeof hMods[0]; ++ i )   
	{   
		// for each module, get:   
		// base address, size   
		pGMI( hProcess, hMods[i], &mi, sizeof mi );   
		e.baseAddress = (DWORD) mi.lpBaseOfDll;   
		e.size = mi.SizeOfImage;   
		// image file name   
		tt[0] = '\0';   
		pGMFNE( hProcess, hMods[i], tt, TTBUFLEN );   
		e.imageName = tt;   
		// module name   
		tt[0] = '\0';   
		pGMBN( hProcess, hMods[i], tt, TTBUFLEN );   
		e.moduleName = tt;   

		cout.width( 8 ); cout.fill( '0');   
		cout <<  hex << e.baseAddress ;   
		cout << dec;   
		cout << " " << e.size << " "   
			<< e.moduleName << " " << e.imageName << endl;   


		modules.push_back( e );   
	}   

cleanup:   
	if ( hPsapi )   
		FreeLibrary( hPsapi );   
	delete [] tt;   
	delete [] hMods;   

	return modules.size() != 0;   
}   

void PrintStack()
{
	HANDLE hThread ;   
	DuplicateHandle( GetCurrentProcess(), GetCurrentThread(),   
		GetCurrentProcess(), &hThread, 0, false, DUPLICATE_SAME_ACCESS );   

	CONTEXT Context;   
	memset( &Context, '\0', sizeof( Context ) );   
	Context.ContextFlags = CONTEXT_FULL;   

	// init CONTEXT record so we know where to start the stackwalk   
	if ( ! GetThreadContext( hThread, &Context ) )   
	{   
		cout <<  "GetThreadContext(): " << GetLastError() << endl;;   
	}   

	DWORD iError = GetLastError();   

	ShowStack( hThread, Context );
	::CloseHandle(hThread);
}


bool CWinUtil::vInitEnviroment() {
	do {
		// we load imagehlp.dll dynamically because the NT4-version does not   
		// offer all the functions that are in the NT5 lib   
		hImagehlpDll = LoadLibrary( "imagehlp.dll" );   
		if ( hImagehlpDll == NULL )   
		{   
			cout << "LoadLibrary imagehlp.dll failure." << endl;   
			break;   
		}   

		pSC = (tSC) GetProcAddress( hImagehlpDll, "SymCleanup" );   
		pSFTA = (tSFTA) GetProcAddress( hImagehlpDll, "SymFunctionTableAccess" );   
		pSGLFA = (tSGLFA) GetProcAddress( hImagehlpDll, "SymGetLineFromAddr" );   
		pSGMB = (tSGMB) GetProcAddress( hImagehlpDll, "SymGetModuleBase" );   
		pSGMI = (tSGMI) GetProcAddress( hImagehlpDll, "SymGetModuleInfo" );   
		pSGO = (tSGO) GetProcAddress( hImagehlpDll, "SymGetOptions" );   
		pSGSFA = (tSGSFA) GetProcAddress( hImagehlpDll, "SymGetSymFromAddr" );   
		pSI = (tSI) GetProcAddress( hImagehlpDll, "SymInitialize" );   
		pSSO = (tSSO) GetProcAddress( hImagehlpDll, "SymSetOptions" );   
		pSW = (tSW) GetProcAddress( hImagehlpDll, "StackWalk" );   
		pUDSN = (tUDSN) GetProcAddress( hImagehlpDll, "UnDecorateSymbolName" );   
		pSLM = (tSLM) GetProcAddress( hImagehlpDll, "SymLoadModule" );   

		if ( pSC == NULL || pSFTA == NULL || pSGMB == NULL || pSGMI == NULL ||   
			pSGO == NULL || pSGSFA == NULL || pSI == NULL || pSSO == NULL ||   
			pSW == NULL || pUDSN == NULL || pSLM == NULL )   
		{   
			cout <<  "GetProcAddress(): some required function not found." << endl;   
			FreeLibrary( hImagehlpDll );
			break;   
		}
	} while (0);

	do {
		if (GetModuleFileName(NULL, szAppPath, _MAX_PATH))
		{
			TCHAR *pSlash = _tcsrchr( szAppPath, '\\' );
			if (pSlash)
			{
				_tcsncpy(szAppBaseName, pSlash + 1, _MAX_PATH);
				szAppBaseName[lenof(szAppBaseName)-1] = _T('\0');
				*(pSlash+1) = 0;
			}
		}

		hDbgHelpDll = ::LoadLibrary(_T("DBGHELP.DLL"));
		if (hDbgHelpDll)
		{
			// If we haven't found it yet - try one more time.
			pMDWD = (tMDWD)::GetProcAddress(hDbgHelpDll, "MiniDumpWriteDump");

			if (NULL == pMDWD) {
				::FreeLibrary(hDbgHelpDll);
				hDbgHelpDll = NULL;
			}
		}
	} while(0);

#if 0
	vSetThrowNewException();
#endif
	vMapSEHtoCE();
	vSetThrowFloatException();
	vSetUnExpectedExceptionFilter();

	return true;
}

void CWinUtil::vUnInitEnviroment()
{
	if (hImagehlpDll) {
		FreeLibrary( hImagehlpDll );
		hImagehlpDll = NULL;
	}
	if (hDbgHelpDll) {
		FreeLibrary( hDbgHelpDll );
		hDbgHelpDll = NULL;
	}
}

CSEHException::CSEHException(UINT code, PEXCEPTION_POINTERS pep )   
	: exception()
	, m_exceptionCode(code)
	, m_pExceptionInfo(pep)
{
	m_exceptionCode = code;
	m_exceptionRecord = *pep->ExceptionRecord;   
	m_context = *pep->ContextRecord ;   

#if 0
	HANDLE hThread;
	DuplicateHandle( GetCurrentProcess(), GetCurrentThread(),
		GetCurrentProcess(), &hThread, 0, false, DUPLICATE_SAME_ACCESS );
	ShowStack( hThread, m_context );
	CloseHandle( hThread );
#endif

	switch ( code ) {
		//  ���ڴ��йص��쳣��   
	case EXCEPTION_ACCESS_VIOLATION:   
		m_strMsg.append( " �߳���ͼ����δ�����Ƿ��ڴ���쳣!" );   
		break;   
	case EXCEPTION_DATATYPE_MISALIGNMENT:   
		m_strMsg.append( " �߳���ͼ����д��֧�ֶ����Ӳ���ϵ�δ���������!" );   
		break;   
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:   
		m_strMsg.append( " �߳���ͼ��ȡһ��Խ�������Ԫ��!" );   
		break;   
	case EXCEPTION_IN_PAGE_ERROR:   
		m_strMsg.append( " �����ļ�ϵͳ��һ���豸�������򷵻�һ����������ɲ�������Ҫ���ҳ����!" );   
		break;   
	case EXCEPTION_GUARD_PAGE:   
		m_strMsg.append( " �߳���ͼ��ȡһ������PAGE_GUARD�������Ե��ڴ�ҳ!" );   
		break;   
	case EXCEPTION_ILLEGAL_INSTRUCTION:   
		m_strMsg.append( " �߳�ִ����һ����Ч��ָ��!" );   
		break;   
	case EXCEPTION_PRIV_INSTRUCTION:   
		m_strMsg.append( " �߳�ִ����һ����ǰ����ģʽ�������ָ��!" );   
		break;   

		//  ��ṹ���쳣��ص��쳣��   
	case EXCEPTION_INVALID_DISPOSITION:   
		m_strMsg.append( " �쳣�����������˴����ֵ!" );   
		break;   
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:   
		m_strMsg.append( " �쳣��������һ�����ܼ������쳣���ص�EXCEPTION_CONTINUE_EXCEPTION!" );   
		break;   

		//  �������йص��쳣��   
	case EXCEPTION_INT_DIVIDE_BY_ZERO:   
		m_strMsg.append( " �����������쳣!" );   
		break;   
	case EXCEPTION_INT_OVERFLOW:   
		m_strMsg.append( " һ�����������Ľ������������ֵ�涨�ķ�Χ!" );   
		break;   

		//  �븡�����йص��쳣��   
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:   
		m_strMsg.append( " �����������쳣" );   
		break;   
	case EXCEPTION_FLT_DENORMAL_OPERAND:   
		m_strMsg.append( " ��������е�һ��������������!" );   
		break;   
	case EXCEPTION_FLT_INEXACT_RESULT:   
		m_strMsg.append( " ��������Ľṹ���ܾ�ȷ��ʾ��ʮ����С��!" );   
		break;   
	case EXCEPTION_FLT_INVALID_OPERATION:   
		m_strMsg.append( " ��ʾ�κ�û���ڴ��г��������������쳣!" );   
		break;   
	case EXCEPTION_FLT_OVERFLOW:   
		m_strMsg.append( " ��������Ľṹ�����������ֵ!" );   
		break;   
	case EXCEPTION_FLT_STACK_CHECK:   
		m_strMsg.append( " ���ڸ���������ջ���!" );   
		break;   
	case EXCEPTION_FLT_UNDERFLOW:   
		m_strMsg.append( " ��������Ľ��С�������ֵ!" );   
		break;   

		//  ���ָܻ��Ľṹ���쳣��ջ���   
	case EXCEPTION_STACK_OVERFLOW:   
		m_strMsg.append( " ջ����쳣!" );   
		break;   
	default:   
		m_strMsg.append( " ����δ֪���͵Ľṹ���쳣!" );   
		break;   
	}
}

void _cdecl CWinUtil::TranslateSEHtoCE( UINT code, PEXCEPTION_POINTERS pep ) {

	if ( code == EXCEPTION_STACK_OVERFLOW ||   
		code  == EXCEPTION_FLT_STACK_CHECK ) {
		throw CUnRecoverableSEHException( code, pep );
	} else {
		throw CRecoverableSEHException( code, pep );   
	}
}   

void CWinUtil::vSetUnExpectedExceptionFilter()   
{   
	SetUnhandledExceptionFilter( UnExpectedExceptionFilter );   
}   

long CWinUtil::UnExpectedExceptionFilter( LPEXCEPTION_POINTERS pe ){   

	HANDLE hThread;   
	DuplicateHandle( GetCurrentProcess(), GetCurrentThread(),   
		GetCurrentProcess(), &hThread, 0, false, DUPLICATE_SAME_ACCESS );   

	ShowStack( hThread, *pe->ContextRecord );
	::CloseHandle(hThread);
	return EXCEPTION_EXECUTE_HANDLER;   
}   


int CWinUtil::NewHandler( size_t size ) {   
	throw bad_alloc( "operator new couldn't allocate memroy." );   
	return 0;   
}   

void CWinUtil::vMapSEHtoCE() {   
	_set_se_translator( TranslateSEHtoCE );
}   

void CWinUtil::vSetThrowNewException() {   
	_set_new_handler( NewHandler );   
	_set_new_mode( 1 );   
}   


void CWinUtil::vCheckMemoryLeak() {   
	//  ����ڴ�й¶��   
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );   
}   

void CWinUtil::vSetThrowFloatException(){   
	int iCw = _controlfp( 0, 0 );   
	iCw &= ~( EM_OVERFLOW | EM_UNDERFLOW  | EM_INEXACT | EM_ZERODIVIDE   
		| EM_DENORMAL | EM_INVALID );   

	_controlfp( iCw, MCW_EM );   
}   

CUserException::CUserException( const string& str )   
: domain_error( str.c_str() )    
{   

	HANDLE hThread ;   
	DuplicateHandle( GetCurrentProcess(), GetCurrentThread(),   
		GetCurrentProcess(), &hThread, 0, false, DUPLICATE_SAME_ACCESS );   

	CONTEXT Context;   
	memset( &Context, '\0', sizeof( Context ) );   
	Context.ContextFlags = CONTEXT_FULL;   

	// init CONTEXT record so we know where to start the stackwalk   
	if ( ! GetThreadContext( hThread, &Context ) )   
	{   
		cout <<  "GetThreadContext(): " << GetLastError() << endl;;   
	}   

	DWORD iError = GetLastError();
	ShowStack( hThread, Context );

	::CloseHandle(hThread);
}

LPCTSTR _GenDumpFileName(LPTSTR szPath, int nLen) {
	time_t lTime;
	::time(&lTime);
	int n = 1;
	do {
		_snprintf(szPath, nLen,
			_T("%s%s.%ld-%d.dmp"), szAppPath, szAppBaseName, lTime, n++);
	} while(0 == _access(szPath, 0));
	return szPath;
}

CString WriteMiniDump(PEXCEPTION_POINTERS pExceptionInfo )
{
	try {
		if (hDbgHelpDll && pMDWD) {
			TCHAR szDumpPath[_MAX_PATH] = {0};
			_GenDumpFileName(szDumpPath, _MAX_PATH);

			// create the file
			HANDLE hFile = ::CreateFile( szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL, NULL );

			if (hFile!=INVALID_HANDLE_VALUE)
			{
				_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

				ExInfo.ThreadId = ::GetCurrentThreadId();
				ExInfo.ExceptionPointers = pExceptionInfo;
				ExInfo.ClientPointers = NULL;

				// write the dump
				pMDWD( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL );
				::CloseHandle(hFile);
			}

			return szDumpPath;
		}
	} catch (...) {}
	return "Failed";
}

} // namespace dbg_util
