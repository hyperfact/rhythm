#include "StdAfx.h"
#include "LayeredWindow.h"
#include "util_tmpl.h"
#include "compare.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0500) && (_MSC_VER<=1200)
	namespace {
		HMODULE	_hUser32_Dll = NULL;

		#if defined(_WINGDI_) && !defined(NOGDI)
			typedef BOOL (WINAPI *_pfnUpdateLayeredWindow_t)(HWND,HDC,POINT*,
				SIZE*,HDC,POINT*,COLORREF,BLENDFUNCTION*,DWORD);
			_pfnUpdateLayeredWindow_t _pfnUpdateLayeredWindow = NULL;
		#endif // defined(_WINGDI_) && !defined(NOGDI)

		#if (_WIN32_WINNT >= 0x0501)
			typedef BOOL (WINAPI *_pfnGetLayeredWindowAttributes_t)(HWND,
				COLORREF*,BYTE*,DWORD*);
			_pfnGetLayeredWindowAttributes_t _pfnGetLayeredWindowAttributes = NULL;
		#endif // (_WIN32_WINNT >= 0x0501)

		typedef BOOL (WINAPI *_pfnSetLayeredWindowAttributes_t)(HWND,COLORREF,BYTE,DWORD);
		_pfnSetLayeredWindowAttributes_t _pfnSetLayeredWindowAttributes = NULL;

		// ²ã´Î´°¿ÚÊØ»¤
		struct _LayeredWndGuard : _HYLIB noncopyable
		{
			static void Guard() { static const _LayeredWndGuard _layeredWndGuard; }
		private:
			_LayeredWndGuard()
			{
				ASSERT(NULL == _hUser32_Dll);

				_hUser32_Dll = ::LoadLibrary(_T("User32.dll"));
				ASSERT(_hUser32_Dll);
				
				#if defined(_WINGDI_) && !defined(NOGDI)
					_pfnUpdateLayeredWindow = (_pfnUpdateLayeredWindow_t)::GetProcAddress(
						_hUser32_Dll, _T("UpdateLayeredWindow"));
					ASSERT(_pfnUpdateLayeredWindow);
				#endif // defined(_WINGDI_) && !defined(NOGDI)
				
				#if (_WIN32_WINNT >= 0x0501)
					_pfnGetLayeredWindowAttributes = (_pfnGetLayeredWindowAttributes_t)::GetProcAddress(
						_hUser32_Dll, _T("GetLayeredWindowAttributes"));
					ASSERT(_pfnGetLayeredWindowAttributes);
				#endif // (_WIN32_WINNT >= 0x0501)
				
				_pfnSetLayeredWindowAttributes = (_pfnSetLayeredWindowAttributes_t)::GetProcAddress(
					_hUser32_Dll, _T("SetLayeredWindowAttributes"));
				ASSERT(_pfnUpdateLayeredWindow);
			}
		public:
			~_LayeredWndGuard()
			{
				if(_hUser32_Dll) {
					::FreeLibrary(_hUser32_Dll);
					_hUser32_Dll = NULL;
				}
				
				#if defined(_WINGDI_) && !defined(NOGDI)
					_pfnUpdateLayeredWindow = NULL;
				#endif // defined(_WINGDI_) && !defined(NOGDI)
				
				#if (_WIN32_WINNT >= 0x0501)
					_pfnGetLayeredWindowAttributes = NULL;
				#endif // (_WIN32_WINNT >= 0x0501)
				
				_pfnSetLayeredWindowAttributes = NULL;
			}
		};
	}; // end anonymous namespace
	#define InitLayeredWndEnv() _LayeredWndGuard::Guard()

#	if defined(_WINGDI_) && !defined(NOGDI)
	BOOL WINAPI UpdateLayeredWindow(
		/*[in]*/ HWND	hwnd,
		/*[in]*/ HDC	hdcDst,
		/*[in]*/ POINT*	pptDst,
		/*[in]*/ SIZE*	psize,
		/*[in]*/ HDC	hdcSrc,
		/*[in]*/ POINT* pptSrc,
		/*[in]*/ COLORREF crKey,
		/*[in]*/ BLENDFUNCTION* pblend,
		/*[in]*/ DWORD	dwFlags)
	{
		InitLayeredWndEnv();
		ASSERT(_pfnUpdateLayeredWindow);
		ASSERT(::IsWindow(hwnd));
		ASSERT(BT_IS_CLEAR(::GetWindowLong(hwnd, GWL_STYLE), WS_CHILD));
		ASSERT(BT_IS_SET(::GetWindowLong(hwnd, GWL_EXSTYLE), WS_EX_LAYERED));
		if (_pfnUpdateLayeredWindow) {
			return (*_pfnUpdateLayeredWindow)(
				hwnd, hdcDst, pptDst, psize, hdcSrc, pptSrc, crKey, pblend, dwFlags);
		}
		return FALSE;
	}
#	endif // defined(_WINGDI_) && !defined(NOGDI)

#	if (_WIN32_WINNT >= 0x0501)
	BOOL WINAPI GetLayeredWindowAttributes(
		/*[in]*/ HWND		hwnd,
		/*[out]*/ COLORREF*	pcrKey,
		/*[out]*/ BYTE*		pbAlpha,
		/*[out]*/ DWORD*	pdwFlags)
	{
		InitLayeredWndEnv();
		ASSERT(_pfnGetLayeredWindowAttributes);
		ASSERT(::IsWindow(hwnd));
		ASSERT(BT_IS_CLEAR(::GetWindowLong(hwnd, GWL_STYLE), WS_CHILD));
		ASSERT(BT_IS_SET(::GetWindowLong(hwnd, GWL_EXSTYLE), WS_EX_LAYERED));
		if (_pfnGetLayeredWindowAttributes) {
			return (*_pfnGetLayeredWindowAttributes)(hwnd, pcrKey, pbAlpha, pdwFlags);
		}
		return FALSE;
	}
#	endif /* _WIN32_WINNT >= 0x0501 */

	BOOL WINAPI SetLayeredWindowAttributes(
		/*[in]*/ HWND hwnd,
		/*[in]*/ COLORREF crKey,
		/*[in]*/ BYTE bAlpha,
		/*[in]*/ DWORD dwFlags)
	{
		InitLayeredWndEnv();
		ASSERT(::IsWindow(hwnd));
		ASSERT(_pfnSetLayeredWindowAttributes);
		ASSERT(BT_IS_CLEAR(::GetWindowLong(hwnd, GWL_STYLE), WS_CHILD));
		ASSERT(BT_IS_SET(::GetWindowLong(hwnd, GWL_EXSTYLE), WS_EX_LAYERED));
		if (_pfnSetLayeredWindowAttributes) {
			return (*_pfnSetLayeredWindowAttributes)(hwnd, crKey, bAlpha, dwFlags);
		}
		return FALSE;
	}

#endif /* defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0500) && (_MSC_VER<=1200) */
