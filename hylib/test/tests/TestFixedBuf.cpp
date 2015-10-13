// TestFixedBuf.cpp: implementation of the CTestFixedBuf class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestFixedBuf.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma function(memcmp)

////////////////////////////////////////////////////////////////////////////////
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(CTestFixedBuf, TEST_SUITE_NAME);

CTestFixedBuf::byte_t CTestFixedBuf::DATA[256];

namespace {
struct _Static_Init {
	_Static_Init() {
		for (int i=0; i<256; ++i) {
			CTestFixedBuf::DATA[i] = i;
		}
	}
} _static_init;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTestFixedBuf::CTestFixedBuf()
{

}

CTestFixedBuf::~CTestFixedBuf()
{

}

void CTestFixedBuf::testSize()
{
	const size_t BUF_LEN = 64;
	CFixedBufTestWrapper buf(BUF_LEN);
	//------------------------------------------------------------------------------
	CPPUNIT_ASSERT_EQUAL(0, (int)buf.size());
	CPPUNIT_ASSERT(buf.empty());
	CPPUNIT_ASSERT_EQUAL(BUF_LEN, buf.capacity());
	CPPUNIT_ASSERT_EQUAL(BUF_LEN, buf.avail());
	
	buf.PPos() = buf.Data()+8;
	CPPUNIT_ASSERT(!buf.empty());
	CPPUNIT_ASSERT_EQUAL(8, (int)buf.size());
	CPPUNIT_ASSERT_EQUAL(BUF_LEN-8, buf.avail());
	
	buf.PPos() = buf.Data()+buf.Total();
	CPPUNIT_ASSERT(!buf.empty());
	CPPUNIT_ASSERT_EQUAL(BUF_LEN, buf.size());
	CPPUNIT_ASSERT_EQUAL(0, (int)buf.avail());
}

void CTestFixedBuf::testReserve()
{
	{
		CFixedBufTestWrapper buf(1);
		buf.reserve(16);
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.size());
		CPPUNIT_ASSERT(buf.empty());
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.capacity());
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.avail());
	}
	
	{
		CFixedBufTestWrapper buf(1);
		buf.reserve(16);
		buf.PPos() = buf.Data()+8;
		memcpy(buf.Data(), DATA, 8);
		buf.reserve(32);
		CPPUNIT_ASSERT_EQUAL(0, memcmp(buf.Data(), DATA, 8));
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(32, (int)buf.capacity());
		CPPUNIT_ASSERT_EQUAL(24, (int)buf.avail());
	}
	
	{
		CFixedBufTestWrapper buf(1);
		buf.reserve(16);
		buf.PPos() = buf.Last();
		memcpy(buf.Data(), DATA, 16);
		buf.reserve(32);
		CPPUNIT_ASSERT_EQUAL(0, memcmp(buf.Data(), DATA, 16));
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(32, (int)buf.capacity());
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.avail());
	}
}

void CTestFixedBuf::testPeek()
{
	byte_t arData[256] = {0};
	
	{
		CFixedBufTestWrapper buf(16);
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.peek(arData, 16));
	}
	
	{
		CFixedBufTestWrapper buf(16);
		buf.PPos() = buf.Data()+8;
		memcpy(buf.Data(), DATA, 8);
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.peek(arData, 16));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 8));
	}
	
	{
		CFixedBufTestWrapper buf(16);
		buf.PPos() = buf.Last();
		memcpy(buf.Data(), DATA, 16);
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.peek(arData, 20));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 16));
	}
}

void CTestFixedBuf::testRead()
{
	byte_t arData[256] = {0};

	{
		CFixedBufTestWrapper buf(16);
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.read(arData, 16));
	}

	{
		CFixedBufTestWrapper buf(16);
		buf.PPos() = buf.Data()+8;
		memcpy(buf.Data(), DATA, 8);
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.read(arData, 16));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 8));
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.read(arData, 16));
	}

	{
		CFixedBufTestWrapper buf(16);
		buf.PPos() = buf.Data()+8;
		memcpy(buf.Data(), DATA, 8);
		CPPUNIT_ASSERT_EQUAL(5, (int)buf.read(arData, 5));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 5));
		CPPUNIT_ASSERT_EQUAL(3, (int)buf.read(arData, 10));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA+5, 3));
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.read(arData, 16));
	}
	
	{
		CFixedBufTestWrapper buf(16);
		buf.PPos() = buf.Last();
		memcpy(buf.Data(), DATA, 16);
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.read(arData, 20));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 16));
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.read(arData, 16));
	}

	{
		CFixedBufTestWrapper buf(16);
		buf.PPos() = buf.Last();
		memcpy(buf.Data(), DATA, 16);
		CPPUNIT_ASSERT_EQUAL(4, (int)buf.read(arData, 4));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA, 4));
		CPPUNIT_ASSERT_EQUAL(12, (int)buf.read(arData, 20));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(arData, DATA+4, 12));
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.read(arData, 16));
	}
}

void CTestFixedBuf::testGrow()
{
	CFixedBufTestWrapper buf(1);
	CPPUNIT_ASSERT_EQUAL(4, (int)buf.capacity());
	buf.Grow(2);
	CPPUNIT_ASSERT_EQUAL(8, (int)buf.capacity());
	buf.Grow(2);
	CPPUNIT_ASSERT_EQUAL(12, (int)buf.capacity());
	buf.Grow(7);
	CPPUNIT_ASSERT_EQUAL(20, (int)buf.capacity());
	buf.Grow(2);
	CPPUNIT_ASSERT_EQUAL(32, (int)buf.capacity());
	buf.Grow(32);
	CPPUNIT_ASSERT_EQUAL(64, (int)buf.capacity());
	buf.Grow(62);
	CPPUNIT_ASSERT_EQUAL(128, (int)buf.capacity());
}

void CTestFixedBuf::testWrite()
{
	byte_t arData[256] = {0};
	
	{
		CFixedBufTestWrapper buf(4);
		CPPUNIT_ASSERT_EQUAL(0, (int)buf.write(DATA, 0));
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.write(DATA, 16));
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(0, memcmp(buf.Data(), DATA, 16));
	}
	
	{
		CFixedBufTestWrapper buf(16);
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.write(DATA, 8));
		CPPUNIT_ASSERT_EQUAL(8, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.write(DATA+8, 16));
		CPPUNIT_ASSERT_EQUAL(24, (int)buf.size());
		
		CPPUNIT_ASSERT_EQUAL(24, (int)buf.peek(arData, 256));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(buf.Data(), DATA, 24));
	}
	
	{
		CFixedBufTestWrapper buf(16);
		CPPUNIT_ASSERT_EQUAL(7, (int)buf.write(arData, 7));
		CPPUNIT_ASSERT_EQUAL(7, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(9, (int)buf.write(DATA+7, 9));
		CPPUNIT_ASSERT_EQUAL(16, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(4, (int)buf.write(DATA+16, 4));
		CPPUNIT_ASSERT_EQUAL(20, (int)buf.size());
		CPPUNIT_ASSERT_EQUAL(20, (int)buf.peek(arData, 256));
		CPPUNIT_ASSERT_EQUAL(0, memcmp(buf.Data(), DATA, 20));
	}
}

void CTestFixedBuf::testRLock()
{
	{
		CFixedBufTestWrapper buf(15);
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

void CTestFixedBuf::testWLock()
{
	char arData[256] = {0};
	{
		CFixedBufTestWrapper buf(16);
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
		CFixedBufTestWrapper buf(16);
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

void CTestFixedBuf::testWriteThenReadContinuous()
{
	{
		CFixedBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteThenRead(buf, 2, 3));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CFixedBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteThenRead(buf, 5, 3));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CFixedBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteThenRead(buf, 97, 23));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CFixedBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteThenRead(buf, 3, 2));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CFixedBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteThenRead(buf, 3, 5));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CFixedBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteThenRead(buf, 23, 97));
		CPPUNIT_ASSERT(buf.empty());
	}
}

void CTestFixedBuf::testWriteAndReadContinuous()
{
	{
		CFixedBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteAndRead(buf, 2, 3));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CFixedBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteAndRead(buf, 3, 5));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CFixedBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteAndRead(buf, 23, 97));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CFixedBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteAndRead(buf, 3, 2));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CFixedBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteAndRead(buf, 5, 3));
		CPPUNIT_ASSERT(buf.empty());
	}
	{
		CFixedBufTestWrapper buf;
		CPPUNIT_ASSERT(_WriteAndRead(buf, 97, 23));
		CPPUNIT_ASSERT(buf.empty());
	}
}

BOOL CTestFixedBuf::_WriteThenRead( CFixedBufTestWrapper &buf, int nWrStep, int nRdStep )
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

BOOL CTestFixedBuf::_WriteAndRead( CFixedBufTestWrapper &buf, int nWrStep, int nRdStep )
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

void CTestFixedBuf::setUp()
{
	
}

void CTestFixedBuf::tearDown()
{
	
}
