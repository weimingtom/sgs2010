#include "config.h"
#include "game.h"
#include "cmd.h"
#include "comm.h"
#include "script.h"

#include "load.h"
#include "event.h"

#include "../pkg/lua_export.h"




static lua_State* g_game_L = NULL;
static lua_State* g_ai_L = NULL;


// some usefull functions

static int luaex_bitor(lua_State* L)
{
	unsigned int ret = 0;
	int  top = lua_gettop(L);
	int  n;
	unsigned int  v;

	if(top < 1)
	{
		luaL_error(L, "at least 1 parameters for function 'bitor'");
	}

	for(n = 1; n <= top; n++)
	{
		v = (unsigned int)luaL_checkinteger(L, n);
		ret |= v;
	}

	lua_pushnumber(L, ret);
	return 1;
}


static int luaex_bitand(lua_State* L)
{
	unsigned int ret = 0xffffffff;

	int  top = lua_gettop(L);
	int  n;
	unsigned int  v;

	if(top < 1)
	{
		luaL_error(L, "at least 1 parameters for function 'bitand'");
	}

	for(n = 1; n <= top; n++)
	{
		v = (unsigned int)luaL_checkinteger(L, n);
		ret &= v;
	}

	lua_pushnumber(L, ret);
	return 1;
}


static int luaex_bitnot(lua_State* L)
{
	unsigned int  v = luaL_checkinteger(L, 1);

	lua_pushnumber(L, ~v);
	return 1;
}

static int luaex_bitxor(lua_State* L)
{
	unsigned int ret = 0;

	int  top = lua_gettop(L);
	int  n;
	unsigned int  v;

	if(top < 2)
	{
		luaL_error(L, "at least 2 parameters for function 'bitxor'");
	}

	for(n = 1; n <= top; n++)
	{
		v = (unsigned int)luaL_checkinteger(L, n);
		ret ^= v;
	}

	lua_pushnumber(L, ret);
	return 1;
}




// for lua replace dofile...
// export as import(path)
static int luaex_import_file(lua_State* L);


static int luaex_message(lua_State* L)
{
	char  msg[4096];
	int   len;
	int   n;
	int   top = lua_gettop(L);

	len = 0;
	for( n = 1; n <= top; n++)
	{
		lua_getglobal(L, "tostring");
		lua_pushvalue(L, n);
		lua_call(L, 1, 1);
		len += snprintf(msg+len, sizeof(msg)-len, "%s\t", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
	msg[len-1] = '\n';
	MSG_OUT("%s", msg);
	return 0;
}



static int file_name_cmp(lua_State* L)
{
	int ret;
	const char*  f1 = luaL_optstring(L, 1, "");
	const char*  f2 = luaL_optstring(L, 2, "");
	ret = strcasecmp(f1, f2);
	//MSG_OUT("file_name_cmp ('%s','%s') = %d\n", f1, f2, ret);
	lua_pushboolean(L, ret < 0);
	return 1;
}

int luaex_import_file(lua_State* L )
{
#ifdef WIN32
	struct _finddata_t  fdata;
	long  fid;
#elif defined(LINUX)
	DIR* pDir;
	struct dirent *pDirent;
	struct stat f_st; 
#endif

	const char* last_path;
	char  rel_path[MAX_PATH];
	char* base_name;
	char  cwd_path[MAX_PATH];
	char  cur_path[MAX_PATH];
	char  full_path[MAX_PATH];
	int  n;
	const char* pattern;

	pattern = luaL_checkstring(L, 1);

	lua_getfield(L, LUA_REGISTRYINDEX, "__import_path");

	if(lua_isnil(L, -1))
	{
		luaL_error(L, "import call only while importing lua files");
		return 0;
	}

	last_path = lua_tostring(L, -1);
	(void)last_path;

	get_cur_path(cwd_path, sizeof(cwd_path));

	strncpy(rel_path, pattern, sizeof(rel_path));
	REGULAR_PATH(rel_path);

	base_name = strrchr(rel_path, '/');

	if(base_name != NULL)
	{
		*base_name = '\0';
		base_name++;

		if(0 != set_cur_path(rel_path))
		{
			luaL_error(L, "import \"%s\" error: (%d) %s", pattern, errno, strerror(errno));
			return 0;
		}
	}
	else
	{
		base_name = rel_path;
	}

	get_cur_path(cur_path, sizeof(cur_path));


	//path = pattern;

#ifdef WIN32
	fid = _findfirst(base_name, &fdata);

	if(fid == -1)
#elif defined(LINUX)
	pDir = opendir(cur_path);

	if(pDir == NULL)
#endif
	{
		set_cur_path(cwd_path);
		luaL_error(L, "import \"%s\" error: (%d) %s", pattern, errno, strerror(errno));
		return 0;
	}

	lua_newtable(L);   // ... [t]
	n = 1;	

#ifdef WIN32
	do {
		// skip the sub directories
		if(0 == (fdata.attrib & _A_SUBDIR))
		{
			//MSG_OUT("%s\n", fdata.name);
			lua_pushnumber(L, n);   // ... [t] [n]
			lua_pushstring(L, fdata.name);   // ... [t] [n] [fdata.name]
			lua_rawset(L, -3);      // ... [t]
			n++;
		}
	} while(0 == _findnext(fid, &fdata));
	_findclose(fid);
#elif defined(LINUX)
	while((pDirent = readdir(pDir)) != NULL)
	{
		// skip '.' and '..'
		if (strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0)  
		{  
			continue;  
		}
		// skip the sub directories
		snprintf(full_path, sizeof(full_path), "%s/%s", cur_path, pDirent->d_name);
		if(0 == stat(full_path, &f_st) && S_ISDIR(f_st.st_mode))
		{
			continue;
		}

		//match the pattern ?
		if(0 == fnmatch(base_name, pDirent->d_name, FNM_PATHNAME))
		{
			//MSG_OUT("%s\n", fdata.name);
			lua_pushnumber(L, n);   // ... [t] [n]
			lua_pushstring(L, pDirent->d_name);   // ... [t] [n] [fdata.name]
			lua_rawset(L, -3);      // ... [t]
			n++;
		}
	}
	closedir(pDir);
#endif

	//call table.sort
	lua_getglobal(L, "table");   // ... [t] [table]
	lua_getfield(L, -1, "sort"); // ... [t] [table] [table.sort]
	lua_pushvalue(L, -3);       
	lua_pushcfunction(L, file_name_cmp);   // ... [t] [table] [table.sort] [t] [file_name_cmp]
	if(0 != script_pcall(L, 2, 0))  // ... [t] [table] 
	{
		set_cur_path(cwd_path);
		lua_error(L);
		return 0;
	}

	lua_pop(L, 1);   // ... [t]

	for(n = 1; ; n ++)
	{
		lua_pushnumber(L, n);   // ... [t] [n]
		lua_rawget(L, -2);      // ... [t] [t[n]]
		if(lua_isnil(L, -1))
		{
			lua_pop(L, 1);     // ... [t]
			break;
		}


		snprintf(full_path, sizeof(full_path), "%s/%s", cur_path, lua_tostring(L, -1));

		lua_pop(L, 1);     // ... [t]

		// is imported?
		lua_getfield(L, LUA_REGISTRYINDEX, "__imported_table");   // ... [t] [__imported_table]
		if(lua_isnil(L, -1)) 
		{
			lua_pop(L, 1);
			lua_newtable(L);
			lua_pushvalue(L, -1);
			lua_setfield(L, LUA_REGISTRYINDEX, "__imported_table");
		}

		lua_getfield(L, -1, full_path); // ... [t] [__imported_table]  [__imported_table[full_path]]

		if(!lua_isnil(L, -1))
		{
			lua_pop(L, 2);
			// imported already
			continue;
		}

		lua_pop(L, 2);  // ... [t]

		// is in importing?

		lua_getfield(L, LUA_REGISTRYINDEX, "__importing_table");  // ... [t] [__importing_table]
		if(lua_isnil(L, -1)) 
		{
			lua_pop(L, 1);
			lua_newtable(L);
			lua_pushvalue(L, -1);
			lua_setfield(L, LUA_REGISTRYINDEX, "__importing_table");
		}

		lua_getfield(L, -1, full_path);  // ... [t] [__importing_table] [__importing_table[full_path]] 

		if(!lua_isnil(L, -1))
		{
			// is in importing
			lua_pop(L, 2);
			set_cur_path(cwd_path);
			luaL_error(L, "import file \"%s\" is circular dependency.", full_path);
		}

		lua_pop(L, 1);  // ... [t] 

		// add to impirting table
		lua_pushstring(L, full_path);
		lua_pushnumber(L, 1);
		lua_settable(L, -3);  // ... [t] [__importing_table]

		lua_pop(L, 1);  // ... [t] 

		//MSG_OUT("import \"%s\"\n", full_path);

		if(0 != luaL_dofile(L, full_path))
		{
			set_cur_path(cwd_path);
			lua_error(L);
			return 0;
		}
		// remove from importing table
		lua_getfield(L, LUA_REGISTRYINDEX, "__importing_table");  // ... [t] [__importing_table]
		lua_pushstring(L, full_path);
		lua_pushnil(L);
		lua_settable(L, -3);  // ... [t] [__importing_table]

		lua_pop(L, 1);  // ... [t] 

		// add to imported table;
		lua_getfield(L, LUA_REGISTRYINDEX, "__imported_table");  // ... [t] [__importing_table]
		lua_pushstring(L, full_path);
		lua_pushnumber(L, 1);
		lua_settable(L, -3);  // ... [t] [__importing_table]

		lua_pop(L, 1);  // ... [t] 

	}

	// ... [t]

	lua_pop(L, 1);

	// [__import_path]
	lua_pop(L, 1);
	set_cur_path(cwd_path);

	return 0;
}



static int luaex_test_send_cmd(lua_State* L);
static int luaex_test_expect(lua_State* L);
static int luaex_game_load(lua_State* L);


static RESULT game_script_prepare(lua_State* L)
{
	// load lua base libraries
	luaL_openlibs(L);

	// owner custom exports
	lua_pushcfunction(L, luaex_message);
	lua_setglobal(L, "print");
	lua_pushcfunction(L, luaex_message);
	lua_setglobal(L, "message");
	lua_pushcfunction(L, luaex_import_file);
	lua_setglobal(L, "import");

	// common useful functions
	lua_pushcfunction(L, luaex_bitor);
	lua_setglobal(L, "bitor");
	lua_pushcfunction(L, luaex_bitand);
	lua_setglobal(L, "bitand");
	lua_pushcfunction(L, luaex_bitnot);
	lua_setglobal(L, "bitnot");
	lua_pushcfunction(L, luaex_bitxor);
	lua_setglobal(L, "bitxor");




	// load game core libraries
	tolua_game_open(L);

	// extra game functions
	// for load game from a table
	lua_pushcfunction(L, luaex_game_load);
	lua_setglobal(L, "game_load");


	// register test api
	lua_pushcfunction(L, luaex_test_send_cmd);
	lua_setglobal(L, "send_cmd");
	lua_pushstring(L, "");
	lua_pushcclosure(L, luaex_test_expect, 1);
	lua_setglobal(L, "expect");


	return reload_game_script();
}


RESULT  init_game_script()
{
	RESULT  ret;
	lua_State* L;
	close_game_script();


	L = lua_open();

	if(L == NULL)
	{
		MSG_OUT("Create lua script engine for game logic failed!\n");
		return R_E_FAIL;
	}

	g_game_L = L;

	ret = game_script_prepare(L);

	if(ret != R_SUCC)
	{
		close_game_script();
		return ret;
	}


	return R_SUCC;
}



lua_State* get_game_script()
{
	return g_game_L;
}


void  close_game_script()
{
	if(g_game_L != NULL)
	{
		lua_close(g_game_L);
	}
}


RESULT lua_import(lua_State* L,  const char* luafile_path)
{
	int   state;
	char  base_path[MAX_PATH];

	get_app_path(base_path, sizeof(base_path));
	set_cur_path(base_path);
	lua_pushstring(L, base_path);
	lua_setfield(L, LUA_REGISTRYINDEX, "__import_path");
	lua_getglobal(L, "import");
	lua_pushstring(L, luafile_path);
	state = script_pcall(L, 1, 0);
	if(state != 0)
	{
		MSG_OUT("%s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return R_E_FAIL;
	}

	lua_pushnil(L);
	lua_setfield(L, LUA_REGISTRYINDEX, "__import_path");

	return R_SUCC;
}


RESULT  reload_game_script()
{
	int state;
	RESULT ret;
	lua_State* L = g_game_L;

	if(L == NULL)
		return R_E_FAIL;

	// clear before reload
	lua_getglobal(L, "reload_clear");
	state = script_pcall(L, 0, 0);

	if(state != 0)
	{
		MSG_OUT("%s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return R_E_FAIL;
	}


	// clear import table
	lua_newtable(L);
	lua_setfield(L, LUA_REGISTRYINDEX, "__imported_table");
	lua_newtable(L);
	lua_setfield(L, LUA_REGISTRYINDEX, "__importing_table");

	// load lua files
	ret = lua_import(L, "../script/main.lua");

	return ret;
}


lua_State* get_ai_script()
{
	return g_ai_L;
}



char* get_enumt_str(const char* enum_type, int val, int is_bit_or, char* buf, int buflen)
{
	static  char s_tmp[1024];

	lua_State* L = get_game_script();

	lua_getglobal(L, "get_enum_str");

	lua_pushstring(L, enum_type);	
	lua_pushnumber(L, val);	
	lua_pushboolean(L, is_bit_or);	
	lua_call(L, 3, 1);

	if(buf == NULL) 
	{
		buf = s_tmp;
		buflen = sizeof(s_tmp);
	}
			
	if(lua_isstring(L, -1))
	{
		strncpy(buf, lua_tostring(L, -1), buflen);
	}
	else
	{
		buf[0] = 0;
	}

	lua_pop(L, 1);
	return buf;

}


static int traceback (lua_State *L) 
{
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	if (!lua_istable(L, -1)) 
	{
		lua_pop(L, 1);
		return 1;
	}
	lua_getfield(L, -1, "traceback");
	if (!lua_isfunction(L, -1)) 
	{
		lua_pop(L, 2);
		return 1;
	}
	lua_pushvalue(L, 1);  /* pass error message */
	lua_pushinteger(L, 2);  /* skip this function and traceback */
	lua_call(L, 2, 1);  /* call debug.traceback */
	return 1;
}


int script_pcall(lua_State *L, int narg, int result) 
{
	int status;
	int base = lua_gettop(L) - narg;  /* function index */
	lua_pushcfunction(L, traceback);  /* push traceback function */
	lua_insert(L, base);  /* put it under chunk and args */
	//signal(SIGINT, laction);
	status = lua_pcall(L, narg, result, base);
	//signal(SIGINT, SIG_DFL);
	lua_remove(L, base);  /* remove traceback function */
	/* force a complete garbage collection in case of errors */
	if (status != 0) lua_gc(L, LUA_GCCOLLECT, 0);
	return status;
}

enum TEST_CMD {
	TEST_CMD_INIT = 0,
	TEST_CMD_LOAD = 1,
	TEST_CMD_SENDCMD = 2,
};

#define  TEST_ERR   printf
#define  TEST_INFO    printf



static int luaex_game_load(lua_State* L)
{
	if(lua_type(L, 1) == LUA_TSTRING)
	{
		TEST_INFO(">> load game from : %s\n", lua_tostring(L, 1));
	}
	else if(lua_type(L, 1) == LUA_TTABLE)
	{
		TEST_INFO(">> load game from table\n", lua_tostring(L, 1));
	}
	else 
	{
		luaL_typerror(L, 1, "string or table");
	}
	lua_pushnumber(L, TEST_CMD_LOAD);
	lua_pushvalue(L, 1);
	return lua_yield(L, 2);
}


static int aux_load(lua_State* L)
{
	GameContext*       pGame;
	GameEventContext*  pParentEvent;
	GameEventContext    event;


	pGame = (GameContext*)lua_touserdata(L, 1);
	pParentEvent = (GameEventContext*)lua_touserdata(L, 2);

	lua_pushvalue(L, lua_upvalueindex(1));
	lua_setfield(L, LUA_REGISTRYINDEX, "?sav");


	//RESULT ret;
	if(pGame->status != Status_None)
	{
		luaL_error(L, "已经在游戏中，不能加载一个游戏进度!");
		return 0;
	}

	// load game

	INIT_EVENT(&event, GameEvent_LoadGame, INVALID_PLAYER , INVALID_PLAYER, pParentEvent);
	event.file_name = "?sav";

	// game main
	lua_pushnumber(L, game_main(pGame, &event));
	return 1;
}

static int luaex_test_send_cmd(lua_State* L)
{
	// return command string to cmd process
	const char* cmd = luaL_checkstring(L, 1);
	//TEST_INFO("send cmd: %s\n", cmd);
	lua_pushnumber(L, TEST_CMD_SENDCMD);
	lua_pushvalue(L, 1);
	return lua_yield(L, 2);
}

static int luaex_test_expect(lua_State* L)
{
	// get out text
	const char* pat = luaL_checkstring(L, 1);
	const char* text = lua_tostring(L, lua_upvalueindex(1));

	//TEST_INFO("pattern: %s\n", pat);
	//TEST_INFO("text   : %s\n", text);

	return 0;
}


static RESULT script_test_resume(lua_State* Lt, int args, char* buf, int len, int* mode);


RESULT script_test_run(GameContext* pGame, int index)
{
	lua_State* L;
	lua_State* Lt;

	// main script state
	L = get_game_script();

	lua_getfield(L, LUA_REGISTRYINDEX, "__test_thread__");

	Lt = lua_tothread(L, -1);
	if(Lt != NULL)
	{
		TEST_ERR("test is already in running!\n");
		return R_E_FAIL;
	}

	lua_pop(L, 1);
	// new lua thread
	Lt = lua_newthread(L);
	if(Lt == NULL)
	{
		TEST_ERR("create test thread failed\n");
		return R_E_FAIL;
	}

	lua_setfield(L, LUA_REGISTRYINDEX, "__test_thread__");

	// 启动函数(需要是lua函数)
	lua_getfield(L, LUA_GLOBALSINDEX, "do_test");
	lua_xmove(L, Lt, 1);


	// 第一次resume 传入 Game对象
	tolua_pushusertype(Lt, pGame, "GameContext");
	// 第二个参数，要运行的testcase 序号
	if(index <= 0)
	{
		lua_pushnil(Lt);
	}
	else
	{
		lua_pushnumber(Lt, index);
	}
	
	return script_test_resume(Lt, 2, NULL, 0, NULL);

}


RESULT script_test_continue(const char* msg, int msg_sz, char* buf, int len, int* mode)
{
	lua_State* L;
	lua_State* Lt;

	// main script state
	L = get_game_script();

	lua_getfield(L, LUA_REGISTRYINDEX, "__test_thread__");
		
	Lt = lua_tothread(L, -1);
	if(Lt == NULL)
	{
		TEST_ERR("test is not in running!\n");
		return R_E_FAIL;
	}

	lua_pop(L, 1); // 删除

	// 修改expect 函数的upvalue(1)
	lua_getglobal(L, "expect");
	lua_pushlstring(L, msg, msg_sz);
	lua_setupvalue(L, -2, 1);
	lua_pop(L, 1);
	
	return script_test_resume(Lt, 0, buf, len, mode);
}


static RESULT script_test_resume(lua_State* Lt, int args, char* buf, int len, int* mode)
{
	int state;

	if(mode)
	{
		*mode = CMD_LINE_MODE_NORMAL;
	}

	state = lua_resume(Lt, args);


	if(state != 0)
	{
		if(state != LUA_YIELD)
		{
			TEST_ERR("%s\n", lua_tostring(Lt, -1));
			return R_E_FAIL;
		}
		else
		{
			if(buf && mode)
			{
				// get cmd 
				buf[0] = 0;
				if(lua_gettop(Lt) >= 2)
				{
					int cmd = luaL_checkinteger(Lt, 1);
					switch(cmd)
					{
					case TEST_CMD_LOAD:
						// load game continue
						*mode = CMD_LINE_MODE_SCRIPT;
						lua_pushvalue(Lt, 2);
						lua_xmove(Lt, get_game_script(), 1);
						lua_pushcclosure(get_game_script(), aux_load, 1);

						// strncpy(buf, "aux_load", len);
						break;
					case TEST_CMD_SENDCMD:
						strncpy(buf, luaL_checkstring(Lt, 2), len);
						buf[len-1] = 0;
						break;
					}
				}
			}
			return R_SUCC;
		}
	}
	else
	{
		// test over
		lua_pushstring(Lt, "__test_thread__");
		lua_pushnil(Lt);
		lua_settable(Lt, LUA_REGISTRYINDEX);

		// exit test mode
		return R_CANCEL;
	}
	return R_SUCC;
}

RESULT script_test_list()
{
	int state;
	lua_State* L;

	// main script state
	L = get_game_script();

	lua_getglobal(L, "test_list");

	state = script_pcall(L, 0, 0);

	if(state != 0)
	{
		MSG_OUT("%s", lua_tostring(L, -1));
		lua_pop(L, 1);
		return R_E_FAIL;
	}

	return R_SUCC;
}

