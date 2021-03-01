# Microsoft Developer Studio Project File - Name="sip_parse_lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=sip_parse_lib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sip_parse_lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sip_parse_lib.mak" CFG="sip_parse_lib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sip_parse_lib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "sip_parse_lib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sip_parse_lib - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../sky_common_lib/inc" /I "../../../sip_parse/inc" /I "./" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_WINDOWS" /D MSVC_VER=6 /D "MEM_DBG" /YX /FD /TP /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../bin/release/sipparse.lib"

!ELSEIF  "$(CFG)" == "sip_parse_lib - Win32 Debug"

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
# ADD CPP /MDd /W3 /vmg /GR /ZI /Od /I "../../../sky_common_lib/inc" /I "../../../sip_parse/inc" /I "./" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D MSVC_VER=6 /D "MEM_DBG" /YX /FD /TP /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../bin/debug/lib/sipparse.lib"

!ENDIF 

# Begin Target

# Name "sip_parse_lib - Win32 Release"
# Name "sip_parse_lib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\sip_parse\src\debug.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_allow.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_answer_code.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_authentication.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_body.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_callid.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_contact.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_cseq.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_field.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_info_display.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_make_msg.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_method.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_parse.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_parse_prehead.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_pre_head.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_record_route.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_sdp_parse.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_to_from.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_tool.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_uri.c
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\src\sip_via.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_allow.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_answer_code.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_authentication.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_body.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_callid.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_contact.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_cseq.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_field.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_header.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_info_display.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_make_msg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_method.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_parse.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_pre_head.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_record_route.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_sdp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_sdp_parse.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_to_from.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_tool.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_uri.h
# End Source File
# Begin Source File

SOURCE=..\..\..\sip_parse\inc\sip_via.h
# End Source File
# End Group
# End Target
# End Project
