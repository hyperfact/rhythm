#include "StdAfx.h"
#include "log.h"
#include "tinyxml/tinyxml.h"
#include "util.h"

_HYLIB_BEGIN
_HYLIB_DETAIL_USING

log_entity_std logger_group::__le_stdout(std::cout);
log_entity_std logger_group::__le_stderr(std::cerr);
log_entity_std logger_group::__le_stdlog(std::clog);

////////////////////////////////////////////////////////////////////////////////
// struct logger_group:
logger_group::~logger_group()
{
	for (_log_iter iter = _logs.begin();
		iter != _logs.end();
		++iter) {
		if (*iter) {
			(*iter)->release();
		}
	}
}

//******************************************************************************
logger_group & logger_group::add_file( const char *szFile, int nMaxLen/*=0*/, log_handle *nHandle/*=NULL*/ )
{
	using namespace std;

	log_entity_file *lef = new log_entity_file(szFile, nMaxLen);
	if (NULL == lef) {
		return *this;
	}
	if (!lef->valid()) {
		lef->release();
		return *this;
	}

	return add_handle(lef, nHandle);
}

void logger_group::_end_rec( _log_rec_handle hdl )
{
	if (hdl->cur_level < _level) { return; }

	hdl->oss << std::endl << std::ends;
	
	for (logger_group::_log_iter iter = _logs.begin();
		iter != _logs.end();
		++iter)
	{
		if ((*iter)->enable()) {
			(*iter)->write(hdl->oss.str().c_str());
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
#ifdef _USING_MFC
CLogView & log_end_rec( CLogView &os )
{
	_log_thrd_unlock_guard<CLogView> ltug(os);

	os << std::endl << std::ends;
	
	os.AppendLog(os.str());
	
	// reset stream
	os.freeze(false);
	os.seekp(0);

	return os;
}
#endif // _USING_MFC

////////////////////////////////////////////////////////////////////////////////
logger_group & operator<<( logger_group &os, log_config file )
{
	try {
		TiXmlDocument doc;
		if (!doc.LoadFile(file.fileName)) { throw 0; }
		
		TiXmlElement *pRoot = doc.RootElement();
		if (NULL == pRoot) { throw 0; }
		
		TiXmlElement *pElm = NULL;
		int nVal = 0;
		const char *sVal = NULL;
		
		// 日志级别
		pElm = pRoot->FirstChildElement("level");
		if (NULL != pElm) {
			pElm->Attribute("value", &nVal);
			os._set_level(nVal);
		}
		
		// 日志头显示内容
		pElm = pRoot->FirstChildElement("show");
		if (NULL != pElm) {
			pElm->Attribute("file", &nVal);
			if (0 != nVal) { os._show(LSF_FILE); }
			pElm->Attribute("func", &nVal);
			if (0 != nVal) { os._show(LSF_FUNCTION); }
			pElm->Attribute("line", &nVal);
			if (0 != nVal) { os._show(LSF_LINE); }
			pElm->Attribute("time", &nVal);
			if (0 != nVal) { os._show(LSF_TIME); }
		}
		
		// 输出目标
		pElm = pRoot->FirstChildElement("target");
		if (NULL != pElm) {
			pElm->Attribute("stdout", &nVal);
			if (0 != nVal) { os._turn(logger_group::STDOUT, true); }

			pElm->Attribute("stderr", &nVal);
			if (0 != nVal) { os._turn(logger_group::STDERR, true); }

			pElm->Attribute("stdlog", &nVal);
			if (0 != nVal) { os._turn(logger_group::STDLOG, true); }

			pElm->Attribute("debugger", &nVal);
			if (0 != nVal) { os._turn(logger_group::DEBUGGER, true); }

			pElm->Attribute("logView", &nVal);
			if (0 != nVal) {
				os._turn(logger_group::LOG_VIEW, true);
				TiXmlElement *pElm1 = pElm->FirstChildElement("logView");
				pElm1->Attribute("maxline", &nVal);
				g_pLogView->SetMaxLine(nVal);
			}

			pElm->Attribute("file", &nVal);
			if (0!=nVal && !pElm->NoChildren()) {
				TiXmlElement *pElm1 = pElm->FirstChildElement("file");
				while (NULL != pElm1) {
					sVal = pElm1->Attribute("name");
					pElm1->Attribute("maxlen", &nVal);
					if (sVal && sVal[0]) { os.add_file(sVal, nVal); }
					pElm1 = pElm1->NextSiblingElement("file");
				}
			}
		}
	} catch (...) {
		os._default_config();
	}
	return os;
}

//******************************************************************************
logger_group &log_end_rec( logger_group &os )
{
	_log_thrd_unlock_guard<logger_group> ltug(os);

	if (os._cur_level < os._level) { return os; }

	os._oss << std::endl << std::ends;

	for (logger_group::_log_iter iter = os._logs.begin();
		iter != os._logs.end();
		++iter)
	{
		if ((*iter)->enable()) {
			(*iter)->write(os._oss.str());
		}
	}

	// reset stream
	os._oss.freeze(false);
	os._oss.seekp(0);
	
	return os;
}

logger_group &log_end_rec(logger_group &os, log_handle hdl)
{
	_log_thrd_unlock_guard<logger_group> ltug(os);
	
	if (os._cur_level < os._level) { return os; }
	
	os._oss << std::endl << std::ends;
	
	if (!BTW_LE(hdl, 0, os._logs.size())) {
		hdl = 0;
	}
	os._logs[hdl]->write(os._oss.str());
	
	// reset stream
	os._oss.freeze(false);
	os._oss.seekp(0);
	
	return os;
}

////////////////////////////////////////////////////////////////////////////////
_HYLIB_END
