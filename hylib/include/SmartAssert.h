#ifndef _SMARTASSERT_H_
#define _SMARTASSERT_H_

#pragma once
#include <sstream>
#include <iostream>
#include <exception>

#ifdef _WIN32
# //include <winbase.h>
#endif // _WIN32

class SmartAssert {
public:
	enum {
		LVL_NONE,
		LVL_WARN = 0x10,
		LVL_DEBUG= 0x20,
		LVL_ERROR= 0x30,
		LVL_FATAL= 0x40,
	};
	typedef unsigned LevelType;

public:
	class Context {
	public:
		LevelType Level() const { return level_; }
		const std::string &Msg() const { return msg_; }
		const std::string &Expr() const { return expr_; }
		std::string Vars() const { return vars_.str(); }
	protected:
		LevelType	level_;
		std::string	msg_;
		std::string	expr_;
		std::ostringstream	vars_;
		
		friend class SmartAssert;
	};
	typedef void (*handler_t)(const Context &ctx);
	static void SetHandler(handler_t );

	class AssertionFailedException : public std::exception {
	public:
		AssertionFailedException(const Context &ctx) : ctx_(ctx) {}
	protected:
		const Context &ctx_;
	};

public:
	SmartAssert() : SMART_ASSERT_A(*this), SMART_ASSERT_B(*this) {
		ctx_.level_ = LVL_DEBUG;
	}

	~SmartAssert() {
		handle();
	}

	template <class T>
	SmartAssert &print(T var, const char *varname) {
		ctx_.vars_ << "\t" << varname << ": " << var << std::endl;
		return *this;
	}

	SmartAssert &print_context(const char *expr, const char *file, int line) {
		std::ostringstream oss;
		oss << "Assertion (" << expr
			<< ") failed at File: " << file
			<< ", Line: " << line;
		ctx_.expr_ = oss.str();
		return *this;
	}

	SmartAssert &level(int lvl, const char *msg=0) {
		ctx_.level_ = lvl;
		if (msg)
			ctx_.msg_ = msg;
		return *this;
	}

	SmartAssert &warn(const char *msg=0) {
		return level(LVL_WARN, msg);
	}
	SmartAssert &debug(const char *msg=0) {
		return level(LVL_DEBUG, msg);
	}
	SmartAssert &error(const char *msg=0) {
		return level(LVL_ERROR, msg);
	}
	SmartAssert &fatal(const char *msg=0) {
		return level(LVL_FATAL, msg);
	}

	void handle() {
		if (handler_)
			(*handler_)(ctx_);
		else
#ifdef _WIN32
			ODSHandler(ctx_);
#else // _WIN32
			StderrHandler(ctx_);
#endif // _WIN32
	}

	static void DefaultProcess(const Context &ctx) {
		switch (ctx.Level())
		{
		case LVL_NONE:
		case LVL_DEBUG:
#ifdef _WIN32
#  if defined(_CrtDbgBreak)
			_CrtDbgBreak();
#  else // _CrtDbgBreak
			DebugBreak();
#  endif // _CrtDbgBreak
#endif // _WIN32
			break;
		case LVL_WARN:
			// do nothing
			break;
		case LVL_ERROR:
			throw AssertionFailedException(ctx);
			break;
		case LVL_FATAL:
			abort();
			break;
		}
	}

	static void StderrHandler(const Context &ctx) {
		std::cerr << ctx.Expr() << ", " << ctx.Msg() << std::endl
			<< ctx.Vars() << std::endl;
		DefaultProcess(ctx);
	}

#ifdef _WIN32
	static void ODSHandler(const Context &ctx) {
		std::ostringstream oss;
		oss << ctx.Expr() << ", " << ctx.Msg() << std::endl
			<< ctx.Vars() << std::endl;
		::OutputDebugString(oss.str().c_str());
		DefaultProcess(ctx);
	}
#endif // _WIN32

public:
	SmartAssert &SMART_ASSERT_A, &SMART_ASSERT_B;

protected:
	Context ctx_;

	static handler_t handler_;
};

__declspec(selectany) SmartAssert::handler_t SmartAssert::handler_;

inline SmartAssert make_assert() {
	return SmartAssert();
}

#define SMART_ASSERT_A(x) SMART_ASSERT_OP(x, B)
#define SMART_ASSERT_B(x) SMART_ASSERT_OP(x, A)
#define SMART_ASSERT_OP(x, next) \
	SMART_ASSERT_A.print((x), #x).SMART_ASSERT_##next

#ifndef _DISABLE_SMART_ASSERT
#  define SMART_ASSERT(expr) \
		if( (expr) ) ; \
		else make_assert().print_context(#expr, __FILE__,__LINE__).SMART_ASSERT_A
#else // _ENABLE_ASSERT
#  define SMART_ASSERT(expr) ((void)0)
#endif // _ENABLE_ASSERT

/** e.g.
	inline void test_smart_assert() {
		int a=0;
		char *b = "char";
		SMART_ASSERT(true)(a)(b)(true);
	}
*/

#endif // _SMARTASSERT_H_
