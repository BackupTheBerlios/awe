# Microsoft Developer Studio Project File - Name="awe" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=awe - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "awe.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "awe.mak" CFG="awe - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "awe - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "awe - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "awe - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x408 /d "NDEBUG"
# ADD RSC /l 0x408 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "awe - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../include" /I "../../../../include" /D "DEBUGMODE" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x408 /d "_DEBUG"
# ADD RSC /l 0x408 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 alleg.lib alfontdll.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "awe - Win32 Release"
# Name "awe - Win32 Debug"
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\com.h
# End Source File
# Begin Source File

SOURCE=..\..\include\draganddrop.h
# End Source File
# Begin Source File

SOURCE=..\..\include\event.h
# End Source File
# Begin Source File

SOURCE=..\..\include\font.h
# End Source File
# Begin Source File

SOURCE=..\..\include\gdi.h
# End Source File
# Begin Source File

SOURCE=..\..\include\geomman.h
# End Source File
# Begin Source File

SOURCE=..\..\include\input.h
# End Source File
# Begin Source File

SOURCE=..\..\include\linkedlist.h
# End Source File
# Begin Source File

SOURCE=..\..\include\mouse.h
# End Source File
# Begin Source File

SOURCE=..\..\include\rect.h
# End Source File
# Begin Source File

SOURCE=..\..\include\skin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\symbintree.h
# End Source File
# Begin Source File

SOURCE=..\..\include\widget.h
# End Source File
# End Group
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\com.c
# End Source File
# Begin Source File

SOURCE=..\..\src\draganddrop.c
# End Source File
# Begin Source File

SOURCE=..\..\src\event.c
# End Source File
# Begin Source File

SOURCE=..\..\src\font.c
# End Source File
# Begin Source File

SOURCE=..\..\src\gdi.c
# End Source File
# Begin Source File

SOURCE=..\..\src\geomman.c
# End Source File
# Begin Source File

SOURCE=..\..\src\input.c
# End Source File
# Begin Source File

SOURCE=..\..\src\input_win32.c
# End Source File
# Begin Source File

SOURCE=..\..\src\linkedlist.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mouse.c
# End Source File
# Begin Source File

SOURCE=..\..\src\skin.c
# End Source File
# Begin Source File

SOURCE=..\..\src\symbintree.c
# End Source File
# Begin Source File

SOURCE=..\..\src\widget.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\main.c
# End Source File
# End Target
# End Project
