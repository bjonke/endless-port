
#ifndef MYLOGHANDLER_H
#define MYLOGHANDLER_H

#include "time.h"
#include "LogHandler.h"

class CMyLogHandler: public CLogHandler
{
public:
	CMyLogHandler(const string& sFileName): CLogHandler(sFileName)
	{
		this->m_CFile->Write("<?xml-stylesheet type=\"text/xsl\" href=\"stylesheet.xsl\" ?>\n\n");
		this->m_CFile->Write("<!-- Created by XMLLogger -->\n\n");
	}
	
	virtual ~CMyLogHandler() {}

	template <typename T>
	void WriteLog(const string& sClassName,
				  const string& sFuncName, 
				  const string& sDebugType, 
				  const T&      tInfo, 
				  const string& sFile, 
				  const long&   lLine)
	{
		time_t systemTime;
		systemTime = time(NULL);
		
		this->AddTagTree("LogEntry");
		this->AddTag("ClassName",	sClassName);
		this->AddTag("FuncName",	sFuncName);
		this->AddTag("DebugType",	sDebugType);
		this->AddTag("Timestamp",	ctime(&systemTime));
		this->AddTag("Info",		tInfo);
		this->AddTag("Line",		lLine);
		this->AddTag("File",		sFile);
		this->CloseTagTree();	// LogEntry
	}
};

#endif MYLOGHANDLER_H