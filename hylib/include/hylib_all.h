/********************************************************************
	created:	2011/09/08
	created:	8:9:2011   9:28
	filename: 	hylib_all.h
	file base:	hylib_all
	file ext:	h
	author:		huaiyu
	
	purpose:	Include all headers in the lib.
*********************************************************************/

#ifndef _HYLIB_ALL_H_
#define _HYLIB_ALL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
////////////////////////////////////////////////////////////////////////////////
// Includes:
// includes go here
//******************************************************************************
// Library "tinyxml" for XML:
#ifndef TIXML_USE_STL
#	define TIXML_USE_STL
#endif // TIXML_USE_STL
#include "tinyxml/tinyxml.h"

//******************************************************************************
// Library "loki":
#include "loki/static_check.h"
#include "loki/NullType.h"
#include "loki/EmptyType.h"
#include "loki/LokiTypeInfo.h"
#include "loki/SmallObj.h"
#include "loki/AssocVector.h"
#include "loki/Typelist.h"
#include "loki/TypeManip.h"
#include "loki/TypeTraits.h"
#include "loki/Tuple.h"
#include "loki/DataGenerators.h"
#include "loki/HierarchyGenerators.h"
#include "loki/Functor.h"
#include "loki/Factory.h"
#include "loki/AbstractFactory.h"
#include "loki/MultiMethods.h"
#include "loki/Singleton.h"
#include "loki/SmartPtr.h"
#include "loki/Threads.h"
#include "loki/Visitor.h"

//******************************************************************************
// Library "hylib":
#include "_prolog.h"
#include "macro.h"
#include "compare.h"
//#include "log.h"
#include "util.h"
#include "stl_ext.h"
#include "util_tmpl.h"
//#include "assoc_vector.h"
#include "strings.h"
#include "buffer.h"
#include "ImgFont.h"
#include "LayeredWindow.h"
#include "LayeredWnd.h"
#include "MemDC.h"
#include "ResourceMgr.h"
#include "WndElem.h"
#include "random.h"
#include "singleton.h"
#include "tmpldlg.h"
#include "logview.h"
#include "threads.h"

//******************************************************************************
#ifdef _MSC_VER
#	ifdef _DEBUG
#		if defined(_HYLIB_DLL) && !defined(_HYLIB_DECL_DLL)
#			pragma comment(lib, "hydll_d.lib")
#		else // defined(_HYLIB_DLL) && !defined(_HYLIB_DECL_DLL)
#			pragma comment(lib, "hylib_d.lib")
#		endif // _HYLIB_DLL
#	else // _DEBUG
#		if defined(_HYLIB_DLL) && !defined(_HYLIB_DECL_DLL)
#			pragma comment(lib, "hydll.lib")
#		else // defined(_HYLIB_DLL) && !defined(_HYLIB_DECL_DLL)
#			pragma comment(lib, "hylib.lib")
#		endif // _HYLIB_DLL		
#	endif // _DEBUG
#endif // _MSVC_VER

////////////////////////////////////////////////////////////////////////////////

#endif // _HYLIB_ALL_H_
