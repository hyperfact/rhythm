// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#ifndef max
#  define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#  define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
//////////////////////////////////////////////////////////////////////////
#include "../gtest/gtest.h"
#ifdef _DEBUG
#  pragma comment(lib, "../gtest/gtestd.lib")
#else // _DEBUG
#  pragma comment(lib, "../gtest/gtest.lib")
#endif // _DEBUG

// TODO: 在此处引用程序需要的其他头文件
