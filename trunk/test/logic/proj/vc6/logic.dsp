# Microsoft Developer Studio Project File - Name="logic" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=logic - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "logic.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "logic.mak" CFG="logic - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "logic - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "logic - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "logic - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "..\..\obj\release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../../3p/tolua++-1.0.93/include" /I "../../../../3p/lua-5.1/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib tolualib.lib LuaStaticLib.lib /nologo /subsystem:console /machine:I386 /libpath:"../../../../3p/tolua++-1.0.93/lib" /libpath:"../../../../3p/lua-5.1/lib"

!ELSEIF  "$(CFG)" == "logic - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "..\..\obj\debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../../3p/tolua++-1.0.93/include" /I "../../../../3p/lua-5.1/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib tolualibd.lib LuaStaticLibd.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\..\bin/logic_d.exe" /pdbtype:sept /libpath:"../../../../3p/tolua++-1.0.93/lib" /libpath:"../../../../3p/lua-5.1/lib"

!ENDIF 

# Begin Target

# Name "logic - Win32 Release"
# Name "logic - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\card.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card.h
# End Source File
# Begin Source File

SOURCE=..\..\src\card_stack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_stack.h
# End Source File
# Begin Source File

SOURCE=..\..\src\cmd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\cmd.h
# End Source File
# Begin Source File

SOURCE=..\..\src\comm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\comm.h
# End Source File
# Begin Source File

SOURCE=..\..\src\config.h
# End Source File
# Begin Source File

SOURCE=..\..\src\decide.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\decide.h
# End Source File
# Begin Source File

SOURCE=..\..\src\discard.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\discard.h
# End Source File
# Begin Source File

SOURCE=..\..\src\equip.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\equip.h
# End Source File
# Begin Source File

SOURCE=..\..\src\event.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\event.h
# End Source File
# Begin Source File

SOURCE=..\..\src\game.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\game.h
# End Source File
# Begin Source File

SOURCE=..\..\src\get.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\get.h
# End Source File
# Begin Source File

SOURCE=..\..\src\hero.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\hero.h
# End Source File
# Begin Source File

SOURCE=..\..\src\info.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\info.h
# End Source File
# Begin Source File

SOURCE=..\..\src\judge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\judge.h
# End Source File
# Begin Source File

SOURCE=..\..\src\life.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\life.h
# End Source File
# Begin Source File

SOURCE=..\..\src\main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\out.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\out.h
# End Source File
# Begin Source File

SOURCE=..\..\src\platform.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\platform.h
# End Source File
# Begin Source File

SOURCE=..\..\src\player.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\player.h
# End Source File
# Begin Source File

SOURCE=..\..\src\script.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\script.h
# End Source File
# Begin Source File

SOURCE=..\..\src\select.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\select.h
# End Source File
# Begin Source File

SOURCE=..\..\src\skill.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\skill.h
# End Source File
# Begin Source File

SOURCE=..\..\src\stack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\stack.h
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "res"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "pkg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\pkg\lua_export.cpp
# End Source File
# Begin Source File

SOURCE=..\..\pkg\lua_export.h
# End Source File
# Begin Source File

SOURCE=..\..\pkg\lua_export.pkg

!IF  "$(CFG)" == "logic - Win32 Release"

!ELSEIF  "$(CFG)" == "logic - Win32 Debug"

USERDEP__LUA_E="..\..\src\card.h"	"..\..\src\card_stack.h"	"..\..\src\cmd.h"	"..\..\src\comm.h"	"..\..\src\config.h"	"..\..\src\decide.h"	"..\..\src\discard.h"	"..\..\src\equip.h"	"..\..\src\event.h"	"..\..\src\game.h"	"..\..\src\get.h"	"..\..\src\hero.h"	"..\..\src\info.h"	"..\..\src\judge.h"	"..\..\src\life.h"	"..\..\src\out.h"	"..\..\src\platform.h"	"..\..\src\player.h"	"..\..\src\script.h"	"..\..\src\select.h"	"..\..\src\skill.h"	"..\..\src\stack.h"	
# Begin Custom Build
InputPath=..\..\pkg\lua_export.pkg

BuildCmds= \
	set DD=%cd% \
	echo %DD% \
	cd  ..\..\pkg \
	"..\..\..\3p\tolua++-1.0.93/bin/toluapp.exe" -o "lua_export.cpp" -H "lua_export.h" -n game  lua_export.pkg \
	cd %DD% \
	

"..\..\pkg\lua_export.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\pkg\lua_export.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
