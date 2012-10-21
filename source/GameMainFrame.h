/**
 * GameMainFrame.h : head file of Game Main Frame Window
 * 
 *
*/

#ifndef __SGS_GAMEMAINFRAME_H__
#define __SGS_GAMEMAINFRAME_H__

namespace sgs
{
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

}




#endif /* __SGS_GAMEMAINFRAME_H__ */
