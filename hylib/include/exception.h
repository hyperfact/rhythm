/* 
*	Copyright (c) 2002-2003, author: canco, email: canco_99@sina.com.  

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

#ifndef __EXCEPTION_H__ 
#define __EXCEPTION_H__ 
#pragma warning( disable: 4786 ) 

//#include <eh.h> 
#include <string> 
#include <vector> 
#include <new.h> 

#include <windows.h> 
#include <cfloat> 

using namespace std; 

#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h> 

#include <crtdbg.h> 

#ifndef DEBUG_NEW
#define DEBUG_NEW new( _NORMAL_BLOCK, THIS_FILE, __LINE__ ) 
#endif // DEBUG_NEW


//#include <stdio.h> 
#include <stdlib.h> 
// imagehlp.h must be compiled with packing to eight-byte-boundaries, 
// but does nothing to enforce that. I am grateful to Jeff Shanholtz 
// <JShanholtz@premia.com> for finding this problem. 
#pragma pack( push, before_imagehlp, 8 ) 
#include <imagehlp.h> 
#pragma pack( pop, before_imagehlp ) 



//	要保证结构化异常在发行版本生效，必须用/EHa编译选项，而不能用默认的。 

//	标准C++包括以下异常： 
/* 
name				throw by					header 
bad_alloc			new							<new> 
bad_cast			dynamic_cast				<typeinfo> 
bad_typeid			typeid						<typeinfo> 
bad_exception		exception specification		<exception> 
out_of_range		at(),						<stdexcept> 
invalid_argument								<stdexcept> 
overflow_error									<stdexcept> 
ios_base::failure								<ios> 
length_error									<stdexcept> 
domain_error									<stdexcept> 
underflow_error									<stdexcept> 


*/ 

namespace dbg_util {

class CWinUtil { 
private: 
	CWinUtil(); 
public: 
	//	启动检查内存泄露的功能。 
	static void vCheckMemoryLeak();

	//	把WINDOWS中的结构化异常转化为C++异常。 
	static void vMapSEHtoCE();

	//	使new函数失败时抛出异常。 
	static void vSetThrowNewException();

	//	让浮点数错误抛出异常 
	static void vSetThrowFloatException();

	static void vSetUnExpectedExceptionFilter();

	static bool vInitEnviroment();

	static void vUnInitEnviroment();

private:
	static int NewHandler( size_t size );
	static void _cdecl TranslateSEHtoCE( UINT code, PEXCEPTION_POINTERS pep );
	static long _stdcall UnExpectedExceptionFilter( LPEXCEPTION_POINTERS pe );
}; 

class CSEHException : public exception { 

public: 
	CSEHException( UINT code, PEXCEPTION_POINTERS pep ); 

	operator unsigned int() const { return m_exceptionCode; } 

	__exString what() const { return m_strMsg.c_str();	}

	PEXCEPTION_POINTERS GetExceptionInfo() const { return m_pExceptionInfo; }

protected: 
	UINT m_exceptionCode; 

	PEXCEPTION_POINTERS m_pExceptionInfo;

	EXCEPTION_RECORD m_exceptionRecord; 

	CONTEXT m_context; 

	string m_strMsg; 


}; 

//	可恢复的结构化异常。 
class CRecoverableSEHException : public CSEHException { 
public: 
	CRecoverableSEHException( UINT code, PEXCEPTION_POINTERS pep ) : 
	  CSEHException( code, pep ){} 
}; 

//	不可恢复的结构化异常。 
class CUnRecoverableSEHException : public CSEHException { 
public: 
	CUnRecoverableSEHException( UINT code, PEXCEPTION_POINTERS pep ):
	  CSEHException( code, pep ){} 

}; 

class CUserException : public domain_error { 
public: 
	CUserException( const string& str ) ; 

}; 

//	跟SOCKET有关的异常。 
class CSocketException : public CUserException { 
public: 
	CSocketException( const string& str ) : CUserException( str ) {} 

}; 
//	跟配置文件有关的异常。 
class CConfigException : public CUserException { 
public: 
	CConfigException( const string& str ) : CUserException( str.c_str() ) {} 

}; 
//	跟数据库操作有关的异常。 
class CDatabaseException : public CUserException { 
public: 
	CDatabaseException( const string& str ) : CUserException( str.c_str() ) {} 

}; 

//	跟操作系统API返回错误有关的异常。 
class COSApiException : public CUserException { 
public: 
	COSApiException( const string& str ) : CUserException( str.c_str() ) {} 

}; 

void PrintStack();

CString WriteMiniDump(PEXCEPTION_POINTERS pException);

} // namespace dbg_util

#endif

