// e.g.
// 日志库
// #define _LOG_HELPER_USE_LOG4CXX
// #define _LOG_HELPER_USE_LOG4CPLUS
// 关闭日志
// #define _LOG_HELPER_SUPPRESS_LOG
// 日志名前缀
// #define _LOG_HELPER_PREFIX	_T("LOG")
// 日志输入库路径
// #define _LOG_HELPER_LIB_PATH "path/to/lib"
// 日志分类
// #define _LOG_HELPER_CATELOG 0
// 编译时日志等级设置
// #define _LOG_HELPER_DISABLE_TRACE
// #define _LOG_HELPER_DISABLE_DEBUG
// #define _LOG_HELPER_DISABLE_INFO
// #define _LOG_HELPER_DISABLE_WARN
// #define _LOG_HELPER_DISABLE_ERROR
// #define _LOG_HELPER_DISABLE_FATAL

#if defined(_LOG_HELPER_DISABLE_FATAL) && !defined(_LOG_HELPER_DISABLE_ERROR)
#define _LOG_HELPER_DISABLE_ERROR
#endif
#if defined(_LOG_HELPER_DISABLE_ERROR) && !defined(_LOG_HELPER_DISABLE_WARN)
#define _LOG_HELPER_DISABLE_WARN
#endif
#if defined(_LOG_HELPER_DISABLE_WARN) && !defined(_LOG_HELPER_DISABLE_INFO)
#define _LOG_HELPER_DISABLE_INFO
#endif
#if defined(_LOG_HELPER_DISABLE_INFO) && !defined(_LOG_HELPER_DISABLE_DEBUG)
#define _LOG_HELPER_DISABLE_DEBUG
#endif
#if defined(_LOG_HELPER_DISABLE_DEBUG) && !defined(_LOG_HELPER_DISABLE_TRACE)
#define _LOG_HELPER_DISABLE_TRACE
#endif

#ifndef _LOG_HELPER_PREFIX
#  error please define macro _LOG_HELPER_PREFIX
#endif // _LOG_HELPER_PREFIX

#ifndef _LOG_HELPER_CATELOG
#define _LOG_HELPER_CATELOG 0
#endif // _LOG_HELPER_CATELOG

#ifndef _LOG_HELPER_NOTHING
#  define _LOG_HELPER_NOTHING()				\
	__pragma (warning (push))				\
	__pragma (warning (disable:4127))		\
	do { } while (0)						\
	__pragma (warning (pop))
#endif // NOTHING

#ifndef _LOG_HELPER_NOTHING_CLS
struct _LOG_HELPER_NOTHING_CLS {
	_LOG_HELPER_NOTHING_CLS(...) {}
};
#endif // _LOG_HELPER_NOTHING_CLS

#undef LOG_TRACE
#undef LOG_DEBUG
#undef LOG_INFO
#undef LOG_WARN
#undef LOG_ERROR
#undef LOG_FATAL

#ifndef _LOG_HELPER_SUPPRESS_LOG

#ifndef _LOG_HELPER_INC_GUARD
#define _LOG_HELPER_INC_GUARD

#if defined(_LOG_HELPER_USE_LOG4CXX)
#  include <log4cxx/xml/domconfigurator.h>
#  include <log4cxx/propertyconfigurator.h>
#  include <log4cxx/logger.h>
#  include <log4cxx/ndc.h>
#  include <log4cxx/mdc.h>
#  define _LOG_HELPER_LIB "log4cxx"

#  define _LOG_HELPER_TRACE(event)	LOG4CXX_TRACE(_log_helper_impl::LogInst(_LOG_HELPER_PREFIX, _LOG_HELPER_CATELOG), event)
#  define _LOG_HELPER_DEBUG(event)	LOG4CXX_DEBUG(_log_helper_impl::LogInst(_LOG_HELPER_PREFIX, _LOG_HELPER_CATELOG), event)
#  define _LOG_HELPER_INFO(event)	LOG4CXX_INFO(_log_helper_impl::LogInst(_LOG_HELPER_PREFIX, _LOG_HELPER_CATELOG), event)
#  define _LOG_HELPER_WARN(event)	LOG4CXX_WARN(_log_helper_impl::LogInst(_LOG_HELPER_PREFIX, _LOG_HELPER_CATELOG), event)
#  define _LOG_HELPER_ERROR(event)	LOG4CXX_ERROR(_log_helper_impl::LogInst(_LOG_HELPER_PREFIX, _LOG_HELPER_CATELOG), event)
#  define _LOG_HELPER_FATAL(event)	LOG4CXX_FATAL(_log_helper_impl::LogInst(_LOG_HELPER_PREFIX, _LOG_HELPER_CATELOG), event)

#  define LOG_NDC_PUSH(msg) do {					\
		::log4cxx::helpers::MessageBuffer oss_;		\
		::log4cxx::NDC::push(oss_.str(oss_ << msg));\
	} while(0)

#  define LOG_NDC_POP() do {						\
		::log4cxx::NDC::pop();						\
	} while(0)

#  define LOG_NDC ::log4cxx::NDC

#  define LOG_MDC_PUT(key, val)		::log4cxx::MDC::put(key, val)
#  define LOG_MDC_GET(key)			::log4cxx::MDC::get(key)
#  define LOG_MDC_DEL(key)			::log4cxx::MDC::remove(key)
#  define LOG_MDC					::log4cxx::MDC

#elif defined(_LOG_HELPER_USE_LOG4CPLUS)
#  include <log4cplus/loggingmacros.h>
#  include <log4cplus/configurator.h>
#  define _LOG_HELPER_LIB "log4cplus"

#  define _LOG_HELPER_TRACE(event)	LOG4CPLUS_TRACE(_log_helper_impl::LogInst(_LOG_HELPER_PREFIX, _LOG_HELPER_CATELOG), event)
#  define _LOG_HELPER_DEBUG(event)	LOG4CPLUS_DEBUG(_log_helper_impl::LogInst(_LOG_HELPER_PREFIX, _LOG_HELPER_CATELOG), event)
#  define _LOG_HELPER_INFO(event)	LOG4CPLUS_INFO(_log_helper_impl::LogInst(_LOG_HELPER_PREFIX, _LOG_HELPER_CATELOG), event)
#  define _LOG_HELPER_WARN(event)	LOG4CPLUS_WARN(_log_helper_impl::LogInst(_LOG_HELPER_PREFIX, _LOG_HELPER_CATELOG), event)
#  define _LOG_HELPER_ERROR(event)	LOG4CPLUS_ERROR(_log_helper_impl::LogInst(_LOG_HELPER_PREFIX, _LOG_HELPER_CATELOG), event)
#  define _LOG_HELPER_FATAL(event)	LOG4CPLUS_FATAL(_log_helper_impl::LogInst(_LOG_HELPER_PREFIX, _LOG_HELPER_CATELOG), event)

#else
#  error no log4c* used, to use one, defined _LOG_HELPER_USE_LOG4CXX or _LOG_HELPER_USE_LOG4CPLUS
#endif

#define _LOG_HELPER_NOTHING_E(event) _LOG_HELPER_NOTHING()

#ifndef _LOG_HELPER_DISABLE_TRACE
#  define LOG_TRACE	_LOG_HELPER_TRACE
#else // _LOG_HELPER_DISABLE_TRACE
#  define LOG_TRACE	_LOG_HELPER_NOTHING_E
#endif // _LOG_HELPER_DISABLE_TRACE

#ifndef _LOG_HELPER_DISABLE_DEBUG
#  define LOG_DEBUG	_LOG_HELPER_DEBUG
#else // _LOG_HELPER_DISABLE_DEBUG
#  define LOG_DEBUG	_LOG_HELPER_NOTHING_E
#endif // _LOG_HELPER_DISABLE_DEBUG

#ifndef _LOG_HELPER_DISABLE_INFO
#  define LOG_INFO	_LOG_HELPER_INFO
#else // _LOG_HELPER_DISABLE_INFO
#  define LOG_INFO	_LOG_HELPER_NOTHING_E
#endif // _LOG_HELPER_DISABLE_INFO

#ifndef _LOG_HELPER_DISABLE_WARN
#  define LOG_WARN	_LOG_HELPER_WARN
#else // _LOG_HELPER_DISABLE_WARN
#  define LOG_WARN	_LOG_HELPER_NOTHING_E
#endif // _LOG_HELPER_DISABLE_WARN

#ifndef _LOG_HELPER_DISABLE_ERROR
#  define LOG_ERROR	_LOG_HELPER_ERROR
#else // _LOG_HELPER_DISABLE_ERROR
#  define LOG_ERROR	_LOG_HELPER_NOTHING_E
#endif // _LOG_HELPER_DISABLE_ERROR

#ifndef _LOG_HELPER_DISABLE_FATAL
#  define LOG_FATAL	_LOG_HELPER_FATAL
#else // _LOG_HELPER_DISABLE_FATAL
#  define LOG_FATAL	_LOG_HELPER_NOTHING_E
#endif // _LOG_HELPER_DISABLE_FATAL

#  include <string>
#  include <map>
#  ifdef _LOG_HELPER_LIB_PATH
#    pragma comment(lib, _LOG_HELPER_LIB_PATH "/" _LOG_HELPER_LIB ".lib")
#  else // _LOG_HELPER_LIB_PATH
#    pragma comment(lib, _LOG_HELPER_LIB ".lib")
#  endif // _LOG_HELPER_LIB_PATH

#  define LOG_INIT(config_file)	_log_helper_impl::InitLogSys(config_file)

#if defined(_LOG_HELPER_USE_LOG4CXX)

#elif defined(_LOG_HELPER_USE_LOG4CPLUS)

#endif

namespace _log_helper_impl {

#if defined(_LOG_HELPER_USE_LOG4CXX)
	typedef log4cxx::LoggerPtr LoggerPtr;
#elif defined(_LOG_HELPER_USE_LOG4CPLUS)
	typedef log4cplus::Logger LoggerPtr;
#endif
	class CCriticalSection
	{
	public:
		CCriticalSection() { ::InitializeCriticalSection(&m_sect); }
		~CCriticalSection() { ::DeleteCriticalSection(&m_sect); }

		operator CRITICAL_SECTION*() { return &m_sect; }
		CRITICAL_SECTION m_sect;

		BOOL Lock() { ::EnterCriticalSection(&m_sect); return TRUE; }
		BOOL Unlock() { ::LeaveCriticalSection(&m_sect); return TRUE; }
	};

	template <class L>
	class CLockGuard {
	public:
		CLockGuard(L &l) : l_(l) { l_.Lock(); }
		~CLockGuard() { l_.Unlock(); }
	private:
		L &l_;
	};

	template <class T>
	struct LogInstHelper {
		typedef T MapKeyType;
		static const char *LoggerName(const char *prefix, T catelog) {
			return prefix;
		}
	};
	template <>
	struct LogInstHelper<int> {
		typedef int MapKeyType;
		static const char *LoggerName(const char *prefix, int catelog) {
			static char buf[256] = {0};
			if (0 == catelog)
				return prefix;
			_snprintf(buf, sizeof(buf)/sizeof(buf[0]), "%s.%d", prefix, catelog);
			return buf;
		}
	};
	template <>
	struct LogInstHelper<const char *> {
		typedef std::string MapKeyType;
		static const char *LoggerName(const char *prefix, const char * catelog) {
			static char buf[256] = {0};
			if (NULL==catelog || NULL==catelog[0])
				return prefix;
			_snprintf(buf, sizeof(buf)/sizeof(buf[0]), "%s.%s", prefix, catelog);
			return buf;
		}
	};

	template <class T>
	LoggerPtr &LogInst(const char *prefix, T catelog) {
		using namespace std;
		typedef typename LogInstHelper<T>::MapKeyType KeyType;
		typedef map<KeyType, LoggerPtr> LoggerMap;
		typedef LoggerMap::iterator LoggerIter;

		static LoggerMap _map_loggers;
		static CCriticalSection _cs;

		CLockGuard<CCriticalSection> l(_cs);
		LoggerIter iF = _map_loggers.find(catelog);
		if (iF == _map_loggers.end()) {
			const char *logger_name = LogInstHelper<T>::LoggerName(prefix, catelog);
#if defined(_LOG_HELPER_USE_LOG4CXX)
			return _map_loggers[catelog] = log4cxx::Logger::getLogger(logger_name);
#elif defined(_LOG_HELPER_USE_LOG4CPLUS)
			return _map_loggers[catelog] = log4cplus::Logger::getInstance(logger_name);
#endif
		} else {
			return iF->second;
		}
	}

	inline void InitLogSys(const TCHAR *config_file) {
#if defined(_LOG_HELPER_USE_LOG4CPLUS)
		log4cplus::initialize();
#endif
		try {
			TCHAR szFilePath[MAX_PATH];
			ZeroMemory(szFilePath, sizeof(szFilePath));
			::GetModuleFileName(NULL, szFilePath, MAX_PATH);
			TCHAR *pLastSlash = _tcsrchr(szFilePath, _T('\\'));
			if (NULL == pLastSlash)
				return;
			_tcscpy_s(pLastSlash+1, szFilePath+MAX_PATH-(pLastSlash+1), config_file);
#if defined(_LOG_HELPER_USE_LOG4CXX)
			TCHAR *pSuf = _tcsrchr(szFilePath, _T('.'));
			if (pSuf && 0 == _tcsicmp(pSuf+1, _T("xml"))) {
				log4cxx::xml::DOMConfigurator::configure(szFilePath);
			} else {
				log4cxx::PropertyConfigurator::configure(szFilePath);
			}		
#elif defined(_LOG_HELPER_USE_LOG4CPLUS)
			log4cplus::PropertyConfigurator::doConfigure(szFilePath);
#endif
			//LOG_TRACE("trace");
			//LOG_DEBUG("debug");
			//LOG_INFO("info");
			//LOG_WARN("warn");
			//LOG_ERROR("error");
			//LOG_FATAL("fatal");
			LOG_INFO("log sys initialized.");
		} catch (...) {
			OutputDebugString("log system initializes failed!");
		}
	}
}
#endif // _LOG_HELPER_INC_GUARD

#else // _LOG_HELPER_SUPPRESS_LOG

#ifndef LOG_INIT
#  define LOG_INIT(config_file)		_LOG_HELPER_NOTHING()
#endif // LOG_INIT

#  define LOG_TRACE(event)	_LOG_HELPER_NOTHING()
#  define LOG_DEBUG(event)	_LOG_HELPER_NOTHING()
#  define LOG_INFO(event)	_LOG_HELPER_NOTHING()
#  define LOG_WARN(event)	_LOG_HELPER_NOTHING()
#  define LOG_ERROR(event)	_LOG_HELPER_NOTHING()
#  define LOG_FATAL(event)	_LOG_HELPER_NOTHING()
#  define LOG_NDC_PUSH(msg)	_LOG_HELPER_NOTHING()
#  define LOG_NDC_POP()		_LOG_HELPER_NOTHING()
#  define LOG_NDC			_LOG_HELPER_NOTHING_CLS
#  define LOG_MDC_PUT(key, val) _LOG_HELPER_NOTHING()
#  define LOG_MDC_GET(key)	_LOG_HELPER_NOTHING()
#  define LOG_MDC_DEL(key)	_LOG_HELPER_NOTHING()
#  define LOG_MDC			_LOG_HELPER_NOTHING_CLS

#endif // _LOG_HELPER_SUPPRESS_LOG

//#undef _LOG_HELPER_CATELOG
