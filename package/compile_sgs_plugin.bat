cd /d %~dp0
set toluapp="e:\proc\tolua++-1.0.93\bin\toluapp.exe"
%toluapp% -n sgs -o sgs_plugin.cpp -H sgs_plugin.h sgs_plugin.pkg
