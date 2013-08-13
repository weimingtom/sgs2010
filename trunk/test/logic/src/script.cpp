#include "config.h"
#include "game.h"
#include "cmd.h"
#include "comm.h"
#include "script.h"


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
		lua_pushstring(L, "import call only while importing lua files");
		lua_error(L);
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
			lua_pushfstring(L, "import \"%s\" error: (%d) %s", pattern, errno, strerror(errno));
			lua_error(L);
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
		lua_pushfstring(L, "import \"%s\" error: (%d) %s", pattern, errno, strerror(errno));
		lua_error(L);
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
	if(0 != lua_pcall(L, 2, 0, 0))  // ... [t] [table] 
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
			lua_pushfstring(L, "import file \"%s\" is circular dependency.", full_path);
			set_cur_path(cwd_path);
			lua_error(L);
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
	state = lua_pcall(L, 1, 0, 0);
	if(state != 0)
	{
		MSG_OUT("%s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return R_ABORT;
	}

	lua_pushnil(L);
	lua_setfield(L, LUA_REGISTRYINDEX, "__import_path");

	return R_SUCC;
}


RESULT  reload_game_script()
{
	RESULT ret;
	lua_State* L = g_game_L;

	if(L == NULL)
		return R_E_FAIL;

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
