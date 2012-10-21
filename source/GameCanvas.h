/**
 * GameCanvas.h : head file of Game display canvas
 * 
 *
*/

#ifndef __SGS_GAMECANVAS_H__
#define __SGS_GAMECANVAS_H__

namespace sgs
{
	class GameApp;
	class GameMainFrame;

	class GameCanvas : public wxPanel
	{
	public:
		enum {
			ID_TIMER_REFRESH = 1,
		};
		GameCanvas(GameMainFrame* parent);
		virtual ~GameCanvas();		

		void OnPaint(wxPaintEvent& event);
		void OnSize(wxSizeEvent& event);

		virtual void OnDraw(wxDC&  dc);
		void OnTimer(wxTimerEvent& event);

	private:
		wxBitmap*   m_bitmap;
		wxTimer     m_timer;
		long        m_step;
		long        m_flag;

		DECLARE_EVENT_TABLE()
	};
}

#endif /* __SGS_GAMECANVAS_H__ */

