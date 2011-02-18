/* **************************************************************************
 * @version $Id: CInputBoxTest.c,v 0.1 2006/03/31 02:26:00 legolas558 Exp $
 *
 * File:		CInputBoxTest.c
 * Content:		test program for the CInputBox class
 * Notes:		compiles with C language rules
 *
 * Copyright(c) 2006 by legolas558
 *
 * https://sourceforge.net/projects/cinputbox
 *
 * This software is released under a BSD license. See LICENSE.TXT
 * You must accept the license before using this software.
 *
 * **************************************************************************/


#include <windows.h>

#include "../InputBox.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR sCmdLine, int nCmd) {
	CInputBox	myinp;
	char	result[21];

	CInputBox_init(&myinp, hInst);

	strcpy(result, "");

	ShowInputBox(&myinp, 0, "Title1", "Caption1", result, 20);

	MessageBox(0, result, "Got this", 0);
	
	ShowInputBox(&myinp, 0, "Title2", "Caption2", result, 20);
//	hAppInstance = hInst;
//	ShowInputBox("Testing this", "default", 0, 1);

	CInputBox_free(&myinp);
	exit(0);
	return 0;
}
