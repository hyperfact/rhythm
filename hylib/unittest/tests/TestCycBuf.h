// TestCycBuf.h: interface for the CTestCycBuf class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTCYCBUF_H__35622CB6_432F_468E_B1CA_E8159B7C6F20__INCLUDED_)
#define AFX_TESTCYCBUF_H__35622CB6_432F_468E_B1CA_E8159B7C6F20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <cppunit/extensions/HelperMacros.h>
#include "buffer.h"

class CCycBufTestWrapper : public hy::cyc_buf<>
{
public:
	CCycBufTestWrapper(size_t n=255) : hy::cyc_buf<>(n) {}
	
	char	*&Data() { return _data; }
	size_t	&Total() { return _total; }
	char	*&PBeg() { return _pbeg; }
	char	*&PEnd() { return _pend; }
	unsigned &State() { return _state; }
	char *Last() { return _last(); }
	const char *Last() const { return _last(); }
	void Grow(size_t inc) { _grow(inc); }
	void Repos() { _repos(); }
};

class CTestCycBuf
	: public CPPUNIT_NS::TestFixture
	, public hy::noncopyable
{
	CPPUNIT_TEST_SUITE(CTestCycBuf);
	CPPUNIT_TEST(testSize);
 	CPPUNIT_TEST(testReserve);
	CPPUNIT_TEST(testPeek);
	CPPUNIT_TEST(testRead);
	CPPUNIT_TEST(testGrow);
	CPPUNIT_TEST(testWrite);
	CPPUNIT_TEST(testRepos);
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
	void testRepos();
	void testRLock();
	void testWLock();
	void testWriteThenReadContinuous();
	void testWriteAndReadContinuous();

private:
	BOOL _WriteThenRead(CCycBufTestWrapper &buf, int nWrStep, int nRdStep);
	BOOL _WriteAndRead(CCycBufTestWrapper &buf, int nWrStep, int nRdStep);

public:
	virtual void setUp();
	virtual void tearDown();
	
public:
	CTestCycBuf();
	virtual ~CTestCycBuf();

private:

public:
	typedef unsigned char byte_t;
	static byte_t DATA[256];
};

#endif // !defined(AFX_TESTCYCBUF_H__35622CB6_432F_468E_B1CA_E8159B7C6F20__INCLUDED_)
