# Microsoft Developer Studio Project File - Name="sky_common_lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=sky_common_lib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sky_common_lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sky_common_lib.mak" CFG="sky_common_lib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sky_common_lib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "sky_common_lib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sky_common_lib - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../sky_common_lib/inc" /I "./" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /D MSVC_VER=6 /D "MEM_DBG" /YX /FD /TP /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../bin/release/skylibc.lib"

!ELSEIF  "$(CFG)" == "sky_common_lib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /MDd /W3 /GX /ZI /Od /I "../../../sky_common_lib/inc" /I "./" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D MSVC_VER=6 /D "MEM_DBG" /YX /FD /GZ /TP /c
# SUBTRACT CPP /nologo
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../bin/debug/lib/skylibc.lib"

!ENDIF 

# Begin Target

# Name "sky_common_lib - Win32 Release"
# Name "sky_common_lib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\sky_common_lib\src\sky_debug.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\src\sky_file.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\src\sky_hash_4.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\src\sky_lock.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\src\sky_pstring.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\src\sky_sleep.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\src\sky_sock.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\src\sky_string.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\src\sky_thread.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\src\strptime.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\src\WinLock.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\sky_common_lib\inc\sky_call_macro.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\inc\sky_debug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\inc\sky_def.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\inc\sky_file.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\inc\sky_hash_4.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\inc\sky_lock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\inc\sky_macro.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\inc\sky_pstring.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\inc\sky_sleep.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\inc\sky_sock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\inc\sky_string.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\inc\sky_thread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\inc\skytool.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\inc\strptime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sky_common_lib\inc\WinLock.h
# End Source File
# End Group
# End Target
# End Project
