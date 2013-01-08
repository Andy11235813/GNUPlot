/************************************************************************
*																																				*
*					main.cpp -- Main file for testing the gnuplot interface				*
*                                                                       *
*					Created by: Andrew Loblaw	on September 26, 2011								*
*					Modified on: October 2, 2011																*
*                                                                       *
************************************************************************/
/* This main is meant entirely to test an initialization of a GNUplotInterface
*	class object and to test all the functionality. */

#include "stdafx.h"
#include "GNUplot.h"

long long g_clockFrequency;
/* A run once routine to obtain the processor clock speed. This function
*	returns non-zero if it is successful */
inline DWORD getCpuClockFrequencyMHz(void){
	DWORD BufSize = _MAX_PATH;
	DWORD dwMHz = _MAX_PATH;
	HKEY hKey;
	long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE,	//Section of registry where processor clock speed located
		TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"),	//location of processor clock speed value
		0,	//must be 0
		KEY_READ,	//Read only (to read the value of the key)
		&hKey	//store the registry key handle here
		);
	if(lError != ERROR_SUCCESS)
	{// if the key is not found, tell the user why:
		TCHAR Buffer[_MAX_PATH] = TEXT("Could not find processor clock-speed registry key\n");
		MessageBox(NULL, Buffer, TEXT("Error"), MB_OK);
		return(0);	//Error return
	}

	// query the key
	RegQueryValueEx(hKey, TEXT("~MHz"), NULL, NULL, (LPBYTE) &dwMHz, &BufSize);
	return dwMHz;
}
inline long long GetCpuClocks(void)
{
	// Counter
	struct { int low, high; } counter;

	// Use RDTSC instruction to get clocks count
	__asm push EAX
	__asm push EDX
	__asm __emit 0fh __asm __emit 031h // RDTSC
	__asm mov counter.low, EAX
	__asm mov counter.high, EDX
	__asm pop EDX
	__asm pop EAX

	// Return result
	return *(__int64 *)(&counter);
}
inline void pressEnter(void){
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
	std::cout<<"Press any key to continue..."<<std::endl;
	_getch();
#endif
}

int main(int argc, char* argv[])
{
	g_clockFrequency = getCpuClockFrequencyMHz()*(1000000);
	long long startTime, endTime;

	/* Create gnuplotinterface class object */
	GNUPlotInterface gpi;

	/* Create the read/write pipes */
	gpi.CreatePipes();
	/* Start the gnuplot.exe process with rerouted input/output using pipes */
	gpi.StartProcess();
	/* This is the intialization of realtime plotting as well as title and 
	*	line type. Be sure to use \" to send the " character. */
	std::string gnuplotOutput = gpi.Init("lines","GNUplot Pipe Interface Test");
	std::cout<<"Initialization - gnuplot output:\n"<<gnuplotOutput.c_str()<<std::endl;
	Sleep(1000);	//Just to slow it down so user can read message

	std::cout<<std::endl<<"Test1: simple plotting [y=x^2]."<<std::endl;
	startTime = GetCpuClocks();
	gnuplotOutput = gpi.Plot("1 1\n2 4\n3 9\n4 16\n5 25\n6 36\n7 49\n8 64\n9 81\n10 100\n",0);
	endTime = GetCpuClocks();
	std::cout<<"Test1 - gnuplot output:\n"<<gnuplotOutput.c_str()<<std::endl;
	std::cout<<"Test1 time taken = "<<((double)endTime-startTime)/g_clockFrequency<<std::endl;
	pressEnter();

	std::cout<<std::endl<<"Test2: vector of doubles data plot [y=sqrt(x)]."<<std::endl;
	const int dims = 2;
	const int length = 200;
	std::vector<double> values[dims];
	values[0].resize(length);
	values[1].resize(length);
	for(int i = 0; i<length; i++){
		values[0][i] = (double) i;
		values[1][i] = sqrt((double) i);
	}
	startTime = GetCpuClocks();
	gnuplotOutput = gpi.Plot(values,dims,length);
	endTime = GetCpuClocks();
	std::cout<<"Test2 - gnuplot output:\n"<<gnuplotOutput.c_str()<<std::endl;
	std::cout<<"Test2 time taken = "<<((double)endTime-startTime)/g_clockFrequency<<std::endl;
	pressEnter();

	/*RTLength: Real-time data length - this value used for similar comparison of 
	*	different real-time plot methods */
	const int RTLength = 300; 
	std::cout<<std::endl<<"Test3: \"realtime\" fast char array data plot [y=sin(sqrt(x))]."<<std::endl;
	std::cout<<"Test3 - gnuplot output:"<<std::endl;
	std::string input;
	long long diffTime = 0;
	for(int i = 0; i<RTLength; i++){
		double dVal = sin(sqrt((double) i));
		Sleep(30);
		CHAR chVal[20];
		sprintf(chVal,"%d",i); input.append(chVal); input.append(" ");
		sprintf(chVal,"%.5f",dVal);	input.append(chVal); input.append("\n");
		startTime = GetCpuClocks();
		gnuplotOutput = gpi.Plot(input.c_str(),0);
		endTime = GetCpuClocks();
		diffTime += (endTime - startTime);
		if(gnuplotOutput.length() > 0)
			std::cout<<gnuplotOutput.c_str()<<std::endl;
	}
	double ddiffTime = (double) diffTime;
	std::cout<<"Test3 time taken = "<<ddiffTime/(double)g_clockFrequency<<std::endl;
	pressEnter();

	std::cout<<std::endl<<"Test4: \"realtime\" vector of doubles data plot [y=sin(sqrt(x))]."<<std::endl;
	std::cout<<"Test4 - gnuplot output:"<<std::endl;
	input.clear();
	diffTime = 0;
	std::vector<double> dVals[2];
	dVals[0].resize(RTLength);
	dVals[1].resize(RTLength);
	for(int i = 0; i<RTLength; i++){
		dVals[0][i] = (double) i;
		dVals[1][i] = sin(sqrt((double) i));
		startTime = GetCpuClocks();
		gnuplotOutput = gpi.Plot(dVals,2,i);
		endTime = GetCpuClocks();
		diffTime += (endTime - startTime);
		if(gnuplotOutput.length() > 0)
			std::cout<<gnuplotOutput.c_str()<<std::endl;
	}
	ddiffTime = (double) diffTime;
	std::cout<<"Test4 time taken = "<<ddiffTime/g_clockFrequency<<std::endl;
	std::cout<<std::endl<<"*** end of gnuplot tests ***"<<std::endl;
	pressEnter();

	std::cout<<std::endl<<"Test5: \"realtime\" x-array/y-array data plot [y=sin(sqrt(x))]."<<std::endl;
	std::cout<<"Test5 - gnuplot output:"<<std::endl;
	input.clear();
	diffTime = 0;
	double xVal[RTLength];
	double yVal[RTLength];
	for(int i = 0; i<RTLength; i++){
		xVal[i] = (double) i;
		yVal[i] = sin(sqrt((double) i));
		startTime = GetCpuClocks();
		gnuplotOutput = gpi.Plot(xVal,yVal,i);
		endTime = GetCpuClocks();
		diffTime += (endTime - startTime);
		if(gnuplotOutput.length() > 0)
			std::cout<<gnuplotOutput.c_str()<<std::endl;
	}
	ddiffTime = (double) diffTime;
	std::cout<<"Test5 time taken = "<<ddiffTime/g_clockFrequency<<std::endl;
	pressEnter();

	std::cout<<std::endl<<"Test6: \"realtime\" y-array (no x-array) data plot [y=sin(sqrt(x))]."<<std::endl;
	std::cout<<"Test6 - gnuplot output:"<<std::endl;
	input.clear();
	diffTime = 0;
	for(int i = 0; i<RTLength; i++){
		yVal[i] = sin(sqrt((double) i));
		startTime = GetCpuClocks();
		gnuplotOutput = gpi.Plot(yVal,i);
		endTime = GetCpuClocks();
		diffTime += (endTime - startTime);
		if(gnuplotOutput.length() > 0)
			std::cout<<gnuplotOutput.c_str()<<std::endl;
	}
	ddiffTime = (double) diffTime;
	std::cout<<"Test6 time taken = "<<ddiffTime/g_clockFrequency<<std::endl;

	std::cout<<std::endl<<"*** end of gnuplot tests ***"<<std::endl;
	pressEnter();

	/* Close all the handles to pipes, process and thread as well as sending 
	*	"quit" command to gnuplot.exe */
	gpi.CloseAll();
	return 0;
}