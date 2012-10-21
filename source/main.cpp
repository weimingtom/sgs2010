#include "stdafx.h"
#include "../include/resource.h"


namespace sgs
{

	class GameApp : public wxApp
	{

	public:
		virtual bool OnInit();

		virtual int OnExit();
	};


	class GameCanvas;

	class GameMainFrame : public wxFrame
	{

	public:
		// constructor
		GameMainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);


		// event handles
		void OnClose(wxCloseEvent& event);
	private:
		GameCanvas*  m_canvas;

		DECLARE_EVENT_TABLE()
	};


	class GameCanvas : public wxPanel
	{
	public:
		GameCanvas(GameMainFrame* parent);
		

		void OnPaint(wxPaintEvent& event);

		virtual void OnDraw(wxDC&  dc);

	private:
		DECLARE_EVENT_TABLE()
	};




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


	BEGIN_EVENT_TABLE(GameCanvas,wxPanel)
		EVT_PAINT(OnPaint)
	END_EVENT_TABLE()

	
	GameCanvas::GameCanvas(GameMainFrame* parent)
		: wxPanel(parent)
	{

	}

	void GameCanvas::OnPaint(wxPaintEvent& event)
	{
		wxPaintDC  paintDC(this);

#if wxUSE_GRAPHICS_CONTEXT
		wxGCDC  gdc(paintDC);
		wxDC& dc = gdc;
#else
		wxDC& dc = paintDC;
#endif
		PrepareDC(dc);

		OnDraw(dc);
	}


	void GameCanvas::OnDraw(wxDC& dc)
	{
		dc.SetBackgroundMode(wxTRANSPARENT);
		dc.SetBackground(*wxBLACK_BRUSH);
		dc.SetTextForeground(*wxRED);

		dc.DrawText(_("This is a test string!!!"), 10,10);

		wxBitmap*  image = new wxBitmap()
	}

}















/*
static BOOL CALLBACK MainDialogCallback(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

int CALLBACK WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, MainDialogCallback);

	return 0;

}


static BOOL CALLBACK MainDialogCallback(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{

	case WM_COMMAND:
		if(LOWORD(wParam) == IDOK)
		{		
			EndDialog(hDlg, IDOK);
			return TRUE;
		}
		else if(LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
	
		break;
	}
	return FALSE;
}
*/




