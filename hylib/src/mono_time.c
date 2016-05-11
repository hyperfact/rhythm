/*
 * Copyright (c) 2016 Stephen Huai <hyperfact@gmail.com>
 * seperate compile version based on Niels Provos and Nick Mathewson's
 *
 * Copyright (c) 2007-2012 Niels Provos and Nick Mathewson
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "mono_time.h"

////////////////////////////////////////////////////////////////////////////////
// config
#if defined(_MT__HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
#define HAVE_POSIX_MONOTONIC
#elif defined(_MT__HAVE_MACH_ABSOLUTE_TIME)
#define HAVE_MACH_MONOTONIC
#elif defined(_WIN32)
#define HAVE_WIN32_MONOTONIC
#else
#define HAVE_FALLBACK_MONOTONIC
#endif

////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32
#  include <winsock2.h>
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  undef WIN32_LEAN_AND_MEAN
#endif

#include <sys/types.h>
#  ifdef _MT__HAVE_STDLIB_H
#  include <stdlib.h>
#endif

#include <errno.h>
#include <limits.h>

#ifndef _MT__HAVE_GETTIMEOFDAY
#  include <sys/timeb.h>
#endif

#if !defined(_MT__HAVE_NANOSLEEP) && !defined(_MT_HAVE_USLEEP) && !defined(_WIN32)
#  include <sys/select.h>
#endif

#include <time.h>
#include <sys/stat.h>
#include <string.h>

#ifdef _WIN32
typedef ULONGLONG (WINAPI *mt_GetTickCount_func)(void);
#endif

/* Evaluates to the same boolean value as 'p', and hints to the compiler that
 * we expect this value to be false. */
#if defined(__GNUC__) && __GNUC__ >= 3         /* gcc 3.0 or later */
#define MT_UNLIKELY(p) __builtin_expect(!!(p),0)
#else
#define MT_UNLIKELY(p) (p)
#endif

static /*inline*/ int64_t abs64(int64_t i) {
 	return i < 0 ? -i : i;
}

struct mono_timer {

#ifdef HAVE_MACH_MONOTONIC
	struct mach_timebase_info mach_timebase_units;
#endif

#ifdef HAVE_POSIX_MONOTONIC
	int monotonic_clock;
#endif

#ifdef HAVE_WIN32_MONOTONIC
	HMODULE kernel_dll;
	mt_GetTickCount_func GetTickCount64_fn;
	mt_GetTickCount_func GetTickCount_fn;
	uint64_t last_tick_count;
	uint64_t adjust_tick_count;

	uint64_t first_tick;
	uint64_t first_counter;
	double usec_per_count;
	int use_performance_counter;
#endif

	struct timeval adjust_monotonic_clock;
	struct timeval last_time;
};

#define MAX_SECONDS_IN_MSEC_LONG  (((LONG_MAX) - 999) / 1000)

static int mt_configure_mono_timer_(struct mono_timer *base, int flags);
static int mt_gettime_monotonic_(struct mono_timer *base, struct timeval *tp);
static void mt_errx(int level, const char *fmt, ...) {}

#ifndef _MT__HAVE_GETTIMEOFDAY
/* No gettimeofday; this must be windows. */
int
mono_timer_gettimeofday(struct timeval *tv, struct timezone *tz)
{
#ifdef _MSC_VER
#define U64_LITERAL(n) n##ui64
#else
#define U64_LITERAL(n) n##llu
#endif

	/* Conversion logic taken from Tor, which in turn took it
	 * from Perl.  GetSystemTimeAsFileTime returns its value as
	 * an unaligned (!) 64-bit value containing the number of
	 * 100-nanosecond intervals since 1 January 1601 UTC. */
#define EPOCH_BIAS U64_LITERAL(116444736000000000)
#define UNITS_PER_SEC U64_LITERAL(10000000)
#define USEC_PER_SEC U64_LITERAL(1000000)
#define UNITS_PER_USEC U64_LITERAL(10)
	union {
		FILETIME ft_ft;
		uint64_t ft_64;
	} ft;

	if (tv == NULL)
		return -1;

	GetSystemTimeAsFileTime(&ft.ft_ft);

	if (MT_UNLIKELY(ft.ft_64 < EPOCH_BIAS)) {
		/* Time before the unix epoch. */
		return -1;
	}
	ft.ft_64 -= EPOCH_BIAS;
	tv->tv_sec = (long) (ft.ft_64 / UNITS_PER_SEC);
	tv->tv_usec = (long) ((ft.ft_64 / UNITS_PER_USEC) % USEC_PER_SEC);
	return 0;
}
#endif

int64_t mono_timer_tv_to_msec(const struct timeval *tv)
{
	if (tv->tv_usec > 1000000 || tv->tv_sec > MAX_SECONDS_IN_MSEC_LONG)
		return -1;

	return ((int64_t)tv->tv_sec * 1000) + (((int64_t)tv->tv_usec + 999) / 1000);
}

/*
  Replacement for usleep on platforms that don't have one.  Not guaranteed to
  be any more finegrained than 1 msec.
 */
void
mono_timer_usleep(const struct timeval *tv)
{
	if (!tv)
		return;
#if defined(_WIN32)
	{
		long msec = mono_timer_tv_to_msec(tv);
		Sleep((DWORD)msec);
	}
#elif defined(_MT__HAVE_NANOSLEEP)
	{
		struct timespec ts;
		ts.tv_sec = tv->tv_sec;
		ts.tv_nsec = tv->tv_usec*1000;
		nanosleep(&ts, NULL);
	}
#elif defined(_MT__HAVE_USLEEP)
	/* Some systems don't like to usleep more than 999999 usec */
	sleep(tv->tv_sec);
	usleep(tv->tv_usec);
#else
	select(0, NULL, NULL, NULL, tv);
#endif
}

/*
   This function assumes it's called repeatedly with a
   not-actually-so-monotonic time source whose outputs are in 'tv'. It
   implements a trivial ratcheting mechanism so that the values never go
   backwards.
 */
static void
adjust_monotonic_time(struct mono_timer *base,
    struct timeval *tv)
{
	mt_timeradd(tv, &base->adjust_monotonic_clock, tv);

	if (mt_timercmp(tv, &base->last_time, <)) {
		/* Guess it wasn't monotonic after all. */
		struct timeval adjust;
		mt_timersub(&base->last_time, tv, &adjust);
		mt_timeradd(&adjust, &base->adjust_monotonic_clock,
		    &base->adjust_monotonic_clock);
		*tv = base->last_time;
	}
	base->last_time = *tv;
}

/*
   Allocate a new struct mono_timer
 */
struct mono_timer *
mono_timer_new(void)
{
  struct mono_timer *p = NULL;

  p = malloc(sizeof(*p));
  if (!p) goto done;

  memset(p, 0, sizeof(*p));

 done:
  return p;
}

/*
   Free a struct mono_timer
 */
void
mono_timer_free(struct mono_timer *timer)
{
	if (timer) {
		if (timer->kernel_dll)
			FreeLibrary(timer->kernel_dll);
		free(timer);
	}
}

/*
   Set up a struct mono_timer for initial use
 */
int
mono_timer_configure(struct mono_timer *timer,
                                int flags)
{
  return mt_configure_mono_timer_(timer, flags);
}

/*
   Query the current monotonic time
 */
int
mono_timer_gettime_monotonic(struct mono_timer *timer,
                         struct timeval *tp)
{
  return mt_gettime_monotonic_(timer, tp);
}


#if defined(HAVE_POSIX_MONOTONIC)
/* =====
   The POSIX clock_gettime() interface provides a few ways to get at a
   monotonic clock.  CLOCK_MONOTONIC is most widely supported.  Linux also
   provides a CLOCK_MONOTONIC_COARSE with accuracy of about 1-4 msec.

   On all platforms I'm aware of, CLOCK_MONOTONIC really is monotonic.
   Platforms don't agree about whether it should jump on a sleep/resume.
 */

int
mt_configure_mono_timer_(struct mono_timer *base,
    int flags)
{
	/* CLOCK_MONOTONIC exists on FreeBSD, Linux, and Solaris.  You need to
	 * check for it at runtime, because some older kernel versions won't
	 * have it working. */
#ifdef CLOCK_MONOTONIC_COARSE
	const int precise = flags & MT_MONOT_PRECISE;
#endif
	const int fallback = flags & MT_MONOT_FALLBACK;
	struct timespec	ts;

#ifdef CLOCK_MONOTONIC_COARSE
	if (CLOCK_MONOTONIC_COARSE < 0) {
		/* Technically speaking, nothing keeps CLOCK_* from being
		 * negative (as far as I know). This check and the one below
		 * make sure that it's safe for us to use -1 as an "unset"
		 * value. */
		mt_errx(1,"I didn't expect CLOCK_MONOTONIC_COARSE to be < 0");
	}
	if (! precise && ! fallback) {
		if (clock_gettime(CLOCK_MONOTONIC_COARSE, &ts) == 0) {
			base->monotonic_clock = CLOCK_MONOTONIC_COARSE;
			return 0;
		}
	}
#endif
	if (!fallback && clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
		base->monotonic_clock = CLOCK_MONOTONIC;
		return 0;
	}

	if (CLOCK_MONOTONIC < 0) {
		mt_errx(1,"I didn't expect CLOCK_MONOTONIC to be < 0");
	}

	base->monotonic_clock = -1;
	return 0;
}

int
mt_gettime_monotonic_(struct mono_timer *base,
    struct timeval *tp)
{
	struct timespec ts;

	if (base->monotonic_clock < 0) {
		if (mono_timer_gettimeofday(tp, NULL) < 0)
			return -1;
		adjust_monotonic_time(base, tp);
		return 0;
	}

	if (clock_gettime(base->monotonic_clock, &ts) == -1)
		return -1;
	tp->tv_sec = ts.tv_sec;
	tp->tv_usec = ts.tv_nsec / 1000;

	return 0;
}
#endif

#if defined(HAVE_MACH_MONOTONIC)
/* ======
   Apple is a little late to the POSIX party.  And why not?  Instead of
   clock_gettime(), they provide mach_absolute_time().  Its units are not
   fixed; we need to use mach_timebase_info() to get the right functions to
   convert its units into nanoseconds.

   To all appearances, mach_absolute_time() seems to be honest-to-goodness
   monotonic.  Whether it stops during sleep or not is unspecified in
   principle, and dependent on CPU architecture in practice.
 */

int
mt_configure_mono_timer_(struct mono_timer *base,
    int flags)
{
	const int fallback = flags & MT_MONOT_FALLBACK;
	struct mach_timebase_info mi;
	memset(base, 0, sizeof(*base));
	/* OSX has mach_absolute_time() */
	if (!fallback &&
	    mach_timebase_info(&mi) == 0 &&
	    mach_absolute_time() != 0) {
		/* mach_timebase_info tells us how to convert
		 * mach_absolute_time() into nanoseconds, but we
		 * want to use microseconds instead. */
		mi.denom *= 1000;
		memcpy(&base->mach_timebase_units, &mi, sizeof(mi));
	} else {
		base->mach_timebase_units.numer = 0;
	}
	return 0;
}

int
mt_gettime_monotonic_(struct mono_timer *base,
    struct timeval *tp)
{
	uint64_t abstime, usec;
	if (base->mach_timebase_units.numer == 0) {
		if (mono_timer_gettimeofday(tp, NULL) < 0)
			return -1;
		adjust_monotonic_time(base, tp);
		return 0;
	}

	abstime = mach_absolute_time();
	usec = (abstime * base->mach_timebase_units.numer)
	    / (base->mach_timebase_units.denom);
	tp->tv_sec = usec / 1000000;
	tp->tv_usec = usec % 1000000;

	return 0;
}
#endif

#if defined(HAVE_WIN32_MONOTONIC)
/* =====
   Turn we now to Windows.  Want monontonic time on Windows?

   Windows has QueryPerformanceCounter(), which gives time most high-
   resolution time.  It's a pity it's not so monotonic in practice; it's
   also got some fun bugs, especially: with older Windowses, under
   virtualizations, with funny hardware, on multiprocessor systems, and so
   on.  PEP418 [1] has a nice roundup of the issues here.

   There's GetTickCount64() on Vista and later, which gives a number of 1-msec
   ticks since startup.  The accuracy here might be as bad as 10-20 msec, I
   hear.  There's an undocumented function (NtSetTimerResolution) that
   allegedly increases the accuracy. Good luck!

   There's also GetTickCount(), which is only 32 bits, but seems to be
   supported on pre-Vista versions of Windows.  Apparently, you can coax
   another 14 bits out of it, giving you 2231 years before rollover.

   The less said about timeGetTime() the better.

   "We don't care.  We don't have to.  We're the Phone Company."
            -- Lily Tomlin, SNL

   Our strategy, if precise timers are turned off, is to just use the best
   GetTickCount equivalent available.  If we've been asked for precise timing,
   then we mostly[2] assume that GetTickCount is monotonic, and correct
   GetPerformanceCounter to approximate it.

   [1] http://www.python.org/dev/peps/pep-0418
   [2] Of course, we feed the Windows stuff into adjust_monotonic_time()
       anyway, just in case it isn't.

 */
/*
    Parts of our logic in the win32 timer code here are closely based on
    BitTorrent's libUTP library.  That code is subject to the following
    license:

      Copyright (c) 2010 BitTorrent, Inc.

      Permission is hereby granted, free of charge, to any person obtaining a
      copy of this software and associated documentation files (the
      "Software"), to deal in the Software without restriction, including
      without limitation the rights to use, copy, modify, merge, publish,
      distribute, sublicense, and/or sell copies of the Software, and to
      permit persons to whom the Software is furnished to do so, subject to
      the following conditions:

      The above copyright notice and this permission notice shall be included
      in all copies or substantial portions of the Software.

      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
      OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
      MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
      NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
      LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
      OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
      WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

static uint64_t
mt_GetTickCount_(struct mono_timer *base)
{
	if (base->GetTickCount64_fn) {
		/* Let's just use GetTickCount64 if we can. */
		return base->GetTickCount64_fn();
	} else if (base->GetTickCount_fn) {
		/* Greg Hazel assures me that this works, that BitTorrent has
		 * done it for years, and this it won't turn around and
		 * bite us.  He says they found it on some game programmers'
		 * forum some time around 2007.
		 */
		uint64_t v = base->GetTickCount_fn();
		return (DWORD)v | ((v >> 18) & 0xFFFFFFFF00000000);
	} else {
		/* Here's the fallback implementation. We have to use
		 * GetTickCount() with its given signature, so we only get
		 * 32 bits worth of milliseconds, which will roll ove every
		 * 49 days or so.  */
		DWORD ticks = GetTickCount();
		if (ticks < base->last_tick_count) {
			base->adjust_tick_count += ((uint64_t)1) << 32;
		}
		base->last_tick_count = ticks;
		return ticks + base->adjust_tick_count;
	}
}

int
mt_configure_mono_timer_(struct mono_timer *base,
    int flags)
{
	const int precise = flags & MT_MONOT_PRECISE;
	const int fallback = flags & MT_MONOT_FALLBACK;
	memset(base, 0, sizeof(*base));

	base->kernel_dll = LoadLibrary(TEXT("kernel32.dll"));
	if (base->kernel_dll != NULL && !fallback) {
		base->GetTickCount64_fn = (mt_GetTickCount_func)GetProcAddress(base->kernel_dll, "GetTickCount64");
		base->GetTickCount_fn = (mt_GetTickCount_func)GetProcAddress(base->kernel_dll, "GetTickCount");
	}

	base->first_tick = base->last_tick_count = mt_GetTickCount_(base);
	if (precise && !fallback) {
		LARGE_INTEGER freq;
		if (QueryPerformanceFrequency(&freq)) {
			LARGE_INTEGER counter;
			QueryPerformanceCounter(&counter);
			base->first_counter = counter.QuadPart;
			base->usec_per_count = 1.0e6 / freq.QuadPart;
			base->use_performance_counter = 1;
		}
	}

	return 0;
}


int
mt_gettime_monotonic_(struct mono_timer *base,
    struct timeval *tp)
{
	uint64_t ticks = mt_GetTickCount_(base);
	if (base->use_performance_counter) {
		/* Here's a trick we took from BitTorrent's libutp, at Greg
		 * Hazel's recommendation.  We use QueryPerformanceCounter for
		 * our high-resolution timer, but use GetTickCount*() to keep
		 * it sane, and adjust_monotonic_time() to keep it monotonic.
		 */
		LARGE_INTEGER counter;
		int64_t counter_elapsed, counter_usec_elapsed, ticks_elapsed;
		QueryPerformanceCounter(&counter);
		counter_elapsed = (int64_t)
		    (counter.QuadPart - base->first_counter);
		ticks_elapsed = ticks - base->first_tick;
		/* TODO: This may upset VC6. If you need this to work with
		 * VC6, please supply an appropriate patch. */
		counter_usec_elapsed = (int64_t)
		    (counter_elapsed * base->usec_per_count);

		if (abs64(ticks_elapsed*1000 - counter_usec_elapsed) > 1000000) {
			/* It appears that the QueryPerformanceCounter()
			 * result is more than 1 second away from
			 * GetTickCount() result. Let's adjust it to be as
			 * accurate as we can; adjust_monotnonic_time() below
			 * will keep it monotonic. */
			counter_usec_elapsed = ticks_elapsed * 1000;
			base->first_counter = (uint64_t) (counter.QuadPart - counter_usec_elapsed / base->usec_per_count);
		}
		tp->tv_sec = (time_t) (counter_usec_elapsed / 1000000);
		tp->tv_usec = counter_usec_elapsed % 1000000;

	} else {
		/* We're just using GetTickCount(). */
		tp->tv_sec = (time_t) (ticks / 1000);
		tp->tv_usec = (ticks % 1000) * 1000;
	}
	adjust_monotonic_time(base, tp);

	return 0;
}
#endif

#if defined(HAVE_FALLBACK_MONOTONIC)
/* =====
   And if none of the other options work, let's just use gettimeofday(), and
   ratchet it forward so that it acts like a monotonic timer, whether it
   wants to or not.
 */

int
mt_configure_mono_timer_(struct mono_timer *base,
    int flags)
{
	memset(base, 0, sizeof(*base));
	return 0;
}

int
mt_gettime_monotonic_(struct mono_timer *base,
    struct timeval *tp)
{
	if (mono_timer_gettimeofday(tp, NULL) < 0)
		return -1;
	adjust_monotonic_time(base, tp);
	return 0;

}
#endif
