// TestFixedBuf.h: interface for the CTestFixedBuf class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTFIXEDBUF_H__5A247259_2DF4_443B_A0A6_2A7EA941E20D__INCLUDED_)
#define AFX_TESTFIXEDBUF_H__5A247259_2DF4_443B_A0A6_2A7EA941E20D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <cppunit/extensions/HelperMacros.h>
#include "buffer.h"

class CFixedBufTestWrapper : public hy::fixed_buf<>
{
public:
	CFixedBufTestWrapper(size_t n=256) : hy::fixed_buf<>(n) {}
	
	char	*&Data() { return _data; }
	size_t	&Total() { return _total; }
	char	*&PPos() { return _ppos; }
	unsigned &State() { return _state; }
	char *Last() { return _last(); }
	const char *Last() const { return _last(); }
	void Grow(size_t inc) { _grow(inc); }
};

class CTestFixedBuf
	: public CPPUNIT_NS::TestFixture
	, public hy::noncopyable
{
	CPPUNIT_TEST_SUITE(CTestFixedBuf);
	CPPUNIT_TEST(testSize);
	CPPUNIT_TEST(testReserve);
	CPPUNIT_TEST(testPeek);
	CPPUNIT_TEST(testRead);
	CPPUNIT_TEST(testGrow);
	CPPUNIT_TEST(testWrite);
	CPPUNIT_TEST(testRLock);
	CPPUNIT_TEST(testWLock);
	CPPUNIT_TEST(testWriteThenReadContinuous);
	CPPUNIT_TEST(testWriteAndReadContinuous);
	CPPUNIT_TEST_SUITE_END();

	void testSize();
	void testReserve();
	void testPeek();
	void testRead();
	void testGrow();
	void testWrite();
	void testRLock();
	void testWLock();
	void testWriteThenReadContinuous();
	void testWriteAndReadContinuous();

private:
	BOOL _WriteThenRead(CFixedBufTestWrapper &buf, int nWrStep, int nRdStep);
	BOOL _WriteAndRead(CFixedBufTestWrapper &buf, int nWrStep, int nRdStep);
	
public:
	virtual void setUp();
	virtual void tearDown();

public:
	CTestFixedBuf();
	virtual ~CTestFixedBuf();

public:
	typedef unsigned char byte_t;
	static byte_t DATA[256];
};

#endif // !defined(AFX_TESTFIXEDBUF_H__5A247259_2DF4_443B_A0A6_2A7EA941E20D__INCLUDED_)
