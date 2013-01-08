========================================================================
    CONSOLE APPLICATION : GNUplot Project Overview
========================================================================

AppWizard has created this GNUplot application for you.

This file contains a summary of what you will find in each of the files that
make up your GNUplot application.


GNUplot.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

GNUplot.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

GNUplot.cpp
    This is the main application source file.
	A.L.:	The StartProcess(...) function controls how the window opens, the size, and 
			other parameters.
			The Init(...) function controls how the program initializes (real-time plotting, 
			window title, and sets terminal for windows machines). *Note, it can be modified
			for linux or mac machines, however this has not been tested.
			The Write(...) function can be used to modify the operation of GNUplot. Things
			dashed lines, x-axis, y-axis, and other plotting parameters should be written
			using this function, just as you would use GNUplot in a command line.
			The Plot(...) functions are used to plot the data of interest. Multiple overloaded
			plot functions are provided for different input types. If providing an array as an 
			argument, it is up to the user to make sure the array is the proper length and the
			length is provided as an argument.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named GNUplot.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
