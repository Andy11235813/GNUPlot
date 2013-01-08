GNUPlot
=======

GNUPlot class source code for C++ real-time plotting

The C++ GNUplot Interface is provided by:
Andrew Loblaw
You are free to use, modify, redistribute and do as you wish with the code. It
is provided AS IS, without any liability or guarantee to operate properly.
Please email me at aloblaw@gmail.com if you have any questions or if you 
have made improvements or additions to the code. 

Enjoy. :)


---Overview of GNUplotInterface---
First you must obviously have gnuplot installed on your computer. It is
available at: www.gnuplot.info.
To make sure GNUplotInterface works properly, make sure gnuplot is placed 
in your C:\ drive. If you have gnuplot installed on another drive, then make
sure to modify the global variable "GNUPLOTEXE" at the top of GNUplot.cpp so
gnuplot can be found by GNUplotInterface. 

---Users with Visual Studio 2010---
The only requirements are that you have recompiled the project for the machine
you are working on. The project file should have 4 different configurations 
available: Debug, Release, DebugLibrary and Library. 
Debug and Release compile a main function for testing of GNUplot functionality;
DebugLibrary and Library create a .lib files which can be linked against to
use in a separate project. (DebugLibrary is the debug version of the library)
---Users not using Visual Studio 2010---
The project file and options file (GNUplot.sln and GNUplot.suo) are saved for
Visual Studio 2010. There is nothing special about the solution, it is just 
added so that you can immediately compile everything as I intended it. If you 
don't have Visual Studio 2010 you will have to add the .cpp and .h files 
manually to a new project yourself.

Please see the "ReadMe.txt" in the internal folder for a brief overview of the 
functionality. Or just compile and run the sample main program.


