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
#ifndef _MONO_TIME_H_
#define _MONO_TIME_H_

// config
//#define _MONO_TIME__HAVE_CLOCK_GETTIME
//#define _MONO_TIME__HAVE_MACH_ABSOLUTE_TIME

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////

#if !defined(int64_t)
	typedef __int64				int64_t;
#endif
# if !defined(uint64_t)
	typedef unsigned __int64	uint64_t;
#endif

/**
 * @name Manipulation macros for struct timeval.
 *
 * We define replacements
 * for timeradd, timersub, timerclear, timercmp, and timerisset.
 *
 * @{
 */
#ifdef _MT__HAVE_TIMERADD
#define mt_timeradd(tvp, uvp, vvp) timeradd((tvp), (uvp), (vvp))
#define mt_timersub(tvp, uvp, vvp) timersub((tvp), (uvp), (vvp))
#else
#define mt_timeradd(tvp, uvp, vvp)					\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec + (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec + (uvp)->tv_usec;       \
		if ((vvp)->tv_usec >= 1000000) {			\
			(vvp)->tv_sec++;				\
			(vvp)->tv_usec -= 1000000;			\
		}							\
	} while (0)
#define	mt_timersub(tvp, uvp, vvp)					\
	do {								\
		(vvp)->tv_sec = (tvp)->tv_sec - (uvp)->tv_sec;		\
		(vvp)->tv_usec = (tvp)->tv_usec - (uvp)->tv_usec;	\
		if ((vvp)->tv_usec < 0) {				\
			(vvp)->tv_sec--;				\
			(vvp)->tv_usec += 1000000;			\
		}							\
	} while (0)
#endif /* !_MT__HAVE_TIMERADD */

#ifdef _MT__HAVE_TIMERCLEAR
#define mt_timerclear(tvp) timerclear(tvp)
#else
#define	mt_timerclear(tvp)	(tvp)->tv_sec = (tvp)->tv_usec = 0
#endif
/**@}*/

/** Return true iff the tvp is related to uvp according to the relational
 * operator cmp.  Recognized values for cmp are ==, <=, <, >=, and >. */
#define	mt_timercmp(tvp, uvp, cmp)					\
	(((tvp)->tv_sec == (uvp)->tv_sec) ?				\
	 ((tvp)->tv_usec cmp (uvp)->tv_usec) :				\
	 ((tvp)->tv_sec cmp (uvp)->tv_sec))

#ifdef _MT__HAVE_TIMERISSET
#define mt_timerisset(tvp) timerisset(tvp)
#else
#define	mt_timerisset(tvp)	((tvp)->tv_sec || (tvp)->tv_usec)
#endif

////////////////////////////////////////////////////////////////////////////////
	struct mono_timer;
	enum {
		MT_MONOT_PRECISE  = 1,
		MT_MONOT_FALLBACK = 2,
	};

	struct mono_timer *mono_timer_new(void);
	void mono_timer_free(struct mono_timer *timer);
	int mono_timer_configure(struct mono_timer *timer, int flags);
	int mono_timer_gettime_monotonic(struct mono_timer *timer, struct timeval *tp);

	int mono_timer_gettimeofday(struct timeval *tv, struct timezone *tz);

	int64_t mono_timer_tv_to_msec(const struct timeval *tv);
	void mono_timer_usleep(const struct timeval *tv);
#ifdef __cplusplus
}
#endif

#endif // _MONO_TIME_H_
