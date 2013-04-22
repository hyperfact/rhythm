/********************************************************************
	created:	2011/09/26
	created:	26:9:2011   11:46
	filename: 	log.h
	file base:	log
	file ext:	h
	author:		huaiyu
	
	purpose:	日志工具:提供类line_logger、工具宏(_LINE_LOG_HELPER)、实用宏HYLOGInfo、
				HYLOGWarn、HYLOGErr。
				工具宏_LINE_LOG_HELPER用法:_LINE_LOG_HELPER(输出流型别,输出流对象,Log级别);
				实用宏HYLOGInfo(普通信息)、HYLOGWarn(警告信息)、HYLOGErr(错误信息)
				用法(以HYLOGInfo为例):HYLOGInfo << "这是一条LOG信息"。
*********************************************************************/

#ifndef _LOG_H_
#define _LOG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
////////////////////////////////////////////////////////////////////////////////
// Includes:
#include "_prolog.h"
#include "macro.h"
#include "compare.h"
#include "util_tmpl.h"
#include <vector>
#include <iosfwd>
#include <fstream>
#include <strstream>
#include <sstream>
#include <time.h>
#include <io.h>
#include "logview.h"
#include "threads.h"

////////////////////////////////////////////////////////////////////////////////
_HYLIB_BEGIN

//******************************************************************************
typedef unsigned log_handle;

//******************************************************************************
// Prototypes:
template <class _Os>
	struct scope_logger;
template <class _Os>
	struct line_logger;
struct CDebugStream;
struct logger_group;

// End prototypes.

_HYLIB_END

#include "detail/_log_detail.h"

////////////////////////////////////////////////////////////////////////////////
_HYLIB_BEGIN
_HYLIB_DETAIL_USING

//******************************************************************************
// 线程锁
template <class _Os>
inline void log_thrd_lock(_Os &os) {}
inline void log_thrd_lock(logger_group &os);

template <class _Os>
inline void log_thrd_unlock(_Os &os) {}
inline void log_thrd_unlock(logger_group &os);

template <class _Os>
struct _log_thrd_unlock_guard {
	_log_thrd_unlock_guard(_Os &_os) : os(_os) {}
	~_log_thrd_unlock_guard() { log_thrd_unlock(os); }
	_Os &os;
};

//******************************************************************************
enum LogShowFlag{
	LSF_NONE		= 0,

	LSF_FILE		= 1<<1,		// 显示文件名
	LSF_FUNCTION	= 1<<2,		// 显示函数名
	LSF_LINE		= 1<<3,		// 显示行号
	LSF_TIME		= 1<<4,		// 显示时间戳
	
	LSF_ALL			= LSF_FILE|LSF_FUNCTION|LSF_LINE|LSF_TIME,
};
typedef unsigned log_show_flag;

template <class _Os>
inline bool log_is_show(_Os &os, LogShowFlag flag) { return true; }
inline bool log_is_show(logger_group &os, LogShowFlag flag);

//******************************************************************************
enum LogLevel {
	LL_ALL		= 0,
	LL_TRACE	= 100,
	LL_DEBUG	= 200,
	LL_INFO		= 300,
	LL_WARN		= 400,
	LL_ERROR	= 500,
	LL_FATAL	= 600,
	LL_NONE		= UINT_MAX,
};
enum { LEVEL_DIVISOR = 100 };
static const char *LOG_LEVEL_NAME[] = {
	"ALL", "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "NONE"
};
typedef unsigned log_level;

//******************************************************************************
template <class _Os>
_Os &log_begin_rec(_Os &os, log_level nLogLvl, const char *pszFile="",
		const char *pszFunction="", int nLine=0);
// logger_group &log_begin_rec(logger_group &os, log_level nLogLvl, const char *pszFile/*=""*/,
// 		const char *pszFunction/*=""*/, int nLine/*=0*/);

template <class _Os>
_Os &log_end_rec(_Os &os);
logger_group &log_end_rec(logger_group &os, log_handle hdl);

template <class _Os>
inline _Os &log_set_cur_level(_Os &os, log_level level) { return os; }
inline logger_group &log_set_cur_level(logger_group &os, log_level level);

template <class _Os>
inline _Os &log_get_level(_Os &os, log_level &level) { level = LL_NONE; return os; }
inline logger_group &log_get_level(logger_group &os, log_level &level);

//******************************************************************************
template <class _Os>
inline _Os &_log_head(_Os &os,  log_level nLogLvl, const char *pszFile="",
				const char *pszFunction="", int nLine=0)
{
	unsigned nLvlInx = nLogLvl/LEVEL_DIVISOR;
	nLogLvl = __min(nLvlInx, ARR_LEN(LOG_LEVEL_NAME)-1);
	os << "[" << LOG_LEVEL_NAME[nLogLvl] << "-";
	
	if (log_is_show(os, LSF_FILE)) { os << " File:" << pszFile; }
	if (log_is_show(os, LSF_FUNCTION)) { os << " Func:" << pszFunction; }
	if (log_is_show(os, LSF_LINE)) { os << " Line:" << nLine; }
	if (log_is_show(os, LSF_TIME)) { os << " Time:" << _log_cur_time(); }
	
	os << "]\n\t";
	return os;
}

//******************************************************************************
// template function log_begin_rec
template <class _Os>
inline _Os &log_begin_rec( _Os &os,
				log_level nLogLvl,
				const char *pszFile/*=""*/,
				const char *pszFunction/*=""*/,
				int nLine/*=0*/ )
{
	log_thrd_lock(os);

	log_set_cur_level(os, nLogLvl);
	log_level lvl = LL_NONE;
	log_get_level(os, lvl);
	if (nLogLvl < lvl) {
		return os; }

	_log_head(os, nLogLvl, pszFile, pszFunction, nLine);
	return os;
}

//******************************************************************************
// template function log_end_rec
// 结束一条日志
template <class _Os>
_Os &log_end_rec(_Os &os)
{
	_log_thrd_unlock_guard<_Os> ltug(os);

	os << _T("\n");
	return os;
}

#ifdef _WIN32
	inline CDebugStream &log_end_rec(CDebugStream &os);
#endif // _WIN32

#ifdef _USING_MFC
	CLogView &log_end_rec(CLogView &os);
#endif // _USING_MFC

logger_group &log_end_rec(logger_group &os);

#ifdef _WIN32
//******************************************************************************
// struct CDebugStream: Windows调试输出流
struct CDebugStream : public std::ostrstream, public noncopyable {
	static CDebugStream &inst() {
		static CDebugStream _inst;
		return _inst;
	}
protected:
	CDebugStream() {}
};

inline CDebugStream &log_end_rec(CDebugStream &os)
{
	_log_thrd_unlock_guard<CDebugStream> ltug(os);

	os << std::endl << std::ends;
	::OutputDebugString(os.str());
	os.freeze(false);
	os.seekp(0);

	return os;
}
#endif // _WIN32

//******************************************************************************
// struct logger_group: 日志组(能够同时向多个流输出日志)
struct logger_group {
	static logger_group &inst() {
		static logger_group _inst;
		return _inst;
	} 
	
	logger_group()
		: _show_flags(LSF_NONE)
		, _level(LL_INFO)
		, _cur_level(LL_NONE)
	{
		_logs.push_back(log_entity_empty::inst());
		_logs.push_back(&__le_stdout);
		_logs.push_back(&__le_stderr);
		_logs.push_back(&__le_stdlog);
		_logs.push_back(log_entity_empty::inst()); // log_entity_debugger::inst()
		_logs.push_back(log_entity_empty::inst()); // log_entity_log_view::inst()
	}

	~logger_group();

public:
	enum {
		NONE		= 0,
		STDOUT		= 1,
		STDERR		= 2,
		STDLOG		= 3,
		DEBUGGER	= 4,
		LOG_VIEW	= 5,
		HANDLE		= 6,
	};

protected:
	static log_entity_std __le_stdout;
	static log_entity_std __le_stderr;
	static log_entity_std __le_stdlog;

public:
	logger_group &add_file(const char *szFile, int nMaxLen=0,
		log_handle *nHandle=NULL);

	logger_group &add_handle(log_entity *ent, log_handle *nHandle=NULL) {
		ASSERT(ent);
		ent->enable(true);
		_logs.push_back(ent);
		if (nHandle) { *nHandle = (log_handle)(_logs.size()-1); }
		return *this;
	}

public:
	void _turn(log_handle hdl, bool bOn=true) {
		ASSERT_RET_VOID(BTW_LE(hdl, 0, _logs.size()));
		_logs[hdl]->enable(bOn);
	}

	log_entity *_set_handle(log_handle hdl, log_entity *ent) {
		ASSERT_RET(BTW_LE(hdl, 0, _logs.size()), NULL);
		if (NULL == ent) { ent = log_entity_empty::inst(); }
		log_entity *old = _logs[hdl];
		_logs[hdl] = ent;
		return old;
	}

	void _show(log_show_flag flag, bool bShow=true) {
		bShow ? BT_SET(_show_flags, flag) : BT_CLEAR(_show_flags, flag);
	}

	bool _is_show(LogShowFlag flag) const {
		return BT_IS_SET(_show_flags, flag);
	}

	void _set_level(log_level level) {
		_level = level;
	}

	log_level _get_level() const {
		return _level;
	}

	void _set_cur_level(log_level level) {
		_cur_level = level;
	}

	void _default_config() {
		_turn(STDOUT, true);
		_show(LSF_FILE|LSF_LINE, true);
		_set_level(LL_INFO);
	}

	void _thrd_lock() {
		_lock.lock();
	}

	void _thrd_unlock() {
		_lock.unlock();
	}

protected:
	typedef std::vector<log_entity *> _log_vsl;
	typedef _log_vsl::iterator _log_iter;

	struct _log_rec {
		bool			end;
		log_level		cur_level;
		std::ostringstream oss;

		_log_rec() : end(false), cur_level(LL_NONE) {}
	};
	typedef std::list<_log_rec> _log_rec_que;
	typedef _log_rec_que::iterator _log_rec_iter;
	typedef _log_rec_iter _log_rec_handle;

	_log_rec_handle _push_rec() {
		_recs.push_back(_log_rec());
		return _recs.end();
	}
	void _pop_rec(_log_rec_handle hdl) {
		while (hdl!=_recs.end() && !hdl->end) {
			_end_rec(hdl);
			hdl = _recs.erase(hdl);
		}
	}
	void _end_rec(_log_rec_handle hdl);

protected:
	MultiThreaded::Lock _lock;
	std::ostrstream	_oss;
	_log_vsl		_logs;
	log_show_flag	_show_flags;
	log_level		_level;
	log_level		_cur_level;
	_log_rec_que	_recs;

//==============================================================================
	friend logger_group &log_end_rec(logger_group &os);
	friend logger_group &log_end_rec(logger_group &os, log_handle hdl);

	template <class T>
	friend logger_group & operator << (logger_group &os, T t);
};

//******************************************************************************
// 重载logger_group"<<"操作符 (PS:VC6不支持成员模板特化)
//------------------------------------------------------------------------------
// 添加日志输出器
struct log_add_handle {
	explicit log_add_handle(log_entity *_entity, log_handle *_handle=NULL)
		: entity(_entity), handle(_handle) {}
	log_entity * const entity;
	log_handle * const handle;
};
inline logger_group &operator << (logger_group &os, log_add_handle handle) {
	return os.add_handle(handle.entity, handle.handle);
}

//------------------------------------------------------------------------------
// 添加日志文件
struct log_add_file {
	explicit log_add_file(const char *_fileName, long _nMaxLen=0, log_handle *_handle=NULL)
		: fileName(_fileName), nMaxLen(_nMaxLen), handle(_handle) {}
	const char * const fileName;
	const long nMaxLen;
	log_handle * const handle;
};
inline logger_group &operator << (logger_group &os, log_add_file file) {
	os.add_file(file.fileName, file.nMaxLen, file.handle);
	return os;
}

//------------------------------------------------------------------------------
// 添加日志管道
struct log_add_pipe {
	explicit log_add_pipe(const char *_pipeName, log_handle *_handle=NULL)
		: pipeName(_pipeName), handle(_handle) {}
	const char * const pipeName;
	log_handle * const handle;
};
inline logger_group &operator << (logger_group &os, log_add_pipe pipe) {
	log_entity_named_pipe<true> *pPipe = new log_entity_named_pipe<true>(pipe.pipeName);
	if (!pPipe->valid()) {
		pPipe->release();
		return os;
	}
	return os.add_handle(pPipe, pipe.handle);
}

//------------------------------------------------------------------------------
// 设置日志输出级别
struct log_set_level {
	explicit log_set_level(log_level _lvl) : level(_lvl) {}
	log_level level;
};
inline logger_group &operator << (logger_group &os, log_set_level level) {
	os._set_level(level.level);
	return os;
}

//------------------------------------------------------------------------------
// 开关日志句柄
struct log_turn_handle {
	log_turn_handle(log_handle _hdl, bool _onoff) : handle(_hdl), onoff(_onoff) {}
	log_handle	handle;
	bool		onoff;
};
inline logger_group &operator << (logger_group &os, log_turn_handle op) {
	os._turn(op.handle, op.onoff);
	return os;
}

//------------------------------------------------------------------------------
// 读配置文件
// 这是一个模板
// <?xml version="1.0" encoding="utf-8" ?>
// <config>
//	<level value="300" />
//	<show file="1" func="1" line="1" time="1" />
//	<target stdout="0" stderr="0" stdlog="0" debugger="1" logView="1" file="1" >
//		<file name="log" maxlen="4096" />
//		<logView maxline="1000" />
//	</target>
// </config>
struct log_config {
	explicit log_config(const char *_fileName) : fileName(_fileName) {}
	const char * const fileName;
};
logger_group &operator << (logger_group &os, log_config file);

//******************************************************************************
// 重载operator <<
inline logger_group &operator << (logger_group &os, logger_group &(*pFunc)(logger_group &)) {
	(*pFunc)(os);
	return os;
}

template <class T>
inline logger_group & operator << (logger_group &os, T t) {
	if (os._cur_level < os._level) { return os; }

	os._oss << t;

	return os;
}

//******************************************************************************
inline bool log_is_show(logger_group &os, LogShowFlag flag) {
	return os._is_show(flag);
}

//******************************************************************************
inline logger_group &log_set_cur_level(logger_group &os, log_level level) {
	os._set_cur_level(level);
	return os;
}

inline logger_group &log_get_level(logger_group &os, log_level &level) {
	level = os._get_level();
	return os;
}

inline void log_thrd_lock(logger_group &os) {
	os._thrd_lock();
}

inline void log_thrd_unlock(logger_group &os) {
	os._thrd_unlock();
}
//******************************************************************************
// 操纵子
inline logger_group &log_endl(logger_group &os) {
	return os << "\n\t";
}

inline logger_group &log_stdout(logger_group &os) {
	os._turn(logger_group::STDOUT, true);
	return os;
}
inline logger_group &log_no_stdout(logger_group &os) {
	os._turn(logger_group::STDOUT, false);
	return os;
}

inline logger_group &log_stderr(logger_group &os) {
	os._turn(logger_group::STDERR, true);
	return os;
}
inline logger_group &log_no_stderr(logger_group &os) {
	os._turn(logger_group::STDERR, false);
	return os;
}

inline logger_group &log_stdlog(logger_group &os) {
	os._turn(logger_group::STDLOG, true);
	return os;
}
inline logger_group &log_no_stdlog(logger_group &os) {
	os._turn(logger_group::STDLOG, false);
	return os;
}

inline logger_group &log_debugger(logger_group &os) {
	os._set_handle(logger_group::DEBUGGER, log_entity_debugger::inst());
	os._turn(logger_group::DEBUGGER, true);
	return os;
}
inline logger_group &log_no_debugger(logger_group &os) {
	os._turn(logger_group::DEBUGGER, false);
	os._set_handle(logger_group::DEBUGGER, log_entity_empty::inst());
	return os;
}

inline logger_group &log_log_view(logger_group &os) {
	os._set_handle(logger_group::LOG_VIEW, log_entity_log_view::inst());
	g_pLogView;
	os._turn(logger_group::LOG_VIEW, true);
	return os;
}
inline logger_group &log_no_log_view(logger_group &os) {
	os._turn(logger_group::LOG_VIEW, false);
	os._set_handle(logger_group::LOG_VIEW, log_entity_empty::inst());
	CLogView::deinst();
	return os;
}

inline logger_group &log_show_file(logger_group &os) {
	os._show(LSF_FILE, true);
	return os;
}
inline logger_group &log_no_show_file(logger_group &os) {
	os._show(LSF_FILE, false);
	return os;
}

inline logger_group &log_show_function(logger_group &os) {
	os._show(LSF_FUNCTION, true);
	return os;
}
inline logger_group &log_no_show_function(logger_group &os) {
	os._show(LSF_FUNCTION, false);
	return os;
}

inline logger_group &log_show_line(logger_group &os) {
	os._show(LSF_LINE, true);
	return os;
}
inline logger_group &log_no_show_line(logger_group &os) {
	os._show(LSF_LINE, false);
	return os;
}

inline logger_group &log_show_time(logger_group &os) {
	os._show(LSF_TIME, true);
	return os;
}
inline logger_group &log_no_show_time(logger_group &os) {
	os._show(LSF_TIME, false);
	return os;
}

inline logger_group &log_show_all(logger_group &os) {
	os._show(LSF_ALL, true);
	return os;
}
inline logger_group &log_show_none(logger_group &os) {
	os._show(LSF_ALL, false);
	return os;
}

//******************************************************************************
// 行输出封装器
// 说明:该类只用于生成临时对象(形如line_logger()),利用临时对象在语句结束时析构的
//		特性实现自行在行尾加上换行符
template <class _Os>
struct scope_logger {
	scope_logger(_Os &os,
		log_level nLogLvl,
		const char *pszFile="",
		const char *pszFunction="",
		int nLine=0)
		: _os(os)
	{
		log_begin_rec(os, nLogLvl, pszFile, pszFunction, nLine);
	}
	~scope_logger() {
		log_end_rec(_os);
	}

	_Os &get() { return _os; }

protected:
	_Os &_os;
};

struct handle_scope_logger {
	handle_scope_logger(logger_group &os,
		log_handle hdl,
		log_level nLogLvl,
		const char *pszFile="",
		const char *pszFunction="",
		int nLine=0)
		: _os(os), _hdl(hdl)
	{
		log_begin_rec(os, nLogLvl, pszFile, pszFunction, nLine);
	}
	~handle_scope_logger() {
		log_end_rec(_os, _hdl);
	}
	
	logger_group &get() { return _os; }
	
protected:
	logger_group &_os;
	const log_handle _hdl;
};

//******************************************************************************
template <class _Os>
struct line_logger {
	explicit line_logger(_Os &os)
		: _os(os) {}

	~line_logger() {
		_os << "\n\t";
	}

	_Os &get() { return _os; }

protected:
	_Os &_os;
};

//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
_HYLIB_END

////////////////////////////////////////////////////////////////////////////////
//******************************************************************************
// 行式log
#define _LINE_LOG_HELPER(os, hdl, level)	\
							(_HYLIB handle_scope_logger((os), (hdl), (level), \
								__FILE__, __funcname__, __LINE__).get())

#define HYLOGTrace(hdl)	_LINE_LOG_HELPER(_HYLIB logger_group::inst(), (hdl), _HYLIB LL_TRACE)
#define HYLOGDebug(hdl)	_LINE_LOG_HELPER(_HYLIB logger_group::inst(), (hdl), _HYLIB LL_DEBUG)
#define HYLOGInfo(hdl)	_LINE_LOG_HELPER(_HYLIB logger_group::inst(), (hdl), _HYLIB LL_INFO)
#define HYLOGWarn(hdl)	_LINE_LOG_HELPER(_HYLIB logger_group::inst(), (hdl), _HYLIB LL_WARN)
#define HYLOGErr(hdl)	_LINE_LOG_HELPER(_HYLIB logger_group::inst(), (hdl), _HYLIB LL_ERROR)
#define HYLOGFatal(hdl)	_LINE_LOG_HELPER(_HYLIB logger_group::inst(), (hdl), _HYLIB LL_FATAL)

//------------------------------------------------------------------------------
#define _LINE_LOGS_HELPER(OsType, os, level)	\
							(_HYLIB scope_logger<OsType>((os), (level), \
								__FILE__, __funcname__, __LINE__).get())

#define HYLOGSTrace	_LINE_LOGS_HELPER(_HYLIB logger_group, _HYLIB logger_group::inst(), _HYLIB LL_TRACE)
#define HYLOGSDebug	_LINE_LOGS_HELPER(_HYLIB logger_group, _HYLIB logger_group::inst(), _HYLIB LL_DEBUG)
#define HYLOGSInfo	_LINE_LOGS_HELPER(_HYLIB logger_group, _HYLIB logger_group::inst(), _HYLIB LL_INFO)
#define HYLOGSWarn	_LINE_LOGS_HELPER(_HYLIB logger_group, _HYLIB logger_group::inst(), _HYLIB LL_WARN)
#define HYLOGSErr	_LINE_LOGS_HELPER(_HYLIB logger_group, _HYLIB logger_group::inst(), _HYLIB LL_ERROR)
#define HYLOGSFatal	_LINE_LOGS_HELPER(_HYLIB logger_group, _HYLIB logger_group::inst(), _HYLIB LL_FATAL)
#define HYLOGSConfig (_HYLIB logger_group::inst())

//******************************************************************************
// 块式log:
#define _LOG_HELPER_BEGIN(os, level)	\
						_HYLIB log_begin_rec((os), (level), \
							__FILE__, __funcname__, __LINE__)
#define _LOG_HELPER_END(os, hdl)	_HYLIB log_end_rec((os), (hdl))
#define _LOG_HELPER_GUARD(os, hdl, level)	\
						_HYLIB handle_scope_logger __blk_log_guard((os), (hdl), (level), \
							__FILE__, __funcname__, __LINE__)

#define HYLOGBlk			(_HYLIB logger_group::inst())
#define HYLOGBlkL			(_HYLIB line_logger<_HYLIB logger_group>(_HYLIB logger_group::inst()).get())
#define HYLOGEnd(hdl)		_LOG_HELPER_END(_HYLIB logger_group::inst(), (hdl))
#define HYLOGBeginTrace()	_LOG_HELPER_BEGIN(_HYLIB logger_group::inst(), _HYLIB LL_TRACE)
#define HYLOGBeginDebug()	_LOG_HELPER_BEGIN(_HYLIB logger_group::inst(), _HYLIB LL_DEBUG)
#define HYLOGBeginInfo()	_LOG_HELPER_BEGIN(_HYLIB logger_group::inst(), _HYLIB LL_INFO)
#define HYLOGBeginWarn()	_LOG_HELPER_BEGIN(_HYLIB logger_group::inst(), _HYLIB LL_WARN)
#define HYLOGBeginErr()		_LOG_HELPER_BEGIN(_HYLIB logger_group::inst(), _HYLIB LL_ERROR)
#define HYLOGBeginFatal()	_LOG_HELPER_BEGIN(_HYLIB logger_group::inst(), _HYLIB LL_FATAL)

#define HYLOGGuardTrace(hdl)	_LOG_HELPER_GUARD(_HYLIB logger_group, _HYLIB logger_group::inst(), (hdl), _HYLIB LL_TRACE)
#define HYLOGGuardDebug(hdl)	_LOG_HELPER_GUARD(_HYLIB logger_group, _HYLIB logger_group::inst(), (hdl), _HYLIB LL_DEBUG)
#define HYLOGGuardInfo(hdl)		_LOG_HELPER_GUARD(_HYLIB logger_group, _HYLIB logger_group::inst(), (hdl), _HYLIB LL_INFO)
#define HYLOGGuardWarn(hdl)		_LOG_HELPER_GUARD(_HYLIB logger_group, _HYLIB logger_group::inst(), (hdl), _HYLIB LL_WARN)
#define HYLOGGuardErr(hdl)		_LOG_HELPER_GUARD(_HYLIB logger_group, _HYLIB logger_group::inst(), (hdl), _HYLIB LL_ERROR)
#define HYLOGGuardFatal(hdl)	_LOG_HELPER_GUARD(_HYLIB logger_group, _HYLIB logger_group::inst(), (hdl), _HYLIB LL_FATAL)
//------------------------------------------------------------------------------

#define _LOGS_HELPER_BEGIN(os, level)	\
						_HYLIB log_begin_rec((os), (level), \
							__FILE__, __funcname__, __LINE__)
#define _LOGS_HELPER_END(os)	_HYLIB log_end_rec((os))
#define _LOGS_HELPER_GUARD(OsType, os, level)	\
						_HYLIB scope_logger<OsType> __blk_logs_guard((os), (level), \
							__FILE__, __funcname__, __LINE__)

#define HYLOGSBlk			(_HYLIB logger_group::inst())
#define HYLOGSBlkL			(_HYLIB line_logger<_HYLIB logger_group>(_HYLIB logger_group::inst()).get())
#define HYLOGSEnd()			_LOGS_HELPER_END(_HYLIB logger_group::inst())
#define HYLOGSBeginTrace()	_LOGS_HELPER_BEGIN(_HYLIB logger_group::inst(), _HYLIB LL_TRACE)
#define HYLOGSBeginDebug()	_LOGS_HELPER_BEGIN(_HYLIB logger_group::inst(), _HYLIB LL_DEBUG)
#define HYLOGSBeginInfo()	_LOGS_HELPER_BEGIN(_HYLIB logger_group::inst(), _HYLIB LL_INFO)
#define HYLOGSBeginWarn()	_LOGS_HELPER_BEGIN(_HYLIB logger_group::inst(), _HYLIB LL_WARN)
#define HYLOGSBeginErr()	_LOGS_HELPER_BEGIN(_HYLIB logger_group::inst(), _HYLIB LL_ERROR)
#define HYLOGSBeginFatal()	_LOGS_HELPER_BEGIN(_HYLIB logger_group::inst(), _HYLIB LL_FATAL)

#define HYLOGSGuardTrace()	_LOGS_HELPER_GUARD(_HYLIB logger_group, _HYLIB logger_group::inst(), _HYLIB LL_TRACE)
#define HYLOGSGuardDebug()	_LOGS_HELPER_GUARD(_HYLIB logger_group, _HYLIB logger_group::inst(), _HYLIB LL_DEBUG)
#define HYLOGSGuardInfo()	_LOGS_HELPER_GUARD(_HYLIB logger_group, _HYLIB logger_group::inst(), _HYLIB LL_INFO)
#define HYLOGSGuardWarn()	_LOGS_HELPER_GUARD(_HYLIB logger_group, _HYLIB logger_group::inst(), _HYLIB LL_WARN)
#define HYLOGSGuardErr()	_LOGS_HELPER_GUARD(_HYLIB logger_group, _HYLIB logger_group::inst(), _HYLIB LL_ERROR)
#define HYLOGSGuardFatal()	_LOGS_HELPER_GUARD(_HYLIB logger_group, _HYLIB logger_group::inst(), _HYLIB LL_FATAL)

////////////////////////////////////////////////////////////////////////////////
#endif // _LOG_H_
