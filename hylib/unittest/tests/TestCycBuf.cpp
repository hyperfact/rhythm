// TestCycBuf.cpp: implementation of the CTestCycBuf class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestCycBuf.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma function(memcmp)

////////////////////////////////////////////////////////////////////////////////
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(CTestCycBuf, TEST_SUITE_NAME);

CTestCycBuf::byte_t CTestCycBuf::DATA[256];
namespace {
struct _Static_Init {
	_Static_Init() {
		for (int i=0; i<256; ++i) {
			CTestCycBuf::DATA[i] = i;
		}
	}
} _static_init;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTestCycBuf::CTestCycBuf()
{

}

CTestCycBuf::~CTestCycBuf()
{

}

void CTestCycBuf::setUp()
{
}

void CTestCycBuf::tearDown()
{
}

void CTestCycBuf::testSize()
{
	const size_t BUF_LEN = 63;
	CCycBufTestWrapper buf(BUF_LEN);
//------------------------------------------------------------------------------
	CPPUNIT_ASSERT_EQUAL(0, (int)buf.size());
	CPPUNIT_ASSERT(buf.empty());
	CPPUNIT_ASSERT_EQUAL(BUF_LEN, buf.capacity());
	CPPUNIT_ASSERT_EQUAL(BUF_LEN, buf.avail());

	buf.PBeg() = buf.Data()+8;
	buf.PEnd() = buf.Data()+8;
	CPPUNIT_ASSERT_EQUAL(0, (int)buf.size());
	CPPUNIT_ASSERT(buf.empty());
	CPPUNIT_ASSERT_EQUAL(BUF_LEN, buf.capacity());
	CPPUNIT_ASSERT_EQUAL(BUF_LEN, buf.avail());

	buf.PBeg() = buf.Data()+8;
	buf.PEnd() = buf.Data()+16;
	CPPUNIT_ASSERT(!buf.empty());
	CPPUNIT_ASSERT_EQUAL(8, (int)buf.size());
	CPPUNIT_ASSERT_EQUAL(BUF_LEN-8, buf.avail());

	buf.PBeg() = buf.Data()+17;
	buf.PEnd() = buf.Data()+8;
	CPPUNIT_ASSERT(!buf.empty());
	CPPUNIT_ASSERT_EQUAL(BUF_LEN-8, buf.size());
	CPPUNIT_ASSERT_EQUAL(8, (int)buf.avail());

	buf.PBeg() = buf.Data()+8;
	buf.PEnd() = buf.Data()+7;
	CPPUNIT_ASSERT(!buf.empty());
	CPPUNIT_ASSERT_EQUAL(BUF_LEN, buf.size());
	CPPUNIT_ASSERT_EQUAL(0, (int)buf.avail());
}

void CTestCycBuf::testReserve()
{
	{
		CCycBufTestWrapper buf(1);
		buf.reserve(16);
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.size());
		CPPUNIT_ASSERT(buf.empty());
		CPPUNIT_ASSERT_EQUAL(19, (int)buf.capacity());
		CPPUNIT_ASSERT_EQUAL(19, (int)buf.avail());
	}

	{
		CCycBufTestWrapper buf(1);
		buf.reserve(16);
		buf.PEnd() = buf.PBeg()+8;
		memcpy(buf.PBeg(), DATA, 8);
		buf.reserve(32);
		CPPUNIT_ASSERT_EQUAL(0, memcmp(buf.PBeg(), DATA, 8));
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(35, (int)buf.capacity());
		CPPUNIT_ASSERT_EQUAL(27, (int)buf.avail());
	}

	{
		CCycBufTestWrapper buf(1);
		buf.reserve(16);
		buf.PBeg() = buf.Data()+4;
		buf.PEnd() = buf.PBeg()+8;
		memcpy(buf.PBeg(), DATA, 8);
		buf.reserve(32);
		CPPUNIT_ASSERT_EQUAL(0, memcmp(buf.PBeg(), DATA, 8));
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(35, (int)buf.capacity());
		CPPUNIT_ASSERT_EQUAL(27, (int)buf.avail());
	}

	{
		CCycBufTestWrapper buf(1);
		buf.reserve(16);
		buf.PBeg() = buf.Last()-6;
		memcpy(buf.PBeg(), DATA, 6);
		buf.PEnd() = buf.Data()+4;
		memcpy(buf.Data(), DATA+6, 4);
		buf.reserve(32);
		CPPUNIT_ASSERT_EQUAL(0, memcmp(buf.PBeg(), DATA, 10));
		CPPUNIT_ASSERT_EQUAL(10, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(35, (int)buf.capacity());
		CPPUNIT_ASSERT_EQUAL(25, (int)buf.avail());
	}

	{
		CCycBufTestWrapper buf(1);
		buf.reserve(16);
		buf.PBeg() = buf.Last()-7;
		memcpy(buf.PBeg(), DATA, 7);
		buf.PEnd() = buf.Data()+9;
		memcpy(buf.Data(), DATA+7, 9);
		buf.reserve(32);
		CPPUNIT_ASSERT_EQUAL(0, memcmp(buf.PBeg(), DATA, 16));
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(35, (int)buf.capacity());
		CPPUNIT_ASSERT_EQUAL(19, (int)buf.avail());
	}
}

void CTestCycBuf::testPeek()
{
	byte_t arData[256] = {0};
	
	{
		CCycBufTestWrapper buf(16);
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.peek(arData, 16));
	}

	{
		CCycBufTestWrapper buf(16);
		buf.PBeg() = buf.Data();
		buf.PEnd() = buf.PBeg()+8;
		memcpy(buf.PBeg(), DATA, 8);
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.peek(arData, 16));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 8));
	}

	{
		CCycBufTestWrapper buf(16);
		buf.PBeg() = buf.Last()-6;
		memcpy(buf.PBeg(), DATA, 6);
		buf.PEnd() = buf.Data()+4;
		memcpy(buf.Data(), DATA+6, 4);
		CPPUNIT_ASSERT_EQUAL(10, (int)buf.peek(arData, 16));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 10));
	}

	{
		CCycBufTestWrapper buf(19);
		buf.PBeg() = buf.Last()-6;
		memcpy(buf.PBeg(), DATA, 6);
		buf.PEnd() = buf.Data()+13;
		memcpy(buf.Data(), DATA+6, 13);
		CPPUNIT_ASSERT_EQUAL(19, (int)buf.peek(arData, 20));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 19));
	}
}

void CTestCycBuf::testRead()
{
	byte_t arData[256] = {0};

	{
		CCycBufTestWrapper buf(16);
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.read(arData, 16));
	}

	{
		CCycBufTestWrapper buf(16);
		buf.PBeg() = buf.Data();
		buf.PEnd() = buf.PBeg()+8;
		memcpy(buf.PBeg(), DATA, 8);
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.read(arData, 16));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 8));
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.read(arData, 16));
	}

	{
		CCycBufTestWrapper buf(16);
		buf.PBeg() = buf.Data();
		buf.PEnd() = buf.PBeg()+8;
		memcpy(buf.PBeg(), DATA, 8);
		CPPUNIT_ASSERT_EQUAL(5, (int)buf.read(arData, 5));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 5));
		CPPUNIT_ASSERT_EQUAL(3, (int)buf.read(arData, 10));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA+5, 3));
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.read(arData, 16));
	}
	
	{
		CCycBufTestWrapper buf(16);
		buf.PBeg() = buf.Last()-6;
		memcpy(buf.PBeg(), DATA, 6);
		buf.PEnd() = buf.Data()+4;
		memcpy(buf.Data(), DATA+6, 4);
		CPPUNIT_ASSERT_EQUAL(10, (int)buf.read(arData, 16));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 10));
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.read(arData, 16));
	}

	{
		CCycBufTestWrapper buf(16);
		buf.PBeg() = buf.Last()-6;
		memcpy(buf.PBeg(), DATA, 6);
		buf.PEnd() = buf.Data()+4;
		memcpy(buf.Data(), DATA+6, 4);
		CPPUNIT_ASSERT_EQUAL(4, (int)buf.read(arData, 4));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 4));
		CPPUNIT_ASSERT_EQUAL(6, (int)buf.read(arData, 16));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA+4, 6));
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.read(arData, 16));
	}

	{
		CCycBufTestWrapper buf(16);
		buf.PBeg() = buf.Last()-6;
		memcpy(buf.PBeg(), DATA, 6);
		buf.PEnd() = buf.Data()+4;
		memcpy(buf.Data(), DATA+6, 4);
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.read(arData, 8));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 8));
		CPPUNIT_ASSERT_EQUAL(2, (int)buf.read(arData, 16));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA+8, 2));
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.read(arData, 16));
	}
	
	{
		CCycBufTestWrapper buf(19);
		buf.PBeg() = buf.Last()-6;
		memcpy(buf.PBeg(), DATA, 6);
		buf.PEnd() = buf.Data()+13;
		memcpy(buf.Data(), DATA+6, 13);
		CPPUNIT_ASSERT_EQUAL(19, (int)buf.read(arData, 20));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 19));
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.read(arData, 16));
	}

	{
		CCycBufTestWrapper buf(19);
		buf.PBeg() = buf.Last()-6;
		memcpy(buf.PBeg(), DATA, 6);
		buf.PEnd() = buf.Data()+13;
		memcpy(buf.Data(), DATA+6, 13);
		CPPUNIT_ASSERT_EQUAL(4, (int)buf.read(arData, 4));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 4));
		CPPUNIT_ASSERT_EQUAL(15, (int)buf.read(arData, 20));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA+4, 15));
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.read(arData, 16));
	}

	{
		CCycBufTestWrapper buf(19);
		buf.PBeg() = buf.Last()-6;
		memcpy(buf.PBeg(), DATA, 6);
		buf.PEnd() = buf.Data()+13;
		memcpy(buf.Data(), DATA+6, 13);
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.read(arData, 8));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 8));
		CPPUNIT_ASSERT_EQUAL(11, (int)buf.read(arData, 20));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA+8, 11));
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.read(arData, 16));
	}
}

void CTestCycBuf::testGrow()
{
	CCycBufTestWrapper buf(3);
	CPPUNIT_ASSERT_EQUAL(3, (int)buf.capacity());
	buf.Grow(2);
	CPPUNIT_ASSERT_EQUAL(7, (int)buf.capacity());
	buf.Grow(2);
	CPPUNIT_ASSERT_EQUAL(11, (int)buf.capacity());
	buf.Grow(6);
	CPPUNIT_ASSERT_EQUAL(19, (int)buf.capacity());
	buf.Grow(2);
	CPPUNIT_ASSERT_EQUAL(31, (int)buf.capacity());
	buf.Grow(32);
	CPPUNIT_ASSERT_EQUAL(63, (int)buf.capacity());
	buf.Grow(65);
	CPPUNIT_ASSERT_EQUAL(131, (int)buf.capacity());
}

void CTestCycBuf::testWrite()
{
	byte_t arData[256] = {0};

	{
		CCycBufTestWrapper buf(4);
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.write(DATA, 0));
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.write(DATA, 16));
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(0, memcmp(buf.PBeg(), DATA, 16));
	}

	{
		CCycBufTestWrapper buf(15);
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.write(DATA, 16));
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(0, memcmp(buf.PBeg(), DATA, 16));
	}

	{
		CCycBufTestWrapper buf(16);
		buf.PBeg() = buf.Data()+4;
		buf.PEnd() = buf.PBeg();
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.write(DATA, 8));
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(0, memcmp(buf.PBeg(), DATA, 8));
	}

	{
		CCycBufTestWrapper buf(15);
		buf.PBeg() = buf.Data()+4;
		buf.PEnd() = buf.PBeg();
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.write(DATA, 16));
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(0, memcmp(buf.PBeg(), DATA, 16));
	}

	{
		CCycBufTestWrapper buf(16);
		buf.PBeg() = buf.Data()+4;
		buf.PEnd() = buf.PBeg();
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.write(DATA, 8));
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.write(DATA+8, 16));
		CPPUNIT_ASSERT_EQUAL(24, (int)buf.size());

		CPPUNIT_ASSERT_EQUAL(24, (int)buf.peek(arData, 256));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 24));
	}

	{
		CCycBufTestWrapper buf(16);
		buf.PBeg() = buf.Data()+12;
		buf.PEnd() = buf.PBeg();
		CPPUNIT_ASSERT_EQUAL(10, (int)buf.write(DATA, 10));
		CPPUNIT_ASSERT_EQUAL(10, (int)buf.size());
		
		CPPUNIT_ASSERT_EQUAL(10, (int)buf.peek(arData, 256));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 10));
	}

	{
		CCycBufTestWrapper buf(16);
		buf.PBeg() = buf.Data()+12;
		buf.PEnd() = buf.PBeg();
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.write(DATA, 8));
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(6, (int)buf.write(DATA+8, 6));
		CPPUNIT_ASSERT_EQUAL(14, (int)buf.size());
		
		CPPUNIT_ASSERT_EQUAL(14, (int)buf.peek(arData, 256));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 14));
	}
	
	{
		CCycBufTestWrapper buf(16);
		buf.PBeg() = buf.Last()-6;
		buf.PEnd() = buf.PBeg();
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.write(arData, 8));
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.write(DATA+8, 16));
		CPPUNIT_ASSERT_EQUAL(24, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(24, (int)buf.peek(arData, 256));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 24));
	}

	{
		CCycBufTestWrapper buf(15);
		buf.PBeg() = buf.Last()-6;
		buf.PEnd() = buf.PBeg();
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.write(arData, 8));
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(7, (int)buf.write(DATA+8, 7));
		CPPUNIT_ASSERT_EQUAL(15, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(5, (int)buf.write(DATA+15, 5));
		CPPUNIT_ASSERT_EQUAL(20, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(20, (int)buf.peek(arData, 256));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 20));
	}
}

void CTestCycBuf::testRepos()
{
	char arData[256] = {0};

// 	if (_pbeg <= _pend) {
// 		::memmove(_data, _pbeg, size());
	{
		CCycBufTestWrapper buf(15);
		
		buf.PBeg() = buf.Data()+4;
		buf.PEnd() = buf.PBeg();
		buf.write(DATA, 8);
		buf.Repos();
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.peek(arData, 256));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 8));
	}
// 	} else {
// 		if (part1 <= unuse) {
// 			// case 1: part1+part2
// 			// 222---11
// 			::memmove(_data+part1, _data, part2);			// --222-11
// 			::memmove(_data, _pbeg, part1);					// 11222---
	{
		CCycBufTestWrapper buf(15);
		// 5-8-3
		buf.PBeg() = buf.Data()+13;
		buf.PEnd() = buf.PBeg();
		buf.write(DATA, 8);
		buf.Repos();
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.peek(arData, 256));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 8));
	}
// 		} else if (part2 <= unuse) {
			// p2 < u < p1
// 			// case 2: part1+part2
// 			// 22---111
// 			::memmove(_data+part2, _pbeg, part1);			// 22111---
// 			::memmove(_data+part2+part1, _data, part2);		// --11122-
// 			//::memmove(_data, _data+part2, part1+part2);	// 11122---
	{
		CCycBufTestWrapper buf(15);
		// 3-5-8
		buf.PBeg() = buf.Data()+8;
		buf.PEnd() = buf.PBeg();
		buf.write(DATA, 11);
		buf.Repos();
		CPPUNIT_ASSERT_EQUAL(11, (int)buf.peek(arData, 256));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 11));
	}
// 		} else if (part1 <= (unuse*2) && part2<=part1) {
// 			// u < p2 < p1 < 2*u
// 			// case 3: part1+part2 + part1+par2
// 			// part2+unuse+part1-unuse == part1+part2
// 			// 2222222---11111
// 			::memmove(_data+unuse, _data, part2);			// ---222222211111
// 			::memmove(_data, _last()-unuse, unuse);			// 111222222211---
// 			::memmove(_pbeg+unuse, _pbeg, part1-unuse);		// 1112222222---11
// 			_pend = _pbeg;
// 			_pbeg += unuse;									// 2222222222---11
// 			assert((size_t)(_last()-_pbeg) <= unuse);
// 			_repos();										// (jmp to case 1)
// 			return;
	{
		CCycBufTestWrapper buf(15);
		// 5-4-7
		buf.PBeg() = buf.Data()+9;
		buf.PEnd() = buf.PBeg();
		buf.write(DATA, 12);
		buf.Repos();
		CPPUNIT_ASSERT_EQUAL(12, (int)buf.peek(arData, 256));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 12));
	}
// 		} else if (part2 <= (unuse*2) && part1<=part2) {
// 			// u < p1 < p2 < 2*u
// 			// case 4: part1+part2 + part1+part2
// 			// part1+unuse+part2-unuse == part1+part2
// 			// 22222---1111111
// 			::memmove(_data+part2, _pbeg, part1);			// 222221111111---
// 			::memmove(_data+part1+part2, _data, unuse);		// ---221111111222
// 			::memmove(_data, _data+unuse, part2-unuse);		// 22---1111111222
// 			_pbeg = _pend;
// 			_pend -= unuse;									// 22---1111111111
// 			assert((size_t)(_pend-_data) <= unuse);
// 			_repos();										// (jmp to case 2)
// 			return;
	{
		CCycBufTestWrapper buf(15);
		// 7-4-5
		buf.PBeg() = buf.Data()+11;
		buf.PEnd() = buf.PBeg();
		buf.write(DATA, 12);
		buf.Repos();
		CPPUNIT_ASSERT_EQUAL(12, (int)buf.peek(arData, 256));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 12));
	}
// 		} else {
// 			// case 5:
// 			char *pnew = new char[_total];
// 			::memcpy(pnew, _pbeg, part1);
// 			::memcpy(pnew+part1, _data, part2);
// 			SafeDeleteArray(_data);
// 			_data = pnew;
	{
		CCycBufTestWrapper buf(15);
		// 6-2-8
		buf.PBeg() = buf.Data()+8;
		buf.PEnd() = buf.PBeg();
		buf.write(DATA, 14);
		buf.Repos();
		CPPUNIT_ASSERT_EQUAL(14, (int)buf.peek(arData, 256));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 14));
	}
}

void CTestCycBuf::testRLock()
{
	{
		CCycBufTestWrapper buf(15);
		buf.PBeg() = buf.Data()+12;
		buf.PEnd() = buf.PBeg();
		buf.write(DATA, 10);
		size_t nRd = 20;
		const void *pData = buf.rlock(nRd);
		CPPUNIT_ASSERT_EQUAL(10, (int)nRd);
		CPPUNIT_ASSERT_EQUAL(0, memcmp(pData, DATA, 10));

		CPPUNIT_ASSERT(!buf.runlock(((const char *)pData)+1, 10));
		CPPUNIT_ASSERT(!buf.runlock(pData, 12));
		CPPUNIT_ASSERT(buf.runlock(pData, 4));
		CPPUNIT_ASSERT_EQUAL(6, (int)buf.size());

		nRd = 20;
		pData = buf.rlock(nRd);
		CPPUNIT_ASSERT_EQUAL(6, (int)nRd);
		CPPUNIT_ASSERT_EQUAL(0, memcmp(pData, DATA+4, 6));
		CPPUNIT_ASSERT(buf.runlock(pData, 6));
		CPPUNIT_ASSERT(buf.empty());
	}
}

void CTestCycBuf::testWLock()
{
	char arData[256] = {0};
	{
		CCycBufTestWrapper buf(15);
		buf.PBeg() = buf.Data()+12;
		buf.PEnd() = buf.PBeg();
		buf.write(DATA, 5);
		size_t nWt = 9;
		void *pData = buf.wlock(nWt);
		CPPUNIT_ASSERT_EQUAL(9, (int)nWt);
		memcpy(pData, DATA+5, 6);
		
		CPPUNIT_ASSERT(!buf.wunlock(((char *)pData)+1, 9));
		CPPUNIT_ASSERT(!buf.wunlock(pData, 12));
		CPPUNIT_ASSERT(buf.wunlock(pData, 6));
		CPPUNIT_ASSERT_EQUAL(11, (int)buf.peek(arData, 256));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 11));

		nWt = 3;
		pData = buf.wlock(nWt);
		CPPUNIT_ASSERT_EQUAL(3, (int)nWt);
		memcpy(pData, DATA+11, 3);
		CPPUNIT_ASSERT(buf.wunlock(pData, 3));
		CPPUNIT_ASSERT_EQUAL(14, (int)buf.peek(arData, 256));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 14));
	}

	{
		CCycBufTestWrapper buf(15);
		buf.PBeg() = buf.Data()+12;
		buf.PEnd() = buf.PBeg();
		buf.write(DATA, 5);
		size_t nWt = 20;
		void *pData = buf.wlock(nWt);
		CPPUNIT_ASSERT_EQUAL(20, (int)nWt);
		memcpy(pData, DATA+5, 6);
		
		CPPUNIT_ASSERT(!buf.wunlock(((char *)pData)+1, 20));
		CPPUNIT_ASSERT(!buf.wunlock(pData, 25));
		CPPUNIT_ASSERT(buf.wunlock(pData, 6));
		CPPUNIT_ASSERT_EQUAL(11, (int)buf.peek(arData, 256));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 11));
		
		nWt = 9;
		pData = buf.wlock(nWt);
		CPPUNIT_ASSERT_EQUAL(9, (int)nWt);
		memcpy(pData, DATA+11, 9);
		CPPUNIT_ASSERT(buf.wunlock(pData, 9));
		CPPUNIT_ASSERT_EQUAL(20, (int)buf.peek(arData, 256));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 14));
	}
}

void CTestCycBuf::testWriteThenReadContinuous()
{
	{
		CCycBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteThenRead(buf, 2, 3));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CCycBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteThenRead(buf, 5, 3));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CCycBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteThenRead(buf, 97, 23));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CCycBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteThenRead(buf, 3, 2));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CCycBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteThenRead(buf, 3, 5));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CCycBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteThenRead(buf, 23, 97));
		CPPUNIT_ASSERT(buf.empty());
	}
}

void CTestCycBuf::testWriteAndReadContinuous()
{
	{
		CCycBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteAndRead(buf, 2, 3));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CCycBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteAndRead(buf, 3, 5));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CCycBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteAndRead(buf, 23, 97));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CCycBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteAndRead(buf, 3, 2));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CCycBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteAndRead(buf, 5, 3));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CCycBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteAndRead(buf, 97, 23));
		CPPUNIT_ASSERT(buf.empty());
	}
}

BOOL CTestCycBuf::_WriteThenRead( CCycBufTestWrapper &buf, int nWrStep, int nRdStep )
{
	char arData[256] = {0};
	const int DATA_LEN = ARR_LEN(DATA);

	{const int STEP = nWrStep;
	for (int i=0, nInx=0; i<nRdStep*1000; ++i) {
		ASSERT(nInx < DATA_LEN);
		int nWt = 0;
		if (nInx+STEP >= DATA_LEN) {
			nWt = DATA_LEN-nInx;
			buf.write(DATA+nInx, nWt);
			nInx = 0;
			
			nWt = STEP-nWt;
			buf.write(DATA+nInx, nWt);
			nInx += nWt;
		} else {
			nWt = STEP;
			buf.write(DATA+nInx, nWt);
			nInx += nWt;
		}
	}}
	
	{const int STEP = nRdStep;
	for (int i=0, nInx=0; i<nWrStep*1000; ++i) {
		ASSERT(nInx < DATA_LEN);
		int nRd = 0;
		if (nInx+STEP >= DATA_LEN) {
			nRd = DATA_LEN-nInx;
			buf.read(arData, nRd);
			if (0 != memcmp(arData, DATA+nInx, nRd)) { return FALSE; }
			nInx = 0;
			
			nRd = STEP-nRd;
			buf.read(arData, nRd);
			if (0 != memcmp(arData, DATA+nInx, nRd)) { return FALSE; }
			nInx += nRd;
		} else {
			nRd = STEP;
			buf.read(arData, nRd);
			if (0 != memcmp(arData, DATA+nInx, nRd)) { return FALSE; }
			nInx += nRd;
		}
	}}
	return TRUE;
}

BOOL CTestCycBuf::_WriteAndRead( CCycBufTestWrapper &buf, int nWrStep, int nRdStep )
{
	char arData[256] = {0};
	const int DATA_LEN = ARR_LEN(DATA);
	
	int nInxWt=0, nWtAccum=0;
	int nInxRd=0, nRdAccum=0;

	const int TOTAL = 1000*nWrStep*nRdStep;
	while (nRdAccum < TOTAL) {
		if (nWtAccum < TOTAL ) {
			ASSERT(nInxWt < DATA_LEN);
			int nWt = 0;
			if (nInxWt+nWrStep >= DATA_LEN) {
				nWt = DATA_LEN-nInxWt;
				buf.write(DATA+nInxWt, nWt);
				nInxWt = 0;
				
				nWt = nWrStep-nWt;
				buf.write(DATA+nInxWt, nWt);
				nInxWt += nWt;
			} else {
				nWt = nWrStep;
				buf.write(DATA+nInxWt, nWt);
				nInxWt += nWt;
			}
			nWtAccum += nWrStep;
		}
		
		if (buf.size() >= nRdStep) {
			ASSERT(nInxRd < DATA_LEN);
			int nRd = 0;
			if (nInxRd+nRdStep >= DATA_LEN) {
				nRd = DATA_LEN-nInxRd;
				buf.read(arData, nRd);
				if (0 != memcmp(arData, DATA+nInxRd, nRd)) { return FALSE; }
				nInxRd = 0;
				
				nRd = nRdStep-nRd;
				buf.read(arData, nRd);
				if (0 != memcmp(arData, DATA+nInxRd, nRd)) { return FALSE; }
				nInxRd += nRd;
			} else {
				nRd = nRdStep;
				buf.read(arData, nRd);
				if (0 != memcmp(arData, DATA+nInxRd, nRd)) { return FALSE; }
				nInxRd += nRd;
			}
			nRdAccum += nRdStep;
		}
	}
	return TRUE;
}
