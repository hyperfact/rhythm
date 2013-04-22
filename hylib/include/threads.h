/********************************************************************
	created:	2012/07/20
	created:	20:7:2012   14:27
	filename: 	threads.h
	file base:	threads
	file ext:	h
	author:		huaiyu
	
	purpose:	
*********************************************************************/

#ifndef _THREADS_H_
#define _THREADS_H_
////////////////////////////////////////////////////////////////////////////////

#pragma once
// Includes go here:
#include "_prolog.h"

_HYLIB_BEGIN
////////////////////////////////////////////////////////////////////////////////

struct SingleThreaded {
	// for typename VolatileType<T>::Type
	template <class _T>
		struct VolatileType {
		typedef _T Type;
	};

	// for typedef IntType:
	typedef int IntType;

	// for atomic arithmetic operation
	static IntType AtomicAdd(volatile IntType& lval, IntType val) {
		return lval += val; }
	static IntType AtomicSubtract(volatile IntType& lval, IntType val) {
		return lval -= val; }
	static IntType AtomicMultiply(volatile IntType& lval, IntType val) {
		return lval *= val; }
	static IntType AtomicDivide(volatile IntType& lval, IntType val) {
		return lval /= val; }
	static IntType AtomicIncrement(volatile IntType& lval) {
		return ++lval; }
	static IntType AtomicDecrement(volatile IntType& lval) {
		return --lval; }
	static void AtomicAssign(volatile IntType & lval, IntType val) {
		lval = val; }
	static void AtomicAssign(IntType & lval, volatile IntType & val) {
		lval = val; }
	static IntType AtomicCompareAssign(volatile IntType &lval, IntType val, IntType comp) {
		IntType initVal = lval;
		lval = (lval==comp) ? val : lval;
		return initVal;
	}
	
	// for struct Lock:
	struct Lock {
		Lock() {}
		~Lock() {}
		void lock() {}
		void unlock() {}
	};
};

struct MultiThreaded {
	// for typename VolatileType<T>::Type:
	template <class _T>
		struct VolatileType {
		typedef volatile _T Type;
	};

#ifdef _WIN32
	// for typedef IntType:
	typedef long IntType;
	
	// for atomic arithmetic operation
	static IntType AtomicAdd(volatile IntType& lval, IntType val) {
		return InterlockedExchangeAdd(&const_cast<IntType&>(lval), val); }
	static IntType AtomicSubtract(volatile IntType& lval, IntType val) {
		return InterlockedExchangeAdd(&const_cast<IntType&>(lval), -val); }
	static IntType AtomicIncrement(volatile IntType& lval) {
		return InterlockedIncrement(&const_cast<IntType&>(lval)); }
	static IntType AtomicDecrement(volatile IntType& lval) {
		return InterlockedDecrement(&const_cast<IntType&>(lval)); }
	static void AtomicAssign(volatile IntType& lval, IntType val) {
		InterlockedExchange(&const_cast<IntType&>(lval), val); }
	static void AtomicAssign(IntType& lval, volatile IntType& val) {
		InterlockedExchange(&lval, val); }
//	static IntType AtomicCompareAssign(volatile IntType &lval, IntType val, IntType comp) {
//		return InterlockedCompareExchange(&const_cast<IntType&>(lval), val, comp); }

	// for struct Lock:
	struct Lock {
		Lock() { ::InitializeCriticalSection(&_cs); }
		~Lock() { ::DeleteCriticalSection(&_cs); }
		void lock() { ::EnterCriticalSection(&_cs); }
		void unlock() { ::LeaveCriticalSection(&_cs); }
		
	private:
		CRITICAL_SECTION  _cs;
	};
#else // _WIN32
	typedef int IntType;
	struct Lock;
#	error 暂不支持非WIN32平台
#endif // _WIN32
};

template <class _Lock>
struct LockGuard {
	LockGuard(_Lock &l) : _l(l) { _l.lock(); }
	~LockGuard() { _l.unlock(); }
private:
	_Lock &_l;
};

////////////////////////////////////////////////////////////////////////////////
_HYLIB_END

#endif // _THREADS_H_