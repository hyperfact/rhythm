#include "stdafx.h"

#ifdef _WIN32
#include "windows.h"
#endif
#include "../include/chained_buf.h"
#include "../Include/util_tmpl.h"
#include "../Include/random.h"

#ifndef ASSERT
#define ASSERT assert
#endif

#ifndef byte_t
typedef unsigned char byte_t;
#endif

#ifndef ArrLen
# define ArrLen(a) (sizeof(a)/sizeof((a)[0]))
#endif

#define RANDOM (hy::random::inst())

static const DWORD crc32_tab[] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de,	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,	0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940,	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,	0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

DWORD
crc32(const void *buf, size_t size)
{
	const BYTE *p;
	DWORD crc = (DWORD)~0U;

	p = (const BYTE *)buf;
	crc = crc ^ ~0U;

	while (size--)
		crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);

	return crc ^ ~0U;
}
 
using namespace ::hy;
class CChainedBufTestWrapper : public chained_buf<>
{
public:
	typedef _NodePtr NodePtr;
	typedef _Node Node;
	void Insert(_NodePtr cur, _NodePtr chain) {
		this->_Insert(cur, chain);
	}
	_NodePtr AllocChain(size_t expect_len) {
		return _AllocChain(expect_len);
	}
	void _FreeChain(_NodePtr chain) {
		_FreeChain(chain);
	}
	bool IsAnyChainLocked(unsigned lflag) const {
		_NodeCPtr chain = _Beg();
		for (; chain!=_End(); chain=chain->next) {
			if (chain->flags & lflag)
				return true;
		}
		return false;
	}
	bool IsAllChainsLocked(unsigned lflag) const {
		_NodeCPtr chain = _Beg();
		for (; chain!=_End(); chain=chain->next) {
			if (0 == (chain->flags & lflag))
				return false;
		}
		return true;
	}
	NodePtr &Beg() { return this->_Beg(); }
	NodePtr &Last() { return this->_Last(); }
	NodePtr End() { return this->_End(); }
	NodePtr Head() { return &this->_head; }
	NodePtr &LWD() { return this->_last_with_data; }
};

typedef CChainedBufTestWrapper CChainedBuf;
typedef CChainedBuf::Node Node;
typedef CChainedBuf::NodePtr NodePtr;
typedef CChainedBuf::iovec IOVec;
const size_t MIN_EXPAND_SIZE = CChainedBuf::MIN_ALLOC_SIZE-sizeof(CChainedBuf::_buf_chain);
byte_t _DATA[1024] = {0};
const byte_t (& const DATA)[1024] = _DATA;

struct _Static_init {
	_Static_init() {
		byte_t d = 0;
		for (int i=0; i<ArrLen(DATA); ++i)
			_DATA[i] = d++;
	}
} _static_init;

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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));

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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));

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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

		rllen = buf.size()+10;
		p = buf.rlock(rllen);
		EXPECT_EQ(plwd->pbegin(), p);
		EXPECT_EQ(wtlen, rllen);
		EXPECT_TRUE(buf.runlock(p, 0));
		EXPECT_EQ(wtlen, buf.size());
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

		rllen = buf.size()-10;
		p = buf.rlock(rllen);
		EXPECT_EQ(plwd->pbegin(), p);
		EXPECT_EQ(wtlen-10, rllen);
		EXPECT_TRUE(buf.runlock(p, 0));
		EXPECT_EQ(wtlen, buf.size());
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

		rllen = buf.size();
		p = buf.rlock(rllen);
		EXPECT_EQ(plwd->pbegin(), p);
		EXPECT_EQ(wtlen, rllen);
		EXPECT_TRUE(buf.runlock(p, 10));
		wtlen -= 10;
		EXPECT_EQ(wtlen, buf.size());
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

		rllen = buf.size();
		p = buf.rlock(rllen);
		EXPECT_EQ(plwd->pbegin(), p);
		EXPECT_EQ(wtlen, rllen);
		EXPECT_TRUE(buf.runlock(p, wtlen-10));
		wtlen = 10;
		EXPECT_EQ(wtlen, buf.size());
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

		rllen = buf.size();
		p = buf.rlock(rllen);
		EXPECT_EQ(plwd->pbegin(), p);
		EXPECT_EQ(wtlen, rllen);
		EXPECT_FALSE(buf.runlock(p, wtlen+10));
		EXPECT_TRUE(buf.runlock(p, wtlen));
		wtlen = 0;
		EXPECT_EQ(wtlen, buf.size());
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

		rllen = wtlen1-10;
		p = buf.rlock(rllen);
		EXPECT_EQ(chain1->pbegin(), p);
		EXPECT_EQ(wtlen1-10, rllen);
		EXPECT_TRUE(buf.runlock(p, rllen));
		wtlen1 -= rllen;
		sumwtlen -= rllen;
		EXPECT_EQ(sumwtlen, buf.size());
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
		wtlen += 80;
		EXPECT_EQ(wtlen, buf.size());

		wlen = 100;
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		iov[0].len = wlen;
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
		wtlen += wlen;
		EXPECT_EQ(wtlen, buf.size());

		wlen = 10;
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.wunlock(nullptr, 0));
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
		wtlen += 0;
		EXPECT_EQ(wtlen, buf.size());

		wlen = buf.Beg()->avail();
		EXPECT_EQ(1, buf.wlock(wlen, nullptr, 0));
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		iov[0].len = 0;
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
		wtlen += iov[0].len;
		EXPECT_EQ(wtlen, buf.size());

		wlen = buf.Beg()->avail()+20;
		EXPECT_EQ(2, buf.wlock(wlen, nullptr, 0));
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(2, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		iov[1].len = 0;
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
		wtlen += sum_iovec(iov, nvecs);
		EXPECT_EQ(wtlen, buf.size());

		wlen = 120;
		EXPECT_EQ(1, buf.wlock(wlen, nullptr, 0));
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
		wtlen += sum_iovec(iov, nvecs);
		EXPECT_EQ(wtlen, buf.size());

		wlen = buf.LWD()->avail()+100;
		EXPECT_EQ(2, buf.wlock(wlen, nullptr, 0));
		EXPECT_EQ(0, buf.wlock(wlen, iov, 0));
		EXPECT_TRUE(buf.wunlock(iov, 0));
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(2, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		iov[0].len = 0;
		iov[1].len = 0;
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
		wtlen += sum_iovec(iov, nvecs);
		EXPECT_EQ(wtlen, buf.size());

		wlen = buf.LWD()->avail()+100;
		EXPECT_EQ(2, buf.wlock(wlen, nullptr, 0));
		nvecs = buf.wlock(wlen, iov, 1);
		EXPECT_EQ(1, nvecs);
		EXPECT_GE(wlen, sum_iovec(iov, nvecs));
		iov[0].len = 0;
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
		EXPECT_EQ(0, buf.size());

		buf.LWD()->offset = 128;

		wlen = 20;
		EXPECT_EQ(1, buf.wlock(wlen, nullptr, 0));
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
		wtlen += sum_iovec(iov, nvecs);
		EXPECT_EQ(wtlen, buf.size());

		wlen = buf.LWD()->avail()-10;
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		iov[0].len = 0;
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
		EXPECT_EQ(0, buf.size());

		buf.LWD()->offset = 128;

		wlen = 20;
		EXPECT_EQ(1, buf.wlock(wlen, nullptr, 0));
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
		wtlen += sum_iovec(iov, nvecs);
		EXPECT_EQ(wtlen, buf.size());

		wlen = buf.LWD()->avail();
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
		EXPECT_EQ(0, buf.size());

		buf.LWD()->offset = 128;

		wlen = 20;
		EXPECT_EQ(1, buf.wlock(wlen, nullptr, 0));
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
		wtlen += sum_iovec(iov, nvecs);
		EXPECT_EQ(wtlen, buf.size());

		wlen = buf.LWD()->avail()+80;
		nvecs = buf.wlock(wlen, iov, 10);
		EXPECT_EQ(2, nvecs);
		EXPECT_EQ(wlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.wunlock(iov, nvecs));
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_WLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

		rlen = wtlen+10;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(wtlen, rlen);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(wtlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(0));
		wtlen -= 0;
		EXPECT_EQ(wtlen, buf.size());
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

		rlen = 40;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(20));
		wtlen -= 20;
		EXPECT_EQ(wtlen, buf.size());
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

		rlen = 40;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_FALSE(buf.runlock(buf.size()+10));
		EXPECT_TRUE(buf.runlock(rlen));
		wtlen -= rlen;
		EXPECT_EQ(wtlen, buf.size());
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

		rlen = wtlen;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(rlen));
		wtlen -= rlen;
		EXPECT_EQ(wtlen, buf.size());
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));
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
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));
		
		rlen = wtlen;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(2, nvecs);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(0));
		wtlen -= 0;
		EXPECT_EQ(wtlen, buf.size());
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

		rlen = wtlen+10;
		nvecs = buf.rlock(rlen, iov, 1);
		EXPECT_EQ(1, nvecs);
		EXPECT_LT(rlen, wtlen);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(0));
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

		rlen = wtlen+10;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(wtlen, rlen);
		EXPECT_EQ(2, nvecs);
		EXPECT_EQ(wtlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(0));
		wtlen -= 0;
		EXPECT_EQ(wtlen, buf.size());
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

		rlen = 40;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(20));
		wtlen -= 20;
		EXPECT_EQ(wtlen, buf.size());
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

		rlen = 40;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(1, nvecs);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_FALSE(buf.runlock(buf.size()+10));
		EXPECT_TRUE(buf.runlock(rlen));
		wtlen -= rlen;
		EXPECT_EQ(wtlen, buf.size());
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

		rlen = wtlen-20;
		nvecs = buf.rlock(rlen, iov, 1);
		EXPECT_EQ(1, nvecs);
		EXPECT_LT(rlen, wtlen);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(0));
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

		rlen = wtlen-20;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(2, nvecs);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(40));
		wtlen -= 40;
		EXPECT_EQ(wtlen, buf.size());
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));

		rlen = wtlen;
		nvecs = buf.rlock(rlen, iov, 10);
		EXPECT_EQ(2, nvecs);
		EXPECT_EQ(rlen, sum_iovec(iov, nvecs));
		EXPECT_TRUE(buf.runlock(rlen));
		wtlen -= rlen;
		EXPECT_EQ(wtlen, buf.size());
		EXPECT_FALSE(buf.IsAnyChainLocked(Node::F_RLOCK));
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

void fillByteSeqData(unsigned id, void *data, int len) {
	assert(len >= 3*sizeof(unsigned));
	void * const odata = data;
	int olen = len;
	*((unsigned *&)data)++ = len;
	len -= sizeof(unsigned);
	*((unsigned *&)data)++ = id;
	len -= sizeof(unsigned);
	unsigned &chksum = *((unsigned *&)data)++ = 0;
	len -= sizeof(unsigned);

	RANDOM.nextBytes(data, len);
	chksum = crc32(odata, olen);
}

bool VerifySeqData(unsigned id, void *data, int len) {
	assert(len >= 3*sizeof(unsigned));
	void *odata = data;
	int olen = len;

	unsigned reallen = *((unsigned *&)data)++;
	len -= sizeof(unsigned);
	if (reallen != olen)
		return false;
	unsigned realid = *((unsigned *&)data)++;
	len -= sizeof(unsigned);
	if (realid != id)
		return false;
	unsigned &chksum = *((unsigned *&)data)++;
	unsigned chksum0 = chksum;
	chksum = 0;
	len -= sizeof(unsigned);
	
	unsigned cc = crc32(odata, olen);
	if (cc != chksum0)
		return false;

	return true;
}

TEST(ChainedBuf, Bugfix2016022501) {
	CChainedBuf buf;
	IOVec iov[2];
	byte_t buffer[8192];
	unsigned wtpacklen = 0;
	unsigned rdpacklen = 0;

	size_t wlen = 12288;
	::memset(iov, 0, sizeof(iov));
	int n = buf.wlock(wlen, iov, ArrLen(iov));
	EXPECT_EQ(1, n);
	wtpacklen = 3736;
	fillByteSeqData(1, iov[0].buf, wtpacklen);
	buf.wunlock(wtpacklen);
	::memset(buffer, 0, sizeof(buffer));
	
	EXPECT_EQ(4, buf.peek(&rdpacklen, sizeof(rdpacklen)));
	EXPECT_EQ(wtpacklen, rdpacklen);
	EXPECT_EQ(rdpacklen, buf.read(buffer, rdpacklen));
	EXPECT_TRUE(VerifySeqData(1, buffer, rdpacklen));

	wlen = 12288;
	::memset(iov, 0, sizeof(iov));
	n = buf.wlock(wlen, iov, ArrLen(iov));
	EXPECT_EQ(1, n);
	EXPECT_EQ(12288, iov[0].len);
	byte_t *pbuf = (byte_t *)iov[0].buf;
	size_t buflen = iov[0].len;
	
	size_t wtpacklen1 = 3699;
	fillByteSeqData(1, pbuf, wtpacklen1);
	pbuf += wtpacklen1;
	buflen -= wtpacklen1;
	size_t wtpacklen2 = 5789;
	fillByteSeqData(2, pbuf, wtpacklen2);
	pbuf += wtpacklen2;
	buflen -= wtpacklen2;
	byte_t *data3 = new byte_t[8192];
	size_t wtpacklen3 = 4857;
	fillByteSeqData(3, data3, wtpacklen3);
	size_t remain3 = wtpacklen3 - buflen;
	memcpy_s(pbuf, buflen, data3, buflen);

	buf.wunlock(iov[0].len);

	::memset(buffer, 0, sizeof(buffer));
	EXPECT_EQ(4, buf.peek(&rdpacklen, sizeof(rdpacklen)));
	EXPECT_EQ(wtpacklen1, rdpacklen);
	EXPECT_EQ(rdpacklen, buf.read(buffer, rdpacklen));
	EXPECT_TRUE(VerifySeqData(1, buffer, rdpacklen));

	::memset(buffer, 0, sizeof(buffer));
	EXPECT_EQ(4, buf.peek(&rdpacklen, sizeof(rdpacklen)));
	EXPECT_EQ(wtpacklen2, rdpacklen);
	EXPECT_EQ(rdpacklen, buf.read(buffer, rdpacklen));
	EXPECT_TRUE(VerifySeqData(2, buffer, rdpacklen));

	delete []data3;
	int end = 0;
}