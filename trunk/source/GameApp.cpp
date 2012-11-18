/**
 * GameApp.cpp : implementing file of Game Application
 * 
 *
*/

#include "stdafx.h"
#include "GameApp.h"
#include "GameMainFrame.h"
#include "../package/sgs_plugin.h"

IMPLEMENT_APP(sgs::GameApp)

namespace sgs {

	// implements


	bool GameApp::OnInit()
	{
		// create main window
		GameMainFrame* pFrame = new GameMainFrame(_("sgs2010"), wxDefaultPosition, wxSize(800,600));
		pFrame->Show(true);
		SetTopWindow(pFrame);

		m_L = lua_open();
		wxASSERT(m_L);
		if(m_L == NULL)
		{
			return false;
		}

		tolua_sgs_open(m_L);

		return true;
	}

	int GameApp::OnExit()
	{
		if(m_L)
			lua_close(m_L);
		return 0;
	}

}


