#ifndef MAIN_H
#define MAIN_H 1

#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <vector>
#include <sstream>
#include <errno.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include <list>

using namespace std;

#include "../XMLLogger/Source/MyLogHandler.h"
#include "../filehandling/CSV_reader.h"

#define COMPILE_TIMESTAMP __DATE__ " " __TIME__
#define FILE_INFO __FILE__, __LINE__

CMyLogHandler xmllog(".\\XMLLogger\\Log\\DebugLog2.xml");
const std::string path_pic = "sprites/";
const std::string path_log = "logs/";

//bool Focused = true;

#endif