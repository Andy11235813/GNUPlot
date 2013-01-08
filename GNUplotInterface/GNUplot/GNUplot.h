/************************************************************************
*																																				*
*					GNUplot.h -- Header file for gnuplot interface class					*
*                                                                       *
*					Created by: Andrew Loblaw	on September 26, 2011								*
*					Modified on: October 2, 2011																*
*                                                                       *
************************************************************************/
#pragma once

#include <windows.h>	// Required for a lot of type definitions
#include <iostream>		// For std:: in Plot(...)
#include <string>			// For string in Plot(...)
#include <vector>			// Required for plotting array of vectors
#include <strsafe.h>	// For MessageBox(...)

#define BUFSIZE 4096 

class GNUPlotInterface
{
public:
	// Initialization functions
	void CreatePipes(void);
	void StartProcess(void);
	std::string Write(const CHAR *);
	std::string Init(const CHAR *linetype,const CHAR *title);
	// Deallocation function
	std::string CloseAll(void);

	// Plotting functions
	std::string Plot(const CHAR *xydata, int arg=0);	//Most efficient plotting function (string data)
	std::string Plot(std::vector<double> x_y[], int dims, int length);		//Array of vectors data ([x][y])
	std::string Plot(double x[], double y[], int length);	//Matlab familiar plotting (double x, double y)
	std::string Plot(double y[], int length);	//Matlab familiar plotting (double y) with index based x-axis

	// Error handling function
	void ErrorExit(PTSTR);

private:

	// Private pipe handle variables
	HANDLE m_hGNUPlotStd_IN_Rd;	//GNUplot input read handle
	HANDLE m_hGNUPlotStd_IN_Wr;	//GNUplot input write handle (for parent)
	HANDLE m_hGNUPlotStd_OUT_Rd;//GNUplot output read handle (for parent)
	HANDLE m_hGNUPlotStd_OUT_Wr;//GNUplot output write handle

	// Private process information struct
	PROCESS_INFORMATION m_ProcInfo; 

};