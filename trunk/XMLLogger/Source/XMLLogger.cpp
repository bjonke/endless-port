
#include "MyLogHandler.h"

#define COMPILE_TIMESTAMP __DATE__ " " __TIME__
#define FILE_INFO __FILE__, __LINE__

int main(int argc, char* argv[])
{
	CMyLogHandler log(".\\Log\\DebugLog.xml");

	log.AddTagTree("Project");
	log.AddTag("Name", "XMLLogger");

	log.AddTag("Type", "Project");
	log.AddTag("FileName", __FILE__);
	log.AddTag("Compiled", COMPILE_TIMESTAMP);

	log.AddTagTree("CmdArguments");
	for (int i = 0; i < argc; ++i) {
		log.AddTag("CmdArgument", argv[i]);
	}
	log.CloseTagTree(); // CmdArguments

	log.AddTagTree("CommentLines");
	log.AddTag("CommentLine", "This is a test");
	log.AddTag("CommentLine", "of multiple lines of");
	log.AddTag("CommentLine", "comments.");
	log.CloseTagTree();	// CommentLines

	log.AddTagTree("LogEntries");
	log.WriteLog("Main", "main", "Info", "Starting up", FILE_INFO);
	log.WriteLog("Main", "main", "Info", "Second test", FILE_INFO);
	log.WriteLog("Main", "main", "Warning", "A fake warning", FILE_INFO);
	log.WriteLog("CLogHandler", "Write", "Info", "Writing down some stuff...", FILE_INFO);
	log.WriteLog("Main", "main", "Error", "A fake error!", FILE_INFO);
	log.WriteLog("Main", "main", "Critical", "Terminating", FILE_INFO);
	log.WriteLog("Main", "main", "Info", "(I'm fine, don't worry!)", FILE_INFO);
	log.WriteLog("Main", "main", "Info", "Look below it's a integer!", FILE_INFO);
	log.WriteLog("Main", "main", "Info", 666, FILE_INFO);
	log.CloseTagTree(); // LogEntries

	return 0;
}