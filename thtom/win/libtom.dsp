# Microsoft Developer Studio Project File - Name="libtom" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libtom - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libtom.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libtom.mak" CFG="libtom - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libtom - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libtom - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "libtom"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libtom - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bin"
# PROP Intermediate_Dir "tmp/libtom"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBTOM_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "$(TCL)/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BUILD_TOM" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib tcl84.lib tk84.lib opengl32.lib glu32.lib /nologo /dll /machine:I386 /libpath:"$(TCL)/lib"

!ELSEIF  "$(CFG)" == "libtom - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libtom___Win32_Debug"
# PROP BASE Intermediate_Dir "libtom___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "bin"
# PROP Intermediate_Dir "tmp/libtom"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBTOM_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "$(TCL)/include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "BUILD_TOM" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib tcl84.lib tk84.lib opengl32.lib glu32.lib /nologo /dll /incremental:no /debug /machine:I386 /pdbtype:sept /libpath:"$(TCL)/lib"

!ENDIF 

# Begin Target

# Name "libtom - Win32 Release"
# Name "libtom - Win32 Debug"
# Begin Source File

SOURCE=..\src\gl.c
# End Source File
# Begin Source File

SOURCE=..\src\glBuffer.c
# End Source File
# Begin Source File

SOURCE=..\src\glColor.c
# End Source File
# Begin Source File

SOURCE=..\src\glData.c
# End Source File
# Begin Source File

SOURCE=..\src\glLight.c
# End Source File
# Begin Source File

SOURCE=..\src\glList.c
# End Source File
# Begin Source File

SOURCE=..\src\glMaterial.c
# End Source File
# Begin Source File

SOURCE=..\src\glMatrix.c
# End Source File
# Begin Source File

SOURCE=..\src\glMesh.c
# End Source File
# Begin Source File

SOURCE=..\src\glName.c
# End Source File
# Begin Source File

SOURCE=..\src\glNormal.c
# End Source File
# Begin Source File

SOURCE=..\src\glRaster.c
# End Source File
# Begin Source File

SOURCE=..\src\glRect.c
# End Source File
# Begin Source File

SOURCE=..\src\glState.c
# End Source File
# Begin Source File

SOURCE=..\src\glTex.c
# End Source File
# Begin Source File

SOURCE=..\src\glu.c
# End Source File
# Begin Source File

SOURCE=..\src\glut.c
# End Source File
# Begin Source File

SOURCE=..\src\glVertex.c
# End Source File
# Begin Source File

SOURCE=..\src\tom.c
# End Source File
# Begin Source File

SOURCE=..\src\tom.h
# End Source File
# End Target
# End Project
