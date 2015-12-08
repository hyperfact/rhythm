#include "stdafx.h"
#include "../include/chained_buf.h"
 
using namespace ::hy;
class CChainedBufTestWrapper : public chained_buf<>
{
public:
	typedef _NodePtr NodePtr;
	void Insert(_NodePtr cur, _NodePtr chain) {
		this->_Insert(cur, chain);
	}
	_NodePtr AllocChain(size_t expect_len) {
		return _AllocChain(expect_len);
	}
	void _FreeChain(_NodePtr chain) {
		_FreeChain(chain);
	}
	NodePtr &Beg() { return this->_Beg(); }
	NodePtr &Last() { return this->_Last(); }
	NodePtr End() { return this->_End(); }
	NodePtr Head() { return &this->_head; }
	NodePtr &LWD() { return this->_last_with_data; }
};

typedef CChainedBufTestWrapper CChainedBuf;
typedef CChainedBuf::NodePtr NodePtr;
typedef CChainedBuf::iovec IOVec;
const size_t MIN_EXPAND_SIZE = CChainedBuf::MIN_ALLOC_SIZE-sizeof(CChainedBuf::_buf_chain);
const char DATA[1024] = {0};

TEST(ChainedBuf, _Insert) {
	CChainedBuf buf;
	size_t total = 0;

	// no node
	EXPECT_TRUE(buf.Beg() == buf.Head());
	EXPECT_TRUE(buf.Last() == buf.Head());
	EXPECT_TRUE(buf.capacity() == total);

	// one node -> 1
	NodePtr chain1 = buf.AllocChain(100);
	total += chain1->capacity();
	buf.Insert(buf.Head(), chain1);
	EXPECT_TRUE(buf.Beg() == chain1);
	EXPECT_TRUE(buf.Last() == chain1);
	EXPECT_TRUE(buf.End() == buf.Head());
	EXPECT_TRUE(buf.capacity() == total);
	EXPECT_TRUE(chain1->next == buf.End());

	// insert to after Head -> 2-1
	NodePtr chain2 = buf.AllocChain(200);
	total += chain2->capacity();
	buf.Insert(buf.Head(), chain2);
	EXPECT_TRUE(buf.Beg() == chain2);
	EXPECT_TRUE(buf.Last() == chain1);
	EXPECT_TRUE(buf.End() == buf.Head());
	EXPECT_TRUE(buf.capacity() == total);
	EXPECT_TRUE(chain1->next == buf.End());
	EXPECT_TRUE(chain2->next == chain1);

	// insert to after Begin -> 2-3-1
	NodePtr chain3 = buf.AllocChain(200);
	total += chain3->capacity();
	buf.Insert(buf.Beg(), chain3);
	EXPECT_TRUE(buf.Beg() == chain2);
	EXPECT_TRUE(buf.Last() == chain1);
	EXPECT_TRUE(buf.End() == buf.Head());
	EXPECT_TRUE(buf.capacity() == total);
	EXPECT_TRUE(chain1->next == buf.End());
	EXPECT_TRUE(chain2->next == chain3);
	EXPECT_TRUE(chain3->next == chain1);

	// insert to after Last -> 2-3-1-4
	NodePtr chain4 = buf.AllocChain(200);
	total += chain4->capacity();
	buf.Insert(buf.Last(), chain4);
	EXPECT_TRUE(buf.Beg() == chain2);
	EXPECT_TRUE(buf.Last() == chain4);
	EXPECT_TRUE(buf.End() == buf.Head());
	EXPECT_TRUE(buf.capacity() == total);
	EXPECT_TRUE(chain1->next == chain4);
	EXPECT_TRUE(chain2->next == chain3);
	EXPECT_TRUE(chain3->next == chain1);
	EXPECT_TRUE(chain4->next == buf.End());

	// insert to after chain1 2-3-1-5-4
	NodePtr chain5 = buf.AllocChain(200);
	total += chain5->capacity();
	buf.Insert(chain1, chain5);
	EXPECT_TRUE(buf.Beg() == chain2);
	EXPECT_TRUE(buf.Last() == chain4);
	EXPECT_TRUE(buf.End() == buf.Head());
	EXPECT_TRUE(buf.capacity() == total);
	EXPECT_TRUE(chain1->next == chain5);
	EXPECT_TRUE(chain2->next == chain3);
	EXPECT_TRUE(chain3->next == chain1);
	EXPECT_TRUE(chain4->next == buf.End());
	EXPECT_TRUE(chain5->next == chain4);
}

TEST(ChainedBuf, write) {
	char DATA[1024];
	{
		/*
		CASE 1:
		chain1: 11122223333333
		chain2: 4444555555xxxx
		*/
		CChainedBuf buf;
		size_t len = 0;
		size_t wt = 0;

		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();
		EXPECT_TRUE(plst == buf.End());
		EXPECT_TRUE(plwd == buf.End());

		int a = 123;
		wt = buf.write(&a, sizeof(a));
		EXPECT_EQ(sizeof(a), wt);
		len += wt;
		EXPECT_EQ(len, buf.size());
		EXPECT_EQ(MIN_EXPAND_SIZE, buf.capacity());
		EXPECT_TRUE(plst != buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		plst = buf.Last();
		plwd = buf.LWD();
		EXPECT_TRUE(plst == plwd);

		char data1[] = "123";
		wt = buf.write(data1, sizeof(data1));
		EXPECT_EQ(sizeof(data1), wt);
		len += wt;
		EXPECT_EQ(len, buf.size());
		EXPECT_TRUE(plst == buf.Last());
		EXPECT_TRUE(plwd == buf.LWD());

		char data2[1024];
		size_t data2len = MIN_EXPAND_SIZE-len;
		wt = buf.write(data2, data2len);
		EXPECT_EQ(data2len, wt);
		len += wt;
		EXPECT_EQ(len, buf.size());
		EXPECT_TRUE(plst == buf.Last());
		EXPECT_TRUE(plwd == buf.LWD());

		char data3[128];
		wt = buf.write(data3, sizeof(data3));
		EXPECT_EQ(sizeof(data3), wt);
		len += wt;
		EXPECT_EQ(len, buf.size());
		EXPECT_TRUE(plst != buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		plst = buf.Last();
		plwd = buf.LWD();
		EXPECT_TRUE(plst == plwd);

		char data4[32];
		wt = buf.write(data4, sizeof(data4));
		EXPECT_EQ(sizeof(data4), wt);
		len += wt;
		EXPECT_EQ(len, buf.size());
		EXPECT_TRUE(plst == buf.Last());
		EXPECT_TRUE(plwd == buf.LWD());
	}
	{
		/*
		CASE 2:
		chain1: 111112222222
		chain2: 223333333xxx
		*/
		CChainedBuf buf;
		size_t len = 0;
		size_t wt = 0;

		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();

		char data1[128];
		wt = buf.write(data1, sizeof(data1));
		EXPECT_EQ(sizeof(data1), wt);
		len += wt;
		EXPECT_EQ(len, buf.size());
		EXPECT_TRUE(plst != buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		plst = buf.Last();
		plwd = buf.LWD();
		EXPECT_TRUE(plst == plwd);

		char data2[1024];
		size_t data2len = buf.avail() + 40;
		wt = buf.write(data2, data2len);
		EXPECT_EQ(data2len, wt);
		len += wt;
		EXPECT_EQ(len, buf.size());
		EXPECT_TRUE(plst != buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		plst = buf.Last();
		plwd = buf.LWD();
		EXPECT_TRUE(plst == plwd);

		char data3[128];
		wt = buf.write(data3, sizeof(data3));
		EXPECT_EQ(sizeof(data3), wt);
		len += wt;
		EXPECT_EQ(len, buf.size());
		EXPECT_TRUE(plst == buf.Last());
		EXPECT_TRUE(plwd == buf.LWD());
		EXPECT_TRUE(plst == plwd);
	}

	{
		/*
		CASE 3:
		chain1: xxxx1112222333
		chain2: 33344444444444
		*/
		CChainedBuf buf;
		size_t len = 0;
		size_t wt = 0;

		buf.reserve(128);

		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();
		
		NodePtr chain1 = buf.Beg();
		chain1->offset = 32;

		char data1[64];
		wt = buf.write(data1, sizeof(data1));
		EXPECT_EQ(sizeof(data1), wt);
		len += wt;
		EXPECT_EQ(len, buf.size());
		EXPECT_TRUE(plst == buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		EXPECT_TRUE(plst != plwd);
		plst = buf.Last();
		plwd = buf.LWD();

		size_t data2len = chain1->post_avail() - 40;
		wt = buf.write(DATA, data2len);
		EXPECT_EQ(data2len, wt);
		len += wt;
		EXPECT_EQ(len, buf.size());
		EXPECT_TRUE(plst == buf.Last());
		EXPECT_TRUE(plwd == buf.LWD());
		EXPECT_TRUE(plst == plwd);

		size_t data3len = 60;
		wt = buf.write(DATA, data3len);
		EXPECT_EQ(data3len, wt);
		len += wt;
		EXPECT_EQ(len, buf.size());
		EXPECT_TRUE(plst != buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		plst = buf.Last();
		plwd = buf.LWD();
		EXPECT_TRUE(plst == plwd);

		size_t data4len = plwd->post_avail();
		wt = buf.write(DATA, data4len);
		EXPECT_EQ(data4len, wt);
		len += wt;
		EXPECT_EQ(len, buf.size());
		EXPECT_TRUE(plst == buf.Last());
		EXPECT_TRUE(plwd == buf.LWD());
		EXPECT_TRUE(plst == plwd);
	}
	{
		/*
		CASE 4:
		chain1: xxxx111xxxxxx + 22222222
		chain1: 11122222222xx
		*/
		CChainedBuf buf;
		size_t len = 0;
		size_t wt = 0;

		buf.reserve(128);

		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();
		
		NodePtr chain1 = buf.Beg();
		chain1->offset = 32;
		buf.Head()->offset = 32;

		size_t data1len = 2;
		wt = buf.write(DATA, data1len);
		EXPECT_EQ(data1len, wt);
		len += wt;
		EXPECT_EQ(len, buf.size());
		EXPECT_TRUE(plst == buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		EXPECT_TRUE(plst != plwd);
		plst = buf.Last();
		plwd = buf.LWD();

		size_t data2len = chain1->post_avail() + chain1->pre_avail()-16;
		wt = buf.write(DATA, data2len);
		EXPECT_EQ(data2len, wt);
		len += wt;
		EXPECT_EQ(len, buf.size());
		EXPECT_TRUE(plst == buf.Last());
		EXPECT_TRUE(plwd == buf.LWD());
		EXPECT_TRUE(plst == plwd);

		size_t data3len = 32;
		wt = buf.write(DATA, data3len);
		EXPECT_EQ(data3len, wt);
		len += wt;
		EXPECT_EQ(len, buf.size());
		EXPECT_TRUE(plst != buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		plst = buf.Last();
		plwd = buf.LWD();
		EXPECT_TRUE(plst == plwd);
	}
	{
		/*
		CASE 5:
		chain1: xxxx111xxxxxx + 2222222222
		chain1: 1112222222222
		*/
		CChainedBuf buf;
		size_t len = 0;
		size_t wt = 0;

		buf.reserve(128);

		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();
		
		NodePtr chain1 = buf.Beg();
		chain1->offset = 32;
		buf.Head()->offset = 32;

		size_t data1len = 2;
		wt = buf.write(DATA, data1len);
		EXPECT_EQ(data1len, wt);
		len += wt;
		EXPECT_EQ(len, buf.size());
		EXPECT_TRUE(plst == buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		EXPECT_TRUE(plst != plwd);
		plst = buf.Last();
		plwd = buf.LWD();

		size_t data2len = chain1->post_avail() + chain1->pre_avail();
		wt = buf.write(DATA, data2len);
		EXPECT_EQ(data2len, wt);
		len += wt;
		EXPECT_EQ(len, buf.size());
		EXPECT_TRUE(plst == buf.Last());
		EXPECT_TRUE(plwd == buf.LWD());
		EXPECT_TRUE(plst == plwd);

		size_t data3len = 32;
		wt = buf.write(DATA, data3len);
		EXPECT_EQ(data3len, wt);
		len += wt;
		EXPECT_EQ(len, buf.size());
		EXPECT_TRUE(plst != buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		plst = buf.Last();
		plwd = buf.LWD();
		EXPECT_TRUE(plst == plwd);
	}
}

TEST(ChainedBuf, read) {
	char buf_out[1024];
	{
		/*
		CASE 1:
		111222233333xxxx
		*/
		CChainedBuf buf;
		size_t len = 0;
		size_t rd = 0;

		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();
		EXPECT_TRUE(plst == buf.End());
		EXPECT_TRUE(plwd == buf.End());

		size_t datalen = 256;
		EXPECT_EQ(datalen, buf.write(DATA, datalen));
		EXPECT_EQ(datalen, buf.size());
		plst = buf.Last();
		plwd = buf.LWD();

		EXPECT_EQ(0, buf.read(buf_out, 0));

		size_t rdlen1 = 96;
		EXPECT_EQ(rdlen1, buf.read(buf_out, rdlen1));
		datalen -= rdlen1;
		EXPECT_EQ(datalen, buf.size());

		size_t rdlen2 = 64;
		EXPECT_EQ(rdlen2, buf.read(buf_out, rdlen2));
		datalen -= rdlen2;
		EXPECT_EQ(datalen, buf.size());

		EXPECT_EQ(0, buf.read(buf_out, 0));

		size_t rdlen3 = datalen;
		EXPECT_EQ(rdlen3, buf.read(buf_out, rdlen3));
		datalen -= rdlen3;
		EXPECT_EQ(0, buf.size());

		EXPECT_EQ(0, buf.read(buf_out, 10));
		EXPECT_EQ(0, buf.read(buf_out, 0));
		EXPECT_EQ(0, buf.read(buf_out, 10));
	}

	{
		/*
		CASE 2:
		111222233333xxxx
		*/
		CChainedBuf buf;
		size_t len = 0;
		size_t rd = 0;

		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();

		size_t datalen = 256;
		EXPECT_EQ(datalen, buf.write(DATA, datalen));
		EXPECT_EQ(datalen, buf.size());
		plst = buf.Last();
		plwd = buf.LWD();

		EXPECT_EQ(0, buf.read(buf_out, 0));

		size_t rdlen1 = 96;
		EXPECT_EQ(rdlen1, buf.read(buf_out, rdlen1));
		datalen -= rdlen1;
		EXPECT_EQ(datalen, buf.size());

		size_t rdlen2 = datalen + 20;
		rd = buf.read(buf_out, rdlen2);
		EXPECT_NE(rdlen2, rd);
		EXPECT_EQ(datalen, rd);
		datalen -= rd;
		EXPECT_EQ(0, buf.size());
	}

	{
		/*
		CASE 3:
		111222233333
		4444xxxxxxxx
		*/
		CChainedBuf buf;
		size_t len = 0;
		size_t rd = 0;

		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();

		size_t datalen = 128;
		EXPECT_EQ(datalen, buf.write(DATA, datalen));
		EXPECT_EQ(datalen, buf.size());
		plst = buf.Last();
		plwd = buf.LWD();

		size_t wtlen2 = 200;
		EXPECT_EQ(wtlen2, buf.write(DATA, wtlen2));
		datalen += wtlen2;
		EXPECT_EQ(datalen, buf.size());
		plst = buf.Last();
		plwd = buf.LWD();

		size_t rdlen1 = 96;
		EXPECT_EQ(rdlen1, buf.read(buf_out, rdlen1));
		datalen -= rdlen1;
		EXPECT_EQ(datalen, buf.size());

		NodePtr chain = buf.Beg();
		size_t rdlen2 = chain->size()+10;
		EXPECT_EQ(rdlen2, buf.read(buf_out, rdlen2));
		datalen -= rdlen2;
		EXPECT_EQ(datalen, buf.size());

		chain = buf.LWD();
		size_t rdlen3 = datalen + 10;
		rd = buf.read(buf_out, rdlen3);
		EXPECT_NE(rdlen3, rd);
		EXPECT_EQ(datalen, rd);
		datalen -= rd;
		EXPECT_EQ(0, buf.size());
	}
}

TEST(ChainedBuf, wlock) {
	char data_out[1024];
	{
		/*
		CASE 1:
		chain1 nil
		*/
		CChainedBuf buf;
		
		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();
		EXPECT_TRUE(plst == buf.End());
		EXPECT_TRUE(plwd == buf.End());

		size_t llen_ep = 128;
		size_t llen = llen_ep;
		void *p = buf.wlock(llen);
		EXPECT_EQ(llen_ep, llen);
		EXPECT_TRUE(plst != buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		plst = buf.Last();
		plwd = buf.LWD();

		EXPECT_FALSE(buf.wunlock(p, buf.capacity()+1));
		EXPECT_TRUE(buf.wunlock(p, llen_ep));
		EXPECT_EQ(llen_ep, buf.size());
	}

	{
		/*
		CASE 2:
		chain1 11xxxxxxx + 222 + 3333
		*/
		CChainedBuf buf;
		size_t avail = 0, writelen = 0;
		
		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();

		size_t llen_ep1 = 128;
		size_t llen1 = llen_ep1;
		void *p = buf.wlock(llen1);
		EXPECT_EQ(llen_ep1, llen1);
		EXPECT_TRUE(plst != buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		plst = buf.Last();
		plwd = buf.LWD();
		avail = plwd->post_avail();
		EXPECT_TRUE(buf.wunlock(p, 80));
		avail -= 80;
		writelen += 80;
		EXPECT_EQ(writelen, buf.size());

		size_t llen_ep2 = 90;
		size_t llen2 = llen_ep2;
		p = buf.wlock(llen2);
		EXPECT_EQ(llen_ep2, llen2);
		EXPECT_TRUE(plst == buf.Last());
		EXPECT_TRUE(plwd == buf.LWD());
		EXPECT_TRUE(buf.wunlock(p, llen2));
		avail -= llen2;
		writelen += llen2;
		EXPECT_EQ(writelen, buf.size());

		size_t llen_ep3 = avail;
		size_t llen3 = llen_ep3;
		p = buf.wlock(llen3);
		EXPECT_EQ(llen_ep3, llen3);
		EXPECT_TRUE(plst == buf.Last());
		EXPECT_TRUE(plwd == buf.LWD());
		EXPECT_TRUE(buf.wunlock(p, llen3));
		avail -= llen3;
		writelen += llen3;
		EXPECT_EQ(writelen, buf.size());
		EXPECT_EQ(0, avail);
	}

	{
		/*
		CASE 3:
		chain1 xx11xxxxxxx + 222 + 3333
		*/
		CChainedBuf buf;
		size_t avail=0, writelen=0;
		
		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();

		// prepare
		size_t llen_ep1 = 128;
		size_t llen1 = llen_ep1;
		void *p = buf.wlock(llen1);
		EXPECT_EQ(llen_ep1, llen1);
		EXPECT_TRUE(plst != buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		plst = buf.Last();
		plwd = buf.LWD();
		avail = plwd->post_avail();
		EXPECT_TRUE(buf.wunlock(p, 80));
		avail -= 80;
		writelen += 80;
		EXPECT_EQ(writelen, buf.size());
		EXPECT_EQ(30, buf.read(data_out, 30));
		writelen -= 30;

		// start test:
		size_t llen_ep2 = 90;
		size_t llen2 = llen_ep2;
		p = buf.wlock(llen2);
		EXPECT_EQ(llen_ep2, llen2);
		EXPECT_TRUE(plst == buf.Last());
		EXPECT_TRUE(plwd == buf.LWD());
		EXPECT_TRUE(buf.wunlock(p, llen2));
		avail -= llen2;
		writelen += llen2;
		EXPECT_EQ(writelen, buf.size());

		size_t llen_ep3 = plwd->post_avail();
		size_t llen3 = llen_ep3;
		p = buf.wlock(llen3);
		EXPECT_EQ(llen_ep3, llen3);
		EXPECT_TRUE(plst == buf.Last());
		EXPECT_TRUE(plwd == buf.LWD());
		EXPECT_TRUE(buf.wunlock(p, llen3));
		avail -= llen3;
		writelen += llen3;
		EXPECT_EQ(writelen, buf.size());
		EXPECT_EQ(0, avail);
	}

	{
		/*
		CASE 4:
		chain1 xx11xxxxxxx + 222 + 333333
		*/
		CChainedBuf buf;
		size_t avail=0, writelen=0;
		
		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();

		// prepare
		size_t llen_ep1 = 128;
		size_t llen1 = llen_ep1;
		void *p = buf.wlock(llen1);
		EXPECT_EQ(llen_ep1, llen1);
		EXPECT_TRUE(plst != buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		plst = buf.Last();
		plwd = buf.LWD();
		avail = plwd->post_avail();
		EXPECT_TRUE(buf.wunlock(p, 40));
		avail -= 40;
		writelen += 40;
		EXPECT_EQ(writelen, buf.size());
		EXPECT_EQ(30, buf.read(data_out, 30));
		writelen -= 30;

		// start test:
		size_t llen_ep2 = 20;
		size_t llen2 = llen_ep2;
		p = buf.wlock(llen2);
		EXPECT_EQ(llen_ep2, llen2);
		EXPECT_TRUE(plst == buf.Last());
		EXPECT_TRUE(plwd == buf.LWD());
		EXPECT_TRUE(buf.wunlock(p, llen2));
		avail -= llen2;
		writelen += llen2;
		EXPECT_EQ(writelen, buf.size());

		size_t llen_ep3 = plwd->avail();
		size_t llen3 = llen_ep3;
		p = buf.wlock(llen3);
		EXPECT_EQ(llen_ep3, llen3);
		EXPECT_TRUE(plst == buf.Last());
		EXPECT_TRUE(plwd == buf.LWD());
		EXPECT_TRUE(buf.wunlock(p, llen3));
		writelen += llen3;
		EXPECT_EQ(writelen, buf.size());
		EXPECT_EQ(0, plwd->avail());
	}

	{
		/*
		CASE 5:
		chain1 1111xxxxx + 222222
		*/
		CChainedBuf buf;
		size_t writelen=0;
		
		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();
		EXPECT_TRUE(plst == buf.End());
		EXPECT_TRUE(plwd == buf.End());

		size_t llen_ep = 128;
		size_t llen = llen_ep;
		void *p = buf.wlock(llen);
		EXPECT_EQ(llen_ep, llen);
		EXPECT_TRUE(plst != buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		plst = buf.Last();
		plwd = buf.LWD();

		EXPECT_TRUE(buf.wunlock(p, llen_ep));
		writelen += llen_ep;
		EXPECT_EQ(writelen, buf.size());

		size_t llen_ep2 = 256;
		size_t llen2 = llen_ep2;
		p = buf.wlock(llen2);
		EXPECT_EQ(llen_ep2, llen2);
		EXPECT_TRUE(plst != buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		plst = buf.Last();
		plwd = buf.LWD();
		EXPECT_TRUE(buf.wunlock(p, llen2));
		writelen += llen2;
		EXPECT_EQ(writelen, buf.size());
	}

	{
		/*
		CASE 3:
		chain1 xx11xxxxxxx + 222 + 3333
		*/
		CChainedBuf buf;
		size_t avail=0, writelen=0;
		
		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();

		// prepare
		size_t llen_ep1 = 128;
		size_t llen1 = llen_ep1;
		void *p = buf.wlock(llen1);
		EXPECT_EQ(llen_ep1, llen1);
		EXPECT_TRUE(plst != buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		plst = buf.Last();
		plwd = buf.LWD();
		avail = plwd->post_avail();
		EXPECT_TRUE(buf.wunlock(p, 80));
		avail -= 80;
		writelen += 80;
		EXPECT_EQ(writelen, buf.size());
		EXPECT_EQ(30, buf.read(data_out, 30));
		writelen -= 30;

		size_t llen_ep2 = 256;
		size_t llen2 = llen_ep2;
		p = buf.wlock(llen2);
		EXPECT_EQ(llen_ep2, llen2);
		EXPECT_TRUE(plst != buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		plst = buf.Last();
		plwd = buf.LWD();
		EXPECT_TRUE(buf.wunlock(p, llen2));
		writelen += llen2;
		EXPECT_EQ(writelen, buf.size());
	}
}

TEST(ChainedBuf, rlock) {
	{
		/*
		CASE 1:
		*/
		CChainedBuf buf;

		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();
		EXPECT_TRUE(plst == buf.End());
		EXPECT_TRUE(plwd == buf.End());

		size_t llen = 0;
		EXPECT_TRUE(nullptr == buf.rlock(llen));
		llen = 10;
		EXPECT_TRUE(nullptr == buf.rlock(llen));

		// preparing:
		size_t wtlen = 256;
		EXPECT_EQ(wtlen, buf.write(DATA, wtlen));
		plst = buf.Last();
		plwd = buf.LWD();

		size_t rllen = 0;
		const void *p = buf.rlock(rllen);
		EXPECT_EQ(nullptr, p);
		EXPECT_EQ(0, rllen);
		EXPECT_EQ(wtlen, buf.size());

		rllen = buf.size();
		p = buf.rlock(rllen);
		EXPECT_EQ(plwd->pbegin(), p);
		EXPECT_EQ(wtlen, rllen);
		EXPECT_TRUE(buf.runlock(p, 0));
		EXPECT_EQ(wtlen, buf.size());

		rllen = buf.size()+10;
		p = buf.rlock(rllen);
		EXPECT_EQ(plwd->pbegin(), p);
		EXPECT_EQ(wtlen, rllen);
		EXPECT_TRUE(buf.runlock(p, 0));
		EXPECT_EQ(wtlen, buf.size());

		rllen = buf.size()-10;
		p = buf.rlock(rllen);
		EXPECT_EQ(plwd->pbegin(), p);
		EXPECT_EQ(wtlen-10, rllen);
		EXPECT_TRUE(buf.runlock(p, 0));
		EXPECT_EQ(wtlen, buf.size());

		rllen = buf.size();
		p = buf.rlock(rllen);
		EXPECT_EQ(plwd->pbegin(), p);
		EXPECT_EQ(wtlen, rllen);
		EXPECT_TRUE(buf.runlock(p, 10));
		wtlen -= 10;
		EXPECT_EQ(wtlen, buf.size());

		rllen = buf.size();
		p = buf.rlock(rllen);
		EXPECT_EQ(plwd->pbegin(), p);
		EXPECT_EQ(wtlen, rllen);
		EXPECT_TRUE(buf.runlock(p, wtlen-10));
		wtlen = 10;
		EXPECT_EQ(wtlen, buf.size());

		rllen = buf.size();
		p = buf.rlock(rllen);
		EXPECT_EQ(plwd->pbegin(), p);
		EXPECT_EQ(wtlen, rllen);
		EXPECT_FALSE(buf.runlock(p, wtlen+10));
		EXPECT_TRUE(buf.runlock(p, wtlen));
		wtlen = 0;
		EXPECT_EQ(wtlen, buf.size());
	}
	{
		/*
		CASE 1:
		*/
		CChainedBuf buf;

		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();

		// preparing:
		size_t sumwtlen = 0;
		size_t wtlen1 = 256;
		EXPECT_EQ(wtlen1, buf.write(DATA, wtlen1));
		sumwtlen += wtlen1;
		EXPECT_TRUE(plst != buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		plst = buf.Last();
		NodePtr chain1 = plwd = buf.LWD();
		size_t wtlen2 = 260;
		EXPECT_EQ(wtlen2, buf.write(DATA, wtlen2));
		sumwtlen += wtlen2;
		EXPECT_TRUE(plst != buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		plst = buf.Last();
		NodePtr chain2 = plwd = buf.LWD();
		size_t wtlen3 = 320;
		EXPECT_EQ(wtlen3, buf.write(DATA, wtlen3));
		sumwtlen += wtlen3;
		EXPECT_TRUE(plst != buf.Last());
		EXPECT_TRUE(plwd != buf.LWD());
		plst = buf.Last();
		NodePtr chain3 = plwd = buf.LWD();

		size_t rllen = 10;
		const void *p = buf.rlock(rllen);
		EXPECT_EQ(chain1->pbegin(), p);
		EXPECT_EQ(10, rllen);
		EXPECT_TRUE(buf.runlock(p, rllen));
		sumwtlen -= rllen;
		wtlen1 -= rllen;
		EXPECT_EQ(sumwtlen, buf.size());

		rllen = wtlen1-10;
		p = buf.rlock(rllen);
		EXPECT_EQ(chain1->pbegin(), p);
		EXPECT_EQ(wtlen1-10, rllen);
		EXPECT_TRUE(buf.runlock(p, rllen));
		wtlen1 -= rllen;
		sumwtlen -= rllen;
		EXPECT_EQ(sumwtlen, buf.size());

		rllen = 100;
		p = buf.rlock(rllen);
		EXPECT_TRUE(buf.Beg() == chain1);
		EXPECT_EQ(chain1->pbegin(), p);
		EXPECT_EQ(100, rllen);
		EXPECT_TRUE(buf.runlock(p, 80));
		EXPECT_EQ(rllen-80, chain1->size());
		wtlen1 = rllen-80;
		sumwtlen -= 80;
		EXPECT_EQ(sumwtlen, buf.size());

		rllen = 80;
		p = buf.rlock(rllen);
		EXPECT_TRUE(buf.Beg() == chain1);
		EXPECT_EQ(chain1->pbegin(), p);
		EXPECT_EQ(80, rllen);
		EXPECT_TRUE(buf.runlock(p, 60));
		EXPECT_EQ(rllen-60, chain1->size());
		wtlen1 = rllen-60;
		sumwtlen -= 60;
		EXPECT_EQ(sumwtlen, buf.size());

		size_t largerllen = chain1->size() + chain2->size() + chain3->size() - 20;
		rllen = largerllen;
		p = buf.rlock(rllen);
		EXPECT_TRUE(buf.Beg() != chain1);
		chain1 = buf.Beg();
		EXPECT_EQ(chain1->pbegin(), p);
		EXPECT_EQ(largerllen, rllen);
		EXPECT_TRUE(buf.runlock(p, largerllen));
		EXPECT_TRUE(buf.Beg() != chain1);
		chain1 = buf.Beg();
		EXPECT_EQ(20, chain1->size());
		sumwtlen -= largerllen;
		EXPECT_EQ(sumwtlen, buf.size());
	}
}

size_t sum_iovec(const IOVec *vecs, int nvec) {
	size_t sum = 0;
	for (int i=0; i<nvec; ++i)
		sum += vecs[i].len;
	return sum;
}

TEST(ChainedBuf, wlock_iovec) {
	{
		/*
		CASE 1:
		*/
		CChainedBuf buf;

		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();
		EXPECT_TRUE(plst == buf.End());
		EXPECT_TRUE(plwd == buf.End());

		size_t wtlen = 0;
		IOVec iov[10] = {0};
		int nvecs = 0;
		size_t wlen = 0;
		EXPECT_EQ(0, buf.wlock(wlen, nullptr, 0));
		wlen = 10;
		EXPECT_EQ(1, buf.wlock(wlen, nullptr, 0));

		wlen = 200;
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		iov[0].len = 0;
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		EXPECT_EQ(0, buf.size());

		wlen = 0;
		EXPECT_EQ(0, buf.wlock(wlen, nullptr, 0));
		wlen = 200;
		EXPECT_EQ(1, buf.wlock(wlen, nullptr, 0));
		wlen = buf.Beg()->avail();
		EXPECT_EQ(1, buf.wlock(wlen, nullptr, 0));
		wlen = buf.Beg()->avail()+10;
		EXPECT_EQ(2, buf.wlock(wlen, nullptr, 0));

		wlen = 200;
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		iov[0].len = buf.Beg()->avail()+10;
		EXPECT_FALSE(buf.wunlock(iov, nvecs));
		iov[0].len = 80;
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		wtlen += 80;
		EXPECT_EQ(wtlen, buf.size());

		wlen = 100;
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		iov[0].len = wlen;
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		wtlen += wlen;
		EXPECT_EQ(wtlen, buf.size());

		wlen = 10;
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.wunlock(nullptr, 0));
		wtlen += 0;
		EXPECT_EQ(wtlen, buf.size());

		wlen = buf.Beg()->avail();
		EXPECT_EQ(1, buf.wlock(wlen, nullptr, 0));
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		iov[0].len = 0;
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		wtlen += iov[0].len;
		EXPECT_EQ(wtlen, buf.size());

		wlen = buf.Beg()->avail()+20;
		EXPECT_EQ(2, buf.wlock(wlen, nullptr, 0));
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(2, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		iov[1].len = 0;
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		wtlen += sum_iovec(iov, nvecs);
		EXPECT_EQ(wtlen, buf.size());

		wlen = 120;
		EXPECT_EQ(1, buf.wlock(wlen, nullptr, 0));
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		wtlen += sum_iovec(iov, nvecs);
		EXPECT_EQ(wtlen, buf.size());

		wlen = buf.LWD()->avail()+100;
		EXPECT_EQ(2, buf.wlock(wlen, nullptr, 0));
		EXPECT_EQ(0, buf.wlock(wlen, iov, 0));
		EXPECT_TRUE(buf.wunlock(iov, 0));
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(2, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		iov[0].len = 0;
		iov[1].len = 0;
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		wtlen += sum_iovec(iov, nvecs);
		EXPECT_EQ(wtlen, buf.size());

		wlen = buf.LWD()->avail()+100;
		EXPECT_EQ(2, buf.wlock(wlen, nullptr, 0));
		nvecs = buf.wlock(wlen, iov, 1);
		EXPECT_EQ(1, nvecs);
		EXPECT_GE(wlen, sum_iovec(iov, nvecs));
		iov[0].len = 0;
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		wtlen += sum_iovec(iov, nvecs);
		EXPECT_EQ(wtlen, buf.size());
	}
	{
		/*
		CASE 2:
		*/
		CChainedBuf buf;

		size_t wtlen = 0;
		IOVec iov[10] = {0};
		int nvecs = 0;
		size_t wlen = 0;

		wlen = 200;
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		iov[0].len = 0;
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		EXPECT_EQ(0, buf.size());

		buf.LWD()->offset = 128;

		wlen = 20;
		EXPECT_EQ(1, buf.wlock(wlen, nullptr, 0));
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		wtlen += sum_iovec(iov, nvecs);
		EXPECT_EQ(wtlen, buf.size());

		wlen = buf.LWD()->avail()-10;
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		iov[0].len = 0;
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		wtlen += sum_iovec(iov, nvecs);
		EXPECT_EQ(wtlen, buf.size());
	}

	{
		/*
		CASE 3:
		*/
		CChainedBuf buf;

		size_t wtlen = 0;
		IOVec iov[10] = {0};
		int nvecs = 0;
		size_t wlen = 0;

		wlen = 200;
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		iov[0].len = 0;
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		EXPECT_EQ(0, buf.size());

		buf.LWD()->offset = 128;

		wlen = 20;
		EXPECT_EQ(1, buf.wlock(wlen, nullptr, 0));
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		wtlen += sum_iovec(iov, nvecs);
		EXPECT_EQ(wtlen, buf.size());

		wlen = buf.LWD()->avail();
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		wtlen += sum_iovec(iov, nvecs);
		EXPECT_EQ(wtlen, buf.size());
	}
	{
		/*
		CASE 4:
		*/
		CChainedBuf buf;

		size_t wtlen = 0;
		IOVec iov[10] = {0};
		int nvecs = 0;
		size_t wlen = 0;

		wlen = 200;
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		iov[0].len = 0;
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		EXPECT_EQ(0, buf.size());

		buf.LWD()->offset = 128;

		wlen = 20;
		EXPECT_EQ(1, buf.wlock(wlen, nullptr, 0));
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		wtlen += sum_iovec(iov, nvecs);
		EXPECT_EQ(wtlen, buf.size());

		wlen = buf.LWD()->avail()+80;
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(2, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		wtlen += sum_iovec(iov, nvecs);
		EXPECT_EQ(wtlen, buf.size());
	}
}

TEST(ChainedBuf, rlock_iovec) {
	{
		/*
		CASE 1:
		*/
		CChainedBuf buf;

		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();
		EXPECT_TRUE(plst == buf.End());
		EXPECT_TRUE(plwd == buf.End());

		IOVec iov[10] = {0};
		int nvecs = 0;
		size_t rlen = 0;
		size_t wtlen = 0;
		size_t remain = 0;

		rlen = 0;
		EXPECT_EQ(0, buf.rlock(rlen, nullptr, 0));
		rlen = 10;
		EXPECT_EQ(0, buf.rlock(rlen, nullptr, 0));
		rlen = 0;
		EXPECT_EQ(0, buf.rlock(rlen, iov, 10));
		rlen = 10;
		EXPECT_EQ(0, buf.rlock(rlen, iov, 10));

		// prepare:
		wtlen += buf.write(DATA, 200);

		rlen = 0;
		EXPECT_EQ(0, buf.rlock(rlen, nullptr, 0));
		rlen = 10;
		EXPECT_EQ(1, buf.rlock(rlen, nullptr, 0));
		rlen = wtlen+10;
		EXPECT_EQ(1, buf.rlock(rlen, nullptr, 0));
		EXPECT_EQ(wtlen, rlen);

		rlen = wtlen;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(0));
		wtlen -= 0;
		EXPECT_EQ(wtlen, buf.size());

		rlen = wtlen+10;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(wtlen, rlen);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wtlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(0));
		wtlen -= 0;
		EXPECT_EQ(wtlen, buf.size());

		rlen = 40;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(20));
		wtlen -= 20;
		EXPECT_EQ(wtlen, buf.size());

		rlen = 40;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_FALSE(buf.runlock(buf.size()+10));
		EXPECT_TRUE(buf.runlock(rlen));
		wtlen -= rlen;
		EXPECT_EQ(wtlen, buf.size());

		rlen = wtlen;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(rlen));
		wtlen -= rlen;
		EXPECT_EQ(wtlen, buf.size());
	}
	{
		/*
		CASE 1:
		*/
		CChainedBuf buf;

		NodePtr plst = buf.Last();
		NodePtr plwd = buf.LWD();
		EXPECT_TRUE(plst == buf.End());
		EXPECT_TRUE(plwd == buf.End());

		IOVec iov[10] = {0};
		int nvecs = 0;
		size_t rlen = 0;
		size_t wtlen = 0;
		size_t remain = 0;

		// prepare:
		wtlen += buf.write(DATA, 200);
		wtlen += buf.write(DATA, 300);

		rlen = 0;
		EXPECT_EQ(0, buf.rlock(rlen, nullptr, 0));
		rlen = 10;
		EXPECT_EQ(1, buf.rlock(rlen, nullptr, 0));
		rlen = wtlen-10;
		EXPECT_EQ(2, buf.rlock(rlen, nullptr, 0));
		rlen = wtlen;
		EXPECT_EQ(2, buf.rlock(rlen, nullptr, 0));
		rlen = wtlen+10;
		EXPECT_EQ(2, buf.rlock(rlen, nullptr, 0));
		EXPECT_EQ(wtlen, rlen);

		rlen = wtlen;
		nvecs = buf.rlock(rlen, iov, 1);
		EXPECT_EQ(1, nvecs);
		EXPECT_LT(rlen, wtlen);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(0));
		
		rlen = wtlen;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(2, nvecs);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(0));
		wtlen -= 0;
		EXPECT_EQ(wtlen, buf.size());

		rlen = wtlen+10;
		nvecs = buf.rlock(rlen, iov, 1);
		EXPECT_EQ(1, nvecs);
		EXPECT_LT(rlen, wtlen);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(0));

		rlen = wtlen+10;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(wtlen, rlen);
		EXPECT_EQ(2, nvecs);
		EXPECT_EQ(wtlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(0));
		wtlen -= 0;
		EXPECT_EQ(wtlen, buf.size());

		rlen = 40;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(20));
		wtlen -= 20;
		EXPECT_EQ(wtlen, buf.size());

		rlen = 40;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_FALSE(buf.runlock(buf.size()+10));
		EXPECT_TRUE(buf.runlock(rlen));
		wtlen -= rlen;
		EXPECT_EQ(wtlen, buf.size());

		rlen = wtlen-20;
		nvecs = buf.rlock(rlen, iov, 1);
		EXPECT_EQ(1, nvecs);
		EXPECT_LT(rlen, wtlen);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(0));

		rlen = wtlen-20;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(2, nvecs);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(40));
		wtlen -= 40;
		EXPECT_EQ(wtlen, buf.size());

		rlen = wtlen;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(2, nvecs);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(rlen));
		wtlen -= rlen;
		EXPECT_EQ(wtlen, buf.size());
	}
}

TEST(ChainedBuf, transplant) {
	{
		/*
		CASE 1:
		buf1: nil
		buf2: nil
		*/
		CChainedBuf buf1;
		CChainedBuf buf2;
		
		buf1.transplant(buf2, -1);

		size_t buf1_len = 0;
		size_t buf2_len = 200;
		buf2.write(DATA, buf2_len);

		buf1.transplant(buf2, 0);
		EXPECT_EQ(0, buf1.size());

		buf1.transplant(buf2, 80);
		buf2_len -= 80;
		buf1_len += 80;
		EXPECT_EQ(buf1_len, buf1.size());
		EXPECT_EQ(buf2_len, buf2.size());

		NodePtr chain2_b = buf2.Beg();
		NodePtr chain2_lwd = buf2.LWD();
		NodePtr chain1_lwd = buf1.LWD();
		size_t trlen = buf2_len;
		buf1.transplant(buf2, -1);
		buf1_len += trlen;
		buf2_len = 0;
		EXPECT_EQ(buf1_len, buf1.size());
		EXPECT_EQ(buf2_len, buf2.size());
		EXPECT_EQ(chain2_b, chain1_lwd->next);
		EXPECT_EQ(chain2_lwd->next, buf1.End());

		int end = 0;
	}

	{
		/*
		CASE 2:
		buf1: nil
		buf2: nil
		*/
		CChainedBuf buf1;
		CChainedBuf buf2;
		
		buf1.transplant(buf2, -1);

		size_t buf1_len = 0;
		size_t buf2_len = 200;
		buf2.write(DATA, buf2_len);
		buf2.write(DATA, 300);
		buf2_len += 300;

		buf1.transplant(buf2, 0);
		EXPECT_EQ(0, buf1.size());

		buf1.transplant(buf2, 80);
		buf2_len -= 80;
		buf1_len += 80;
		EXPECT_EQ(buf1_len, buf1.size());
		EXPECT_EQ(buf2_len, buf2.size());

		NodePtr chain2_b = buf2.Beg();
		NodePtr chain2_lwd = buf2.LWD();
		NodePtr chain1_lwd = buf1.LWD();
		size_t trlen = buf2_len;
		buf1.transplant(buf2, -1);
		buf1_len += trlen;
		buf2_len = 0;
		EXPECT_EQ(buf1_len, buf1.size());
		EXPECT_EQ(buf2_len, buf2.size());
		EXPECT_EQ(chain2_b, chain1_lwd->next);
		EXPECT_EQ(chain2_lwd->next, buf1.End());

		int end = 0;
	}

	{
		/*
		CASE 2:
		buf1: nil
		buf2: nil
		*/
		CChainedBuf buf1;
		CChainedBuf buf2;
		
		buf1.transplant(buf2, -1);

		size_t buf1_len = 0;
		size_t buf2_len = 200;
		buf2.write(DATA, buf2_len);
		buf2.write(DATA, 300);
		buf2_len += 300;

		buf1.transplant(buf2, 0);
		EXPECT_EQ(0, buf1.size());

		buf1.transplant(buf2, 240);
		buf2_len -= 240;
		buf1_len += 240;
		EXPECT_EQ(buf1_len, buf1.size());
		EXPECT_EQ(buf2_len, buf2.size());

		NodePtr chain2_b = buf2.Beg();
		NodePtr chain2_lwd = buf2.LWD();
		NodePtr chain1_lwd = buf1.LWD();
		size_t trlen = buf2_len;
		buf1.transplant(buf2, -1);
		buf1_len += trlen;
		buf2_len = 0;
		EXPECT_EQ(buf1_len, buf1.size());
		EXPECT_EQ(buf2_len, buf2.size());
		EXPECT_EQ(chain2_b, chain1_lwd->next);
		EXPECT_EQ(chain2_lwd->next, buf1.End());

		int end = 0;
	}
}