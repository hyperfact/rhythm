#ifndef _DBG_UTIL_H_
#define _DBG_UTIL_H_

#include <imagehlp.h>
#pragma comment(lib, "dbghelp.lib")
#include <atlcoll.h>

#ifndef _countof
# define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif // _countof

#define _DBG_UTIL_THREAD_DUMP

namespace dbg_util {
	__interface IStackDumpHandler
	{
	public:
		void __stdcall OnBegin();
		void __stdcall OnEntry(void *pvAddress, LPCSTR szModule, LPCSTR szSymbol);
		void __stdcall OnError(LPCSTR szError);
		void __stdcall OnEnd();
	};
	
	#define ATL_MODULE_NAME_LEN _MAX_PATH
	#define ATL_SYMBOL_NAME_LEN 1024
	
	// Helper class for generating a stack dump
	// This is used internally by AtlDumpStack
	class CStackDumper
	{
	public:
		struct _ATL_SYMBOL_INFO
		{
			ULONG_PTR dwAddress;
			ULONG_PTR dwOffset;
			CHAR	szModule[ATL_MODULE_NAME_LEN];
			CHAR	szSymbol[ATL_SYMBOL_NAME_LEN];
		};
	
		static LPVOID __stdcall FunctionTableAccess( HANDLE hProcess, ULONG_PTR dwPCAddress)
		{
	#ifdef _WIN64
			return SymFunctionTableAccess(hProcess, dwPCAddress);
	#else
			return SymFunctionTableAccess(hProcess, (ULONG)dwPCAddress);
	#endif
		}
	
		static ULONG_PTR __stdcall GetModuleBase( HANDLE hProcess, ULONG_PTR dwReturnAddress)
		{
			IMAGEHLP_MODULE moduleInfo;
			moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE);
	
	#ifdef _WIN64
			if (SymGetModuleInfo(hProcess, dwReturnAddress, &moduleInfo))
	#else
			if (SymGetModuleInfo(hProcess, (ULONG)dwReturnAddress, &moduleInfo))
	#endif
				return moduleInfo.BaseOfImage;
			else
			{
				MEMORY_BASIC_INFORMATION memoryBasicInfo;
	
				if (::VirtualQueryEx(hProcess, (LPVOID) dwReturnAddress,
					&memoryBasicInfo, sizeof(memoryBasicInfo)))
				{
					DWORD cch = 0;
					char szFile[MAX_PATH] = { 0 };
	
					cch = GetModuleFileNameA((HINSTANCE)memoryBasicInfo.AllocationBase,
						szFile, MAX_PATH);
	
					// Ignore the return code since we can't do anything with it.
					SymLoadModule(hProcess,
						NULL, ((cch) ? szFile : NULL),
	#ifdef _WIN64
						NULL, (DWORD_PTR) memoryBasicInfo.AllocationBase, 0);
	#else
						NULL, (DWORD)(DWORD_PTR)memoryBasicInfo.AllocationBase, 0);
	#endif
					return (DWORD_PTR) memoryBasicInfo.AllocationBase;
				}
			}
	
			return 0;
		}
	
		static BOOL ResolveSymbol( HANDLE hProcess, UINT_PTR dwAddress,
			_ATL_SYMBOL_INFO &siSymbol)
		{
			BOOL fRetval = TRUE;
	
			siSymbol.dwAddress = dwAddress;
	
			CHAR szUndec[ATL_SYMBOL_NAME_LEN];
			CHAR szWithOffset[ATL_SYMBOL_NAME_LEN];
			LPSTR pszSymbol = NULL;
			IMAGEHLP_MODULE mi;
	
			memset(&siSymbol, 0, sizeof(_ATL_SYMBOL_INFO));
			mi.SizeOfStruct = sizeof(IMAGEHLP_MODULE);
	
	#ifdef _WIN64
			if (!SymGetModuleInfo(hProcess, dwAddress, &mi))
	#else
			if (!SymGetModuleInfo(hProcess, (UINT)dwAddress, &mi))
	#endif
			{
				::strncpy(siSymbol.szModule, "<no module>", _countof(siSymbol.szModule));
				siSymbol.szModule[_countof(siSymbol.szModule)-1] = '\0';
			}
			else
			{
				LPSTR pszModule = strchr(mi.ImageName, '\\');
				if (pszModule == NULL)
					pszModule = mi.ImageName;
				else
					pszModule++;
	
				::strncpy(siSymbol.szModule, pszModule, _countof(siSymbol.szModule));
				siSymbol.szModule[_countof(siSymbol.szModule)-1] = '\0';
			}
	
			__try
			{
				union 
				{
					CHAR rgchSymbol[sizeof(IMAGEHLP_SYMBOL) + ATL_SYMBOL_NAME_LEN];
					IMAGEHLP_SYMBOL  sym;
				} sym;
				memset(&sym.sym, 0x00, sizeof(sym.sym));
				sym.sym.SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
	#ifdef _WIN64
				sym.sym.Address = dwAddress;
	#else
				sym.sym.Address = (DWORD)dwAddress;
	#endif
				sym.sym.MaxNameLength = ATL_SYMBOL_NAME_LEN;
	
	#ifdef _WIN64
				if (SymGetSymFromAddr(hProcess, dwAddress, &(siSymbol.dwOffset), &sym.sym))
	#else
				if (SymGetSymFromAddr(hProcess, (DWORD)dwAddress, &(siSymbol.dwOffset), &sym.sym))
	#endif
				{
					pszSymbol = sym.sym.Name;
	
					if (UnDecorateSymbolName(sym.sym.Name, szUndec, sizeof(szUndec)/sizeof(szUndec[0]), 
						UNDNAME_NO_MS_KEYWORDS | UNDNAME_NO_ACCESS_SPECIFIERS))
					{
						pszSymbol = szUndec;
					}
					else if (SymUnDName(&sym.sym, szUndec, sizeof(szUndec)/sizeof(szUndec[0])))
					{
						pszSymbol = szUndec;
					}
					if (siSymbol.dwOffset != 0)
					{
						_snprintf(szWithOffset, ATL_SYMBOL_NAME_LEN, "%s + %d bytes", pszSymbol, siSymbol.dwOffset);
	
						// ensure null-terminated
						szWithOffset[ATL_SYMBOL_NAME_LEN-1] = '\0';
	
						pszSymbol = szWithOffset;
					}
				}
				else
					pszSymbol = "<no symbol>";
			}
			__except (EXCEPTION_ACCESS_VIOLATION == GetExceptionCode())
			{
				pszSymbol = "<EX: no symbol>";
				siSymbol.dwOffset = dwAddress - mi.BaseOfImage;
			}
	
			strncpy(siSymbol.szSymbol, pszSymbol, _countof(siSymbol.szSymbol));
			siSymbol.szSymbol[_countof(siSymbol.szSymbol)-1] = '\0';
			return fRetval;
		}
	};
	
	#ifndef _ATL_MAX_STACK_FRAMES
	#define _ATL_MAX_STACK_FRAMES 256
	#endif
	
	class _AtlThreadContextInfo
	{
	public:
		HANDLE             hThread; //Thread to get context for
		CONTEXT            context; //Where to put context
		IStackDumpHandler* pHandler;
		_AtlThreadContextInfo(IStackDumpHandler* p) throw()
		{
			hThread = NULL;
			memset(&context, 0, sizeof(context));
			context.ContextFlags = CONTEXT_FULL;
			pHandler = p;
		}
		~_AtlThreadContextInfo() throw()
		{
			if (hThread != NULL)
				CloseHandle(hThread);
		}
	#pragma warning(push)
	#pragma warning(disable:4793)
		HRESULT DumpStack() throw()
		{
			//We can't just call GetThreadContext for the current thread.
			//The docs state this is incorrect and we see problems on x64/Itanium
	
			//We need to spin up another thread which suspends this thread and gets
			//this thread's context, then unsuspends this thread and signals it.
	
			HANDLE hThreadPseudo = ::GetCurrentThread();
			HANDLE hProcess = ::GetCurrentProcess();
			if (DuplicateHandle(hProcess, hThreadPseudo, hProcess, &hThread, 0, FALSE, DUPLICATE_SAME_ACCESS) == 0)
				return HRESULT_FROM_WIN32(GetLastError());

#ifndef _DBG_UTIL_THREAD_DUMP
			DoDumpStack();
			return S_OK;
#endif // _DBG_UTIL_THREAD_DUMP
	
			DWORD dwID;
			HANDLE hWorkerThread = CreateThread(NULL, 0, ContextThreadProc, this, 0, &dwID);
			if (hWorkerThread == NULL)
				return HRESULT_FROM_WIN32(GetLastError());
			//Wait for thread to terminate which will indicate it is done.
			//On CE we can't suspend this thread while in the kernel, so loop
			//rather than just waiting.
			while (WaitForSingleObject(hWorkerThread, 0) != WAIT_OBJECT_0)
			{
			}
			CloseHandle(hWorkerThread);
			return S_OK;
		}
		static DWORD WINAPI ContextThreadProc( LPVOID pv) throw()
		{
			_AtlThreadContextInfo* pThis = 
				reinterpret_cast< _AtlThreadContextInfo* >(pv); 
			return pThis->DoDumpStack();
		}
		DWORD DoDumpStack() throw()
		{
#ifdef _DBG_UTIL_THREAD_DUMP
			SuspendThread(hThread);
#endif // _DBG_UTIL_THREAD_DUMP
			GetThreadContext(hThread, &context);
	
			pHandler->OnBegin();
	
			CAtlArray<void *> adwAddress;
			HANDLE hProcess = ::GetCurrentProcess();
			if (SymInitialize(hProcess, NULL, TRUE))
			{
				// force undecorated names to get params
				DWORD dw = SymGetOptions();
				dw &= ~SYMOPT_UNDNAME;
				SymSetOptions(dw);
	
				STACKFRAME stackFrame;
				memset(&stackFrame, 0, sizeof(stackFrame));
				stackFrame.AddrPC.Mode = AddrModeFlat;
				stackFrame.AddrFrame.Mode = AddrModeFlat;
				stackFrame.AddrStack.Mode = AddrModeFlat;
				stackFrame.AddrReturn.Mode = AddrModeFlat;
				stackFrame.AddrBStore.Mode = AddrModeFlat;
	
				DWORD dwMachType;
	
	#if defined(_M_IX86)
				dwMachType                   = IMAGE_FILE_MACHINE_I386;
	
				// program counter, stack pointer, and frame pointer
				stackFrame.AddrPC.Offset     = context.Eip;
				stackFrame.AddrStack.Offset  = context.Esp;
				stackFrame.AddrFrame.Offset  = context.Ebp;
	#elif defined(_M_AMD64)
				// only program counter
				dwMachType                   = IMAGE_FILE_MACHINE_AMD64;
				stackFrame.AddrPC.Offset     = context.Rip;
	#elif defined(_M_MRX000)
				// only program counter
				dwMachType                   = IMAGE_FILE_MACHINE_R4000;
				stackFrame.AddrPC.Offset     = context.Fir;
	#elif defined(_M_ALPHA)
				// only program counter
				dwMachType                   = IMAGE_FILE_MACHINE_ALPHA;
				stackFrame.AddrPC.Offset     = (unsigned long) context.Fir;
	#elif defined(_M_PPC)
				// only program counter
				dwMachType                   = IMAGE_FILE_MACHINE_POWERPC;
				stackFrame.AddrPC.Offset     = context.Iar;
	#elif defined(_M_IA64)
				// only program counter
				dwMachType                   = IMAGE_FILE_MACHINE_IA64;
				stackFrame.AddrPC.Offset     = context.StIIP;
	#elif defined(_M_ALPHA64)
				// only program counter
				dwMachType                   = IMAGE_FILE_MACHINE_ALPHA64;
				stackFrame.AddrPC.Offset     = context.Fir;
	#else
	#error("Unknown Target Machine");
	#endif
	
				adwAddress.SetCount(0, 16);
	
				int nFrame;
				for (nFrame = 0; nFrame < _ATL_MAX_STACK_FRAMES; nFrame++)
				{
					if (!StackWalk(dwMachType, hProcess, hThread,
						&stackFrame, &context, NULL,
						CStackDumper::FunctionTableAccess, CStackDumper::GetModuleBase, NULL))
					{
						break;
					}
					if (stackFrame.AddrPC.Offset != 0)
						adwAddress.Add((void*)(DWORD_PTR)stackFrame.AddrPC.Offset);
				}
			}
	
			// dump it out now
			INT_PTR nAddress;
			INT_PTR cAddresses = adwAddress.GetCount();
			for (nAddress = 0; nAddress < cAddresses; nAddress++)
			{
				CStackDumper::_ATL_SYMBOL_INFO info;
				UINT_PTR dwAddress = (UINT_PTR)adwAddress[nAddress];
	
				LPCSTR szModule = NULL;
				LPCSTR szSymbol = NULL;
	
				if (CStackDumper::ResolveSymbol(hProcess, dwAddress, info))
				{
					szModule = info.szModule;
					szSymbol = info.szSymbol;
				}
				pHandler->OnEntry((void *) dwAddress, szModule, szSymbol);
			}
			pHandler->OnEnd();
#ifdef _DBG_UTIL_THREAD_DUMP
			ResumeThread(hThread);
#endif // _DBG_UTIL_THREAD_DUMP
	
			return 0;
		} 
	};
	
	// Helper function to produce a stack dump
	ATL_NOINLINE inline void DumpStack(IStackDumpHandler *pHandler)
	{
		ATLASSERT(pHandler);
		_AtlThreadContextInfo info(pHandler);
		info.DumpStack();
	}
	#pragma warning(pop)

	class CStackDumpHandlerImpl : public IStackDumpHandler
	{
		CString m_sOut;

	public:
		void __stdcall OnBegin()
		{
			SendOut("=== begin DumpStack output ===\r\n");
		}
		void __stdcall OnEntry(void *pvAddress, LPCSTR szModule, LPCSTR szSymbol)
		{
			char sz[20];
			wsprintfA(sz, "%8.8X: ", pvAddress);
			SendOut(sz);

			if (szModule)
			{
				ATLASSERT(szSymbol);
				SendOut(szModule);
				SendOut("! ");
				SendOut(szSymbol);
			}
			else
				SendOut("symbol not found");
			SendOut("\r\n");
		}

		void __stdcall OnError(LPCSTR szError)
		{
			SendOut(szError);
		}
		void __stdcall OnEnd()
		{
			SendOut("=== end DumpStack() output ===\r\n");
		}
		virtual void SendOut(LPCSTR pszData) {
			m_sOut.Append(pszData);
		}
		const CString &operator() () const {
			return m_sOut;
		}
		CStackDumpHandlerImpl() {}
		~CStackDumpHandlerImpl() {}
	};

	CString DumpStack()
	{
		CStackDumpHandlerImpl sdhi;
		DumpStack(&sdhi);
		return sdhi();
	}
}

#endif // _DBG_UTIL_H_
