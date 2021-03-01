# Microsoft Developer Studio Project File - Name="SKYSIPUA" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SKYSIPUA - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SKYSIPUA.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SKYSIPUA.mak" CFG="SKYSIPUA - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SKYSIPUA - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SKYSIPUA - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SKYSIPUA - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SKYSIPUA_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SKYSIPUA_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "SKYSIPUA - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SKYSIPUA_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../sky_common_lib/inc" /I "../../../sip_parse/inc" /I "../../../skyregister/inc" /I "../../../interface/inc" /I "../../../sky_conf_lib/inc" /I "./" /I "../../../skyproxylib/inc" /I "../../../hash_library/inc" /I "../../../SipUaLib/inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SKYSIPUA_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib sipparse.lib skylibc.lib WS2_32.LIB hash_library.lib /nologo /dll /debug /machine:I386 /out:"../../../bin/debug/SKYSIPUA.dll" /implib:"../../../bin/debug/lib/SKYSIPUA.lib" /pdbtype:sept /libpath:"../../../bin/debug/lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "SKYSIPUA - Win32 Release"
# Name "SKYSIPUA - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\SipUaLib\src\cProfile.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\src\sipuac.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\src\sipuas.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\src\sky_nonce.c
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\src\SkyAsyncSock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\src\SkyRtp.cpp
# End Source File
# Begin Source File

SOURCE=.\SKYSIPUA.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\src\SkySipUaManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\src\SkySock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\src\SkyTimer.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\src\vmd5.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\SipUaLib\inc\cProfile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\inc\sipuac.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\inc\sipuas.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\inc\sky_nonce.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\inc\sky_sip_lib.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\inc\SkyAsyncSock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\inc\SkyRtp.h
# End Source File
# Begin Source File

SOURCE=.\SKYSIPUA.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\inc\skysipua_g.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\inc\SkySipUaManager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\inc\SkySock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\inc\SkyTimer.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\SipUaLib\inc\vmd5.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
