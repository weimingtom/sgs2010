/**
 * GameMainFrame.cpp : implementing file of Game Main Frame window
 * 
 *
*/

#include "stdafx.h"
#include "GameMainFrame.h"
#include "GameApp.h"
#include "GameCanvas.h"

namespace sgs {
	BEGIN_EVENT_TABLE(GameMainFrame, wxFrame)
		EVT_CLOSE(OnClose)
	END_EVENT_TABLE()


	GameMainFrame::GameMainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
		:wxFrame((wxFrame*)NULL, wxID_ANY, title, pos,  size, wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE) 
	{
		// main icon
		// SetIcon(wxICON());
		m_canvas = new GameCanvas(this);

	}

	void GameMainFrame::OnClose(wxCloseEvent& event)
	{
		Close();
	}


}
