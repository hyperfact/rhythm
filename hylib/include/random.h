// random.h: interface for the random class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RANDOM_H__076A1912_0065_4545_B51C_A2B4DB062F21__INCLUDED_)
#define AFX_RANDOM_H__076A1912_0065_4545_B51C_A2B4DB062F21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "_prolog.h"
#include "stl_ext.h"
#include <time.h>

////////////////////////////////////////////////////////////////////////////////
// Mersenne Twister (Refers to MT19937 by Takuji Nishimura & Makoto Matsumoto)
// see: http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
void init_genrand(unsigned long s);
void init_by_array(unsigned long init_key[], int key_length);
unsigned long genrand_int32(void);

////////////////////////////////////////////////////////////////////////////////
_HYLIB_BEGIN

////////////////////////////////////////////////////////////////////////////////
class random : public noncopyable
{
//******************************************************************************
public:
	static random &inst() {
		static random _inst;
		return _inst;
	}
	
//******************************************************************************
public:
	int	nextInt() {
		return (int)next(32);
	}
	int nextInt(int nMin, int nMax) {
		assert(nMin < nMax);
		if (nMin == nMax) { return nMin; }
		return nMin + (int)nextUint((unsigned int)(nMax-nMin));
	}

	// an uniform distribution [0,nUpper).
	// refer to loki(ver0.1.7), Andrei Alexandrescu
	unsigned int nextUint(unsigned int nUpper)
    {
		if ((nUpper & (unsigned int)(-(int)nUpper)) == nUpper) {
			// n is pow of 2
			return (unsigned)(
				((uint64_t)next(32) * (uint64_t)nUpper) >> 32);
		}

        const unsigned int bucket_size = ULONG_MAX / nUpper;
        unsigned int a;
        do {
            a = next(32) / bucket_size;
        } while (a >= nUpper);
        return a;
    }

	char nextChar() {
		return (char)(next(8));
	}
	short nextShort() {
		return (short)(next(16));
	}
	int64_t nextInt64() {
		return (int64_t)(
			(((uint64_t)next(32))<<32) | ((uint64_t)next(32))
			);
	}

	bool nextBool() {
		return (0!=next(1)) ? true : false;
	}
	bool nextBool(float fProbability) {
		if (F_LTE(fProbability, 0.0f)) { return false; }
		if (F_GTE(fProbability, 1.0f)) { return true; }
		return F_LT(nextFloat(), fProbability);
	}

	// [ret]-[0.0f,1.0f)
	float nextFloat() {
		float f = float(next(FLT_MANT_DIG)) / float(1U << FLT_MANT_DIG);
		return (f<1.0f) ? f : 1.0f-FLT_EPSILON;
	}
	// [ret]-[0.0f,1.0f)
	double nextDouble() {
		double d = double(int64_t(unsigned(nextInt()))
						| (int64_t(unsigned(next(DBL_MANT_DIG-32)))<<32))
				/ double(((int64_t)1U) << DBL_MANT_DIG);
		return (d<1.0) ? d : 1.0-DBL_EPSILON;
	}

	int mutexHit(const float *pProbs, int nCount) {
		assert(F_GTE(1.0f, std::accumulate(pProbs, pProbs+nCount, 0.0f)));
		float fRand = nextFloat();
		float nLower = 0.0f;
		float nUpper = 0.0f;
		for (int i=0; i<nCount; ++i) {
			nUpper += pProbs[i];
			if (F_BTW_LE(fRand, nLower, nUpper)) { return i; }
			nLower = nUpper;
		}
		return -1;
	}

	template <class _II, class _Type>
	int mutexHit(_II iProbFirst, _II iProbEnd, _Type total) {
		assert(total >= std::accumulate(iProbFirst, iProbEnd, _Type()));
		float fRand = nextFloat();
		float nLower = 0.0f;
		float nUpper = 0.0f;
		const _II iProbBegin = iProbFirst;
		for ( ; iProbFirst!=iProbEnd; ++iProbFirst) {
			nUpper += ((float)(*iProbFirst)/(float)total);
			if (F_BTW_LE(fRand, nLower, nUpper)) { return std::distance(iProbBegin,iProbFirst); }
			nLower = nUpper;
		}
		return -1;
	}

//******************************************************************************
protected:
	random() {
		::init_genrand(::time(NULL));
	}

	unsigned next(unsigned nBits) {
		assert(nBits <= 32);
		return ::genrand_int32() >> (32-nBits);
	}
};

////////////////////////////////////////////////////////////////////////////////
_HYLIB_END

#endif // !defined(AFX_RANDOM_H__076A1912_0065_4545_B51C_A2B4DB062F21__INCLUDED_)
