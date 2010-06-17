# Microsoft Developer Studio Project File - Name="soldat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=soldat - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "soldat.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "soldat.mak" CFG="soldat - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "soldat - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "soldat - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "soldat - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib glut32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "soldat - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib glu32.lib glut32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "soldat - Win32 Release"
# Name "soldat - Win32 Debug"
# Begin Group "physics"

# PROP Default_Filter ""
# Begin Group "bodies"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\source\Body.cpp
# End Source File
# Begin Source File

SOURCE=.\source\Body.h
# End Source File
# Begin Source File

SOURCE=.\source\BodyList.cpp
# End Source File
# Begin Source File

SOURCE=.\source\BodyList.h
# End Source File
# Begin Source File

SOURCE=.\source\Box.cpp
# End Source File
# Begin Source File

SOURCE=.\source\Box.h
# End Source File
# Begin Source File

SOURCE=.\source\Chain.cpp
# End Source File
# Begin Source File

SOURCE=.\source\Chain.h
# End Source File
# Begin Source File

SOURCE=.\source\RigidBody.cpp
# End Source File
# Begin Source File

SOURCE=.\source\RigidBody.h
# End Source File
# Begin Source File

SOURCE=.\source\SoftBody.cpp
# End Source File
# Begin Source File

SOURCE=.\source\SoftBody.h
# End Source File
# End Group
# Begin Group "mesh"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\source\Mesh.cpp
# End Source File
# Begin Source File

SOURCE=.\source\Mesh.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\source\LinConstraint.cpp
# End Source File
# Begin Source File

SOURCE=.\source\LinConstraint.h
# End Source File
# Begin Source File

SOURCE=.\source\Particle.cpp
# End Source File
# Begin Source File

SOURCE=.\source\Particle.h
# End Source File
# End Group
# Begin Group "maths"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\source\Vector.cpp
# End Source File
# Begin Source File

SOURCE=.\source\Vector.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\source\GameCode.cpp
# End Source File
# Begin Source File

SOURCE=.\source\main.cpp
# End Source File
# End Target
# End Project
