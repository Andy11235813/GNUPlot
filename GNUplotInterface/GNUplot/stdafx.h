/************************************************************************
*																																				*
*					stdafx.h -- Precompile header file for main.cpp								*
*                                                                       *
*					Created by: Andrew Loblaw	on September 26, 2011								*
*					Modified on: September 30, 2011																*
*                                                                       *
************************************************************************/
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

/*Disable stricmp warnings*/
#if defined(_MSC_VER)
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE (1)
#endif
#pragma warning(disable : 4996)
#endif

#include "targetver.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
#include <windows.h> //for Sleep()
#include <conio.h>	//for _getch()
#endif

#include <iostream>
#include <math.h>

#pragma comment(lib,"Advapi32.lib")	//For registry query
#pragma comment(lib,"user32.lib") //For MessageBox


