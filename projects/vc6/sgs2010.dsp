# Microsoft Developer Studio Project File - Name="sgs2010" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=sgs2010 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sgs2010.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sgs2010.mak" CFG="sgs2010 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sgs2010 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "sgs2010 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sgs2010 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\obj\vc6\Release"
# PROP Intermediate_Dir "..\..\obj\vc6\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\include" /I "E:\proc\wxWidgets-2.8.12\include" /I "E:\proc\wxWidgets-2.8.12\lib\vc_lib\mswd" /I "E:\proc\lua-5.1\include" /I "E:\proc\tolua++-1.0.93\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "__WXMSW__" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 tolualib.lib LuaStaticLib.lib wxexpat.lib wxbase28.lib wxbase28_net.lib wxbase28_xml.lib wxjpeg.lib wxmsw28_core.lib wxmsw28_adv.lib wxmsw28_aui.lib wxmsw28_html.lib wxpng.lib wxtiff.lib wxregex.lib wxzlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib rpcrt4.lib  /nologo /subsystem:windows /machine:I386 /out:"..\..\bin\sgs2010_vc6.exe" /libpath:"E:\proc\wxWidgets-2.8.12\lib\vc_lib" /libpath:"E:\proc\lua-5.1\lib" /libpath:"E:\proc\tolua++-1.0.93\lib"

!ELSEIF  "$(CFG)" == "sgs2010 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\obj\vc6\Debug"
# PROP Intermediate_Dir "..\..\obj\vc6\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "E:\proc\wxWidgets-2.8.12\include" /I "E:\proc\wxWidgets-2.8.12\lib\vc_lib\mswd" /I "E:\proc\lua-5.1\include" /I "E:\proc\tolua++-1.0.93\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "__WXMSW__" /D "__WXDEBUG__" /FR /YX"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 tolualibd.lib LuaStaticLibd.lib wxexpatd.lib wxbase28d.lib wxbase28d_net.lib wxbase28d_xml.lib wxjpegd.lib wxmsw28d_core.lib wxmsw28d_adv.lib wxmsw28d_aui.lib wxmsw28d_html.lib wxpngd.lib wxtiffd.lib wxregexd.lib wxzlibd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib rpcrt4.lib /nologo /subsystem:windows /debug /machine:I386 /out:"..\..\bin\sgs2010_vc6d.exe" /pdbtype:sept /libpath:"E:\proc\wxWidgets-2.8.12\lib\vc_lib" /libpath:"E:\proc\lua-5.1\lib" /libpath:"E:\proc\tolua++-1.0.93\lib"

!ENDIF 

# Begin Target

# Name "sgs2010 - Win32 Release"
# Name "sgs2010 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\source\Canvas_wxDC.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\Card.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\CardBaseInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\CardRender.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\GameApp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\GameCanvas.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\GameMainFrame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\Imageset.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\res\sgs2010.rc
# End Source File
# Begin Source File

SOURCE=..\..\package\sgs_plugin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\source\stdafx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\source\Canvas.h
# End Source File
# Begin Source File

SOURCE=..\..\source\Canvas_wxDC.h
# End Source File
# Begin Source File

SOURCE=..\..\source\Card.h
# End Source File
# Begin Source File

SOURCE=..\..\source\CardBaseInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\source\CardDef.h
# End Source File
# Begin Source File

SOURCE=..\..\source\CardRender.h
# End Source File
# Begin Source File

SOURCE=..\..\source\GameApp.h
# End Source File
# Begin Source File

SOURCE=..\..\source\GameCanvas.h
# End Source File
# Begin Source File

SOURCE=..\..\source\GameMainFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\source\Imageset.h
# End Source File
# Begin Source File

SOURCE=..\..\package\sgs_plugin.h
# End Source File
# Begin Source File

SOURCE=..\..\source\stdafx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Interface"

# PROP Default_Filter "itf"
# Begin Source File

SOURCE=..\..\include\IModule.h
# End Source File
# Begin Source File

SOURCE=..\..\include\IRender.h
# End Source File
# Begin Source File

SOURCE=..\..\include\IScript.h
# End Source File
# End Group
# Begin Group "Package"

# PROP Default_Filter "pkg"
# Begin Source File

SOURCE=..\..\package\sgs_plugin.pkg

!IF  "$(CFG)" == "sgs2010 - Win32 Release"

!ELSEIF  "$(CFG)" == "sgs2010 - Win32 Debug"

# Begin Custom Build
InputDir=\proc\krh\sgs2010\trunk\package
InputPath=..\..\package\sgs_plugin.pkg

BuildCmds= \
	cd /d $(InputDir) \
	set toluapp="e:\proc\tolua++-1.0.93\bin\toluapp.exe" \
	%toluapp% -n sgs -o sgs_plugin.cpp -H sgs_plugin.h sgs_plugin.pkg \
	

"..\..\package\sgs_plugin.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\package\sgs_plugin.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
