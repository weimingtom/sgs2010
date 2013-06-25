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
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\..\bin/logic_d.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "logic - Win32 Release"
# Name "logic - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "card_impl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\card_impl\card_aggression.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_allattack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_attack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_axeguanshi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_bagua.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_bowqiling.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_chitu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_dawan.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_defend.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_dilu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_duel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_foison.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_genwithair.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_halberdfangtian.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_happy.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_jueying.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_kickladder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_liannu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_lightning.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_makebrother.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_medicine.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_otherattack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_sheildrenwang.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_snitch.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_spearzhangba.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_swordchixiong.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_swordhanbing.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_swordqinghong.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_swordqinglong.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_unassailable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_zhuahuang.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\card_impl\card_zixing.cpp
# End Source File
# End Group
# Begin Group "hero_impl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\hero_impl\hero_cacao.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\hero_impl\hero_guanyu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\hero_impl\hero_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\hero_impl\hero_liubei.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\hero_impl\hero_luxun.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\hero_impl\hero_machao.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\hero_impl\hero_sunquan.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\hero_impl\hero_sunshangxiang.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\hero_impl\hero_xiahoudun.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\hero_impl\hero_zhangfei.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\hero_impl\hero_zhangliao.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\hero_impl\hero_zhaoyu.cpp
# End Source File
# End Group
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

SOURCE=..\..\src\main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\out.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\out.h
# End Source File
# Begin Source File

SOURCE=..\..\src\player.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\player.h
# End Source File
# Begin Source File

SOURCE=..\..\src\skill.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\skill.h
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "res"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
