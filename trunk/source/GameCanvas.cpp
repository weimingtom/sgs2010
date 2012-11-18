/**
 * GameCanvas.cpp : implementing file of Game display canvas
 * 
 *
*/

#include "stdafx.h"
#include "GameCanvas.h"
#include "GameApp.h"
#include "GameMainFrame.h"

namespace sgs {

	// event table of GameCanvas
	BEGIN_EVENT_TABLE(GameCanvas,wxPanel)
		EVT_PAINT(GameCanvas::OnPaint)
		EVT_SIZE(GameCanvas::OnSize)
		EVT_TIMER(ID_TIMER_REFRESH, GameCanvas::OnTimer)
	END_EVENT_TABLE()

	
	GameCanvas::GameCanvas(GameMainFrame* parent)
		: wxPanel(parent), m_timer(this, ID_TIMER_REFRESH)
	{
		wxFileName  appPath(wxStandardPaths().GetExecutablePath());
		appPath.AppendDir(wxT("data"));
		appPath.SetFullName(wxT("card.bmp"));
		//wxFileName  bmpPath(appPath.GetPath(),  _("data/card.bmp"), wxPATH_UNIX);

		m_bitmap = new wxBitmap;
		m_bitmap->LoadFile(appPath.GetFullPath(), wxBITMAP_TYPE_BMP);

		m_bitmap->SetMask(new wxMask(*m_bitmap, wxColour(255,0,255)));

		// for double buffer paint
		SetBackgroundStyle(wxBG_STYLE_CUSTOM);

		m_timer.Start(40);
		m_step = 0;
		m_flag = 1;
	}

	GameCanvas::~GameCanvas()
	{
		delete m_bitmap;
	}

	void GameCanvas::OnPaint(wxPaintEvent& event)
	{
		wxAutoBufferedPaintDC  paintDC(this);

#if wxUSE_GRAPHICS_CONTEXT
		wxGCDC  gdc(paintDC);
		wxDC& dc = gdc;
#else
		wxDC& dc = paintDC;
#endif
		PrepareDC(dc);

		OnDraw(dc);
	}

	void GameCanvas::OnSize(wxSizeEvent& event)
	{
		Refresh();
	}

	void GameCanvas::OnDraw(wxDC& dc)
	{
		dc.SetBackgroundMode(wxTRANSPARENT);
		dc.SetBackground(*wxBLACK_BRUSH);
		dc.SetTextForeground(*wxRED);

		
		wxSize size = GetClientSize();
		wxRect rcClient(wxPoint(0,0), size);

		//dc.GradientFillConcentric(rcClient, wxColour(14,148,81), wxColour(0,117,58));
		wxBrush(wxColour(0,117,58),wxSOLID);
		dc.SetPen(wxNullPen);
		dc.SetBrush(wxBrush(wxColour(0,117,58),wxSOLID));

		dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

		wxString  text(_("This is a test string!!!"));
		dc.DrawText(text, m_step,10);
		wxSize szText = dc.GetTextExtent(text);

		if(szText.GetWidth() + m_step >= size.GetWidth())
			m_flag = -1;
		else if(m_flag < 0 && m_step <= 0)
			m_flag = 1;


		wxMemoryDC  memDC;
		memDC.SelectObject(*m_bitmap);

		dc.Blit(10, 100, 61, 79, &memDC, 0, 0, wxCOPY, true, -1, -1);
		dc.Blit(12, 104, 13, 14, &memDC, 126, 317, wxCOPY, true, -1, -1);
		dc.Blit(13, 118, 11, 11, &memDC, 36, 347, wxCOPY, true, -1, -1);

		//dc.DrawBitmap(*m_bitmap, 10, 100, true);

	}


	void  GameCanvas::OnTimer(wxTimerEvent& event)
	{
		m_step += m_flag;
		Refresh();
	}

}

