/**
 * GameApp.cpp : implementing file of Game Application
 * 
 *
*/

#include "stdafx.h"
#include "GameApp.h"
#include "GameMainFrame.h"

namespace sgs {

	// implements

	IMPLEMENT_APP(GameApp)


	bool GameApp::OnInit()
	{
		// create main window
		GameMainFrame* pFrame = new GameMainFrame(_("sgs2010"), wxDefaultPosition, wxSize(800,600));
		pFrame->Show(true);
		SetTopWindow(pFrame);


		return true;
	}

	int GameApp::OnExit()
	{

		return 0;
	}

}


