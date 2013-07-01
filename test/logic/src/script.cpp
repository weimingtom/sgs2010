#include "config.h"
#include "game.h"
#include "cmd.h"
#include "script.h"


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
#include "../pkg/lua_export.h"


struct tagScriptHandle
{
	lua_State* L;
};


struct tagScriptObjectHandle
{
	int ref;
};


script_t script_create()
{
	script_t  s = X_TALLOC(struct tagScriptHandle);

	if(s == NULL)
		return NULL;

	s->L = lua_open();
	if(!s->L) 
	{
		X_FREE(s);
		s = NULL;
		return  NULL;
	}

	luaL_openlibs(s->L);

	tolua_game_open(s->L);

	return s;
}



void script_destroy(script_t s)
{
	if(s != NULL)
	{
		if(s->L != NULL)
		{
			lua_close(s->L);
		}
		X_FREE(s);
	}
}




