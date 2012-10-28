/**
 * GameApp.h : head file of Game Application
 * 
 *
*/

#ifndef __SGS_GAMEAPP_H__
#define __SGS_GAMEAPP_H__

namespace sgs
{

	class GameApp : public wxApp
	{

	public:
		virtual bool OnInit();

		virtual int OnExit();

	private:
		lua_State* m_L;
	};

	DECLARE_APP(GameApp)
}



#endif /* __SGS_GAMEAPP_H__ */

