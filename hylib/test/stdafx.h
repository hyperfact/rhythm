// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
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

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
