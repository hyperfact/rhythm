# Microsoft Developer Studio Project File - Name="hylib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=hylib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "hylib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hylib.mak" CFG="hylib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hylib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "hylib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "hylib - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I ".." /I "../include" /I "../loki/include/loki/noncc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\bin\hylib.lib"
# Begin Special Build Tool
WkspDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=cd $(WkspDir)	redist.bat	redist_src.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "hylib - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I ".." /I "../include" /I "../loki/include/loki/noncc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\bin\hylib_d.lib"
# Begin Special Build Tool
WkspDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=cd $(WkspDir)	redist_src.bat
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "hylib - Win32 Release"
# Name "hylib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\ImgFont.cpp
# End Source File
# Begin Source File

SOURCE=..\src\LayeredWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\src\LayeredWnd.cpp
# End Source File
# Begin Source File

SOURCE=..\src\log.cpp
# End Source File
# Begin Source File

SOURCE=..\src\logview.cpp
# End Source File
# Begin Source File

SOURCE=..\src\mt19937ar.cpp
# End Source File
# Begin Source File

SOURCE=..\src\ResourceMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\src\strings.cpp
# End Source File
# Begin Source File

SOURCE=..\src\tmpldlg.cpp
# End Source File
# Begin Source File

SOURCE=..\src\util.cpp
# End Source File
# Begin Source File

SOURCE=..\src\WndElem.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\_prolog.h
# End Source File
# Begin Source File

SOURCE=..\include\assoc_vector.h
# End Source File
# Begin Source File

SOURCE=..\include\buffer.h
# End Source File
# Begin Source File

SOURCE=..\include\compare.h
# End Source File
# Begin Source File

SOURCE=..\include\hylib_all.h
# End Source File
# Begin Source File

SOURCE=..\include\ImgFont.h
# End Source File
# Begin Source File

SOURCE=..\include\LayeredWindow.h
# End Source File
# Begin Source File

SOURCE=..\include\LayeredWnd.h
# End Source File
# Begin Source File

SOURCE=..\include\log.h
# End Source File
# Begin Source File

SOURCE=..\include\logview.h
# End Source File
# Begin Source File

SOURCE=..\include\macro.h
# End Source File
# Begin Source File

SOURCE=..\include\MemDC.h
# End Source File
# Begin Source File

SOURCE=..\include\random.h
# End Source File
# Begin Source File

SOURCE=..\include\ResourceMgr.h
# End Source File
# Begin Source File

SOURCE=..\include\singleton.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\include\stl_ext.h
# End Source File
# Begin Source File

SOURCE=..\include\strings.h
# End Source File
# Begin Source File

SOURCE=..\include\threads.h
# End Source File
# Begin Source File

SOURCE=..\include\tmpldlg.h
# End Source File
# Begin Source File

SOURCE=..\include\util.h
# End Source File
# Begin Source File

SOURCE=..\include\util.inl
# End Source File
# Begin Source File

SOURCE=..\include\util_tmpl.h
# End Source File
# Begin Source File

SOURCE=..\include\WndElem.h
# End Source File
# Begin Source File

SOURCE=..\include\WndElem.inl
# End Source File
# End Group
# Begin Group "tinyxml"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\tinyxml\tinystr.cpp
# End Source File
# Begin Source File

SOURCE=..\tinyxml\tinystr.h
# End Source File
# Begin Source File

SOURCE=..\tinyxml\tinyxml.cpp
# End Source File
# Begin Source File

SOURCE=..\tinyxml\tinyxml.h
# End Source File
# Begin Source File

SOURCE=..\tinyxml\tinyxmlerror.cpp
# End Source File
# Begin Source File

SOURCE=..\tinyxml\tinyxmlparser.cpp
# End Source File
# End Group
# Begin Group "loki"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\loki\include\noncc\loki\AbstractFactory.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\AssocVector.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\DataGenerators.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\EmptyType.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\Factory.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\Functor.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\HierarchyGenerators.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\LokiTypeInfo.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\MultiMethods.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\NullType.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\Singleton.cpp
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\Singleton.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\SmallObj.cpp
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\SmallObj.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\SmartPtr.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\static_check.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\Threads.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\Tuple.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\Typelist.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\TypeManip.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\TypeTraits.h
# End Source File
# Begin Source File

SOURCE=..\loki\include\noncc\loki\Visitor.h
# End Source File
# End Group
# Begin Group "detail"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\detail\_log_detail.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
