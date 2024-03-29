//#define  WIN32_LEAN_AND_MEAN
//#include <windows.h>


#ifndef __STDAFX_H__
#define __STDAFX_H__


// compiler detection; includes setup.h
#include <wx/defs.h>

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/dcbuffer.h>
#include <wx/xml/xml.h>


//  for lua script

#ifdef __cplusplus
extern "C" {
#endif
	
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
	
#ifdef __cplusplus
};
#endif


#include <tolua++.h>


#endif /* __STDAFX_H__ */
