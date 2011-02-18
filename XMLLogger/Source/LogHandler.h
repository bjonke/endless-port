
#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include <fstream>
using std::ofstream;

#include <string>
using std::string;

#include <vector>
using std::vector;

class CFile
{
private:
	string	m_sFileName;
	
public:
	CFile(const string sFileName): m_sFileName(sFileName) {}
	virtual ~CFile() {}
	
	void ClearFile()
	{
		ofstream outFile(m_sFileName.c_str(), std::ios::out | std::ios::trunc);
		outFile.close();
	}
	
	template <typename T> bool Write(const T& Data)
	{
		try {
			ofstream outFile(m_sFileName.c_str(), std::ios::app); // Set the append flag
			if (!outFile.is_open()) {
				throw std::ios::failure("Couldn't open file");
			}
			
			outFile << Data;
			outFile.close();
			
			return true;
		}
		catch (...) {
			return false;
		}
	}
};

class CLogHandler
{
private:
	vector<string>	m_vsTags;
	unsigned int	m_iIndent;

protected:
	CFile*			m_CFile;

public:
	CLogHandler(const string& sFileName)
	{
		m_iIndent = 0;
		m_CFile	= new CFile(sFileName);
		m_CFile->ClearFile();
		m_CFile->Write("<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>\n");
	}
	
	virtual ~CLogHandler()
	{
		while(m_vsTags.size() > 0) {
			CloseTagTree();
		}
		
		if (m_CFile) 
			delete m_CFile;
		m_CFile = 0;
	}

	void AddTagTree	(const string& TagName)
	{
		m_CFile->Write(string(m_iIndent, '\t') + string("<") + TagName + string(">\n"));
		m_vsTags.push_back(TagName);
		m_iIndent += 1;
	}

	template <typename T> 
	void AddTag(const string& TagName, const T& TagValue)
	{
		m_CFile->Write(string(m_iIndent, '\t') + string("<") + TagName + string(">") );
		m_CFile->Write(TagValue);
		m_CFile->Write(string("</") + TagName + string(">\n"));
	}
	
	void CloseTagTree()
	{
		if (m_vsTags.size() > 0) {
			if (m_iIndent > 0) {
				m_iIndent -= 1;
			}
			m_CFile->Write(string(m_iIndent, '\t') + string("</") + m_vsTags[m_vsTags.size()-1] + string(">\n"));
			m_vsTags.pop_back();
		}
	}
};

#endif LOGHANDLER_H