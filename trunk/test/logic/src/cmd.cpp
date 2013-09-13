#include "config.h"
#include "cmd.h"
#include "game.h"
#include "card.h"
#include "hero.h"
#include "comm.h"
#include "event.h"
#include "get.h"
#include "out.h"
#include "skill.h"
#include "info.h"
#include "discard.h"
#include "script.h"
#include "save.h"


#define MAX_PARAM_NUM   64
#define MAX_CMD_LEN     4096

#define MAX_OUT_MSG_SIZE  65536

static char s_out_messages[MAX_OUT_MSG_SIZE] = {0};
static size_t s_out_len = 0;
static int  s_test_mode = 0;


void set_test_mode()
{
	s_test_mode = 1;
	s_out_messages[0] = 0;
	s_out_len = 0;
}


BOOL  is_test_mode()
{
	return s_test_mode != 0;
}



// get a world from cmd split with space,return the point to the word first valid char, null if no more any words 
static char* get_word(char* cmd, char** next)
{
	char *p = cmd;
	char *q;
	char quote = 0;

	while(*p && isspace(C2I(*p))) p++;
	
	// end of line
	if(*p == 0)
	{
		if(next) *next = p;
		return NULL; 
	}

	if(*p == '\'' || *p == '\"')
	{
		quote = *p;

		p++;
	}

	q = p;

	if(quote == 0)
	{
		while(*q && !isspace(C2I(*q))) q++;
	}
	else
	{
		while(*q && *q != quote) q++;

		if(*q == 0 || !( isspace(C2I(*(q+1))) || *(q+1) == 0) )
		{
			if(next) *next = p;
			return NULL;
		}
	}

	if(*q)
	{
		*q = 0;
		if(next) *next = q + 1;
	}
	else
	{
		if(next) *next = q;
	}


	return p;
}


static char* get_line(const char* prompt, char* buf, int size)
{
#ifdef WIN32
	int n,c;

	//fflush(stdin);

	MSG_OUT("%s", prompt);


	for(n = 0; n < size-1; n++)
	{
		c = getchar();
		if(c == EOF)
		{
			if(n == 0)
				return NULL;
			break;
		}
		else if(c=='\n')
		{
			break;
		}
		else
		{
			buf[n] = (char)c;
		}
	}
	buf[n] = 0;

#elif defined(LINUX)
	char    utf8[1024];
	char*   sl;


	A2UTF8( prompt, utf8, sizeof(utf8));

	log_text(utf8);
	sl = readline(utf8);

	if(strlen(sl) > 0)
		add_history(sl);

	strncpy(buf, sl, size);

	free(sl);

#endif

	log_text("%s\n", buf);

	return buf;
}

void cmd_output(const char* fmt, ...)
{
	char text[4096];
	char utf8[4096*2];


	va_list vl;
	size_t sz;

	va_start(vl, fmt);
	sz = vsnprintf(text, sizeof(text), fmt, vl);
	va_end(vl);

	if(is_test_mode())
	{
		if(sz + s_out_len >= sizeof(s_out_messages))
		{
			sz = sizeof(s_out_messages)/sizeof(s_out_messages[0]) - s_out_len - 1;
		}
		memcpy(s_out_messages + s_out_len, text, sz);
		s_out_len += sz;
		s_out_messages[s_out_len] = 0;
	}	

	A2UTF8(text, utf8, sizeof(utf8));

	log_text("%s", utf8);

	printf("%s", utf8);
}


static void cmd_help_i(const char* cmd);

static RESULT cmd_help(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	MSG_OUT(PROJ_NAME" "VERSION_STR"\n");
	cmd_help_i(argc > 1 ? argv[1] : NULL);
	return R_DEF;
}

static void param_error(const char* cmd)
{
	MSG_OUT("execute cmd \'%s\', get wrong params!\n", cmd);
	cmd_help_i(cmd);
}

static RESULT cmd_quit(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	//exit(0);

	if(pContext->status != Status_None)
	{
		//longjmp(pContext->__jb__, R_EXIT);
		pContext->status = Status_GameAbort;
		luaL_error(get_game_script(), "User Quit");
	}

	return R_EXIT;
}



static const NewGameConfig s_configs [] = {
	{ 6, 1, 1, 1, 3 },
	{ 7, 1, 2, 1, 3 },
	{ 8, 1, 2, 1, 4 },
};

#define CONFIG_SIZE    (int)(COUNT(s_configs))


const NewGameConfig* select_config(int players)
{
	int n;
	for(n = 0; n < CONFIG_SIZE; n++)
	{
		if(s_configs[n].players == players)
		{
			return &s_configs[n];
		}
	}
	return NULL;
}


static RESULT cmd_start(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{

	//RESULT  ret;
	int cfg;
	const NewGameConfig*  pConfig;
	GameEventContext    event;

	if(pContext->status != Status_None)
	{
		MSG_OUT("你正在游戏中，不能开始一个新的游戏!");
		return R_E_STATUS;
	}

	if(argc < 2 || 0 != to_int(argv[1], &cfg))
	{
		param_error(argv[0]);
		return R_E_PARAM;
	}

	// get config struct 
	pConfig = select_config(cfg);

	if(pConfig == NULL )
	{
		param_error(argv[0]);
		return R_E_PARAM;
	}

	// new game
	INIT_EVENT(&event, GameEvent_NewGame, INVALID_PLAYER, INVALID_PLAYER, pEvent);
	event.new_game_config = pConfig;


	//ret = init_game_context(pContext, pConfig->ministers, pConfig->spies, pConfig->mutineers);

	//if(ret != R_SUCC)
	//{
	//	MSG_OUT("start init new game failed!");
	//	return ret;
	//}

	// game main
	return game_main(pContext, &event);
}

static RESULT cmd_info(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	if(argc <= 1) // self info
	{
		if(pContext->status == Status_None)
		{
			MSG_OUT("游戏中才能查看当前玩家信息!\n");
			return R_E_STATUS;
		}
		else
		{
			game_cur_info(pContext, pEvent);
		}
	}
	else if(!strcmp(argv[1], "event") || !strcmp(argv[1], "e")) // game event info
	{
		game_event_info(pContext, pEvent, 0);
	}
	else if(!strcmp(argv[1], "eventfull") || !strcmp(argv[1], "ef")) // game event info
	{
		game_event_info(pContext, pEvent, 1);
	}
	else if(!strcmp(argv[1], "game") || !strcmp(argv[1], "g")) // game global info
	{
		if(pContext->status == Status_None)
		{
			MSG_OUT("游戏中才能查看游戏全局信息!\n");
			return R_E_STATUS;
		}
		else
		{
			game_global_info(pContext, pEvent);
		}
	}
	else if(!strcmp(argv[1], "gamefull") || !strcmp(argv[1], "gf")) // game full info 
	{
		MSG_OUT("暂时不支持该指令!\n");
	}
	else if(!strcmp(argv[1], "stack") || !strcmp(argv[1], "s")) // list out cards
	{
		if(pContext->status == Status_None)
		{
			MSG_OUT("游戏中才能查看牌堆信息!\n");
			return R_E_STATUS;
		}
		else
		{
			game_stack_info(pContext, pEvent);
		}
	}
	else if(!strcmp(argv[1], "player") || !strcmp(argv[1], "p"))  // player info: -n - prev [n] player; +n - next n player; n - player index n info; 
	{
		const char* pp;
		char cp = 0;
		int n = 0;
		if(pContext->status == Status_None)
		{
			MSG_OUT("游戏中才能查看玩家信息!\n");
			return R_E_STATUS;
		}

		if(argc < 3)
		{
			param_error(argv[0]);
			return R_E_PARAM;
		}

		pp = argv[2];

		if(*pp == '+' || *pp == '-')
		{
			cp = *pp;
			pp++;
		}


		if(*pp ==0 )
		{
			param_error(argv[0]);
			return R_E_PARAM;
		}


		while(*pp >= '0' && *pp <= '9')
		{
			n = n * 10 + (*pp -'0');
			pp++;
		}

		if(*pp != 0 )
		{
			param_error(argv[0]);
			return R_E_PARAM;
		}

		if(cp == '+')
		{
			n = (n + pContext->cur_player) % pContext->player_count;
		}
		else if(cp == '-')
		{
			n = (pContext->cur_player - n % pContext->player_count + pContext->player_count) % pContext->player_count;
		}
		else
		{
			n = n % pContext->player_count;
		}

		game_other_player_info(pContext, pEvent, n);
	}
	else if(!strcmp(argv[1], "card") || !strcmp(argv[1], "c"))
	{
		game_card_info(pContext, pEvent, argc < 3 ? NULL : argv[2]);
	}
	else if(!strcmp(argv[1], "hero") || !strcmp(argv[1], "h"))
	{
		game_hero_info(pContext, pEvent, argc < 3 ? NULL : argv[2]);	
	}
	else
	{
		param_error(argv[0]);
		return R_E_PARAM;
	}

	return R_DEF;
}

static RESULT cmd_get(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	if(get_game_status(pContext) == Status_None)
	{
		MSG_OUT("当前不在游戏中!\n");
		return R_E_STATUS;
	}
	int num = 1;
	if(argc>= 2)
	{
		if(0 != to_int(argv[1], &num))
		{
			num = 1;
		}
	}

	return game_cmd_getcard(pContext, pEvent, num);
}

static RESULT cmd_out(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	int idx[MAX_PARAM_NUM];
	int cnt = 0;
	int n;

	if(get_game_status(pContext) == Status_None)
	{
		MSG_OUT("当前不在游戏中!\n");
		return R_E_STATUS;
	}


	if(argc < 2)
	{
		param_error(argv[0]);
		return R_E_PARAM;
	}



	for(n = 1; n < argc && cnt < MAX_PARAM_NUM; n++)
	{
		if(0 != to_int(argv[n], &idx[cnt]))
		{
			param_error(argv[0]);
			return R_E_PARAM;
		}
		cnt++;
	}

	return game_cmd_outcard(pContext, pEvent, idx, cnt);

}


static RESULT cmd_useskill(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	int    idx;
	RESULT ret;
	if(get_game_status(pContext) == Status_None)
	{
		MSG_OUT("当前不在游戏中!\n");
		return R_E_STATUS;
	}

	ret = R_DEF;

	if(argc >= 2 && 0 == to_int(argv[1], &idx))
	{
		ret = game_cmd_use_skill(pContext, pEvent, idx);
	}
	else if(argc >= 2 && (0 == strcasecmp(argv[1], "weapon") || 0 == strcasecmp(argv[1], "w")) )
	{
		ret = game_cmd_use_weapon(pContext, pEvent);
	}
	else if(argc >= 2 && (0 == strcasecmp(argv[1], "armor") || 0 == strcasecmp(argv[1], "a")) )
	{
		ret = game_cmd_use_armor(pContext, pEvent);
	}
	else
	{
		param_error(argv[0]);
		return R_E_PARAM;
	}
	
	return ret;
}

/*
static RESULT cmd_useweapon(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	// no need param
	return R_SUCC;
}
*/


static RESULT cmd_cancelskill(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	if(get_game_status(pContext) == Status_None)
	{
		MSG_OUT("当前不在游戏中!\n");
		return R_E_STATUS;
	}


	return game_cmd_cancelskill(pContext, pEvent);

	//MSG_OUT("not in skill using!\n");

	//return CMD_RET_SUCC;
}


static RESULT cmd_pass(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	if(get_game_status(pContext) == Status_None)
	{
		MSG_OUT("当前不在游戏中!\n");
		return R_E_STATUS;
	}


	return game_cmd_pass(pContext, pEvent);

	//return CMD_RET_SUCC;
}

static RESULT cmd_discard(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	int idx[MAX_PARAM_NUM];
	int cnt = 0;
	int n;

	if(get_game_status(pContext) == Status_None)
	{
		MSG_OUT("当前不在游戏中!\n");
		return R_E_STATUS;
	}

	if(argc < 2)
	{
		param_error(argv[0]);
		return R_E_PARAM;
	}

	
	for(n = 1; n < argc && cnt < MAX_PARAM_NUM; n++)
	{
		if(0 != to_int(argv[n], &idx[cnt]))
		{
			param_error(argv[0]);
			return R_E_PARAM;
		}
		cnt++;
	}


	return game_cmd_discard_card(pContext, pEvent, idx, cnt);
}


static RESULT cmd_save(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	if(get_game_status(pContext) == Status_None)
	{
		MSG_OUT("当前不在游戏中，不能保存进度!\n");
		return R_E_STATUS;
	}

	if(argc != 2)
	{
		param_error(argv[0]);
		return R_E_PARAM;
	}


	game_save(pContext, argv[1]);

	return R_DEF;
}

static RESULT cmd_load(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	GameEventContext    event;
	//RESULT ret;
	if(pContext->status != Status_None)
	{
		MSG_OUT("已经在游戏中，不能加载一个游戏进度!\n");
		return R_E_STATUS;
	}

	if(argc != 2)
	{
		param_error(argv[0]);
		return R_E_PARAM;
	}
	
	// load game

	INIT_EVENT(&event, GameEvent_LoadGame, INVALID_PLAYER , INVALID_PLAYER, pEvent);
	event.file_name = argv[1];

	//ret = game_load(pContext, argv[1]);
	
	//if(R_SUCC != ret)
	//{
	//	return ret;
	//}

	// game main
	return game_main(pContext, &event);
}


static RESULT cmd_reload(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	if(pContext->status != Status_None)
	{
		MSG_OUT("在游戏中不能重新加载脚本，请先结束本局游戏!\n");
		return R_E_STATUS;
	}

	return reload_game_script();
}


static RESULT  cmd_test(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	// 列出/执行测试用例


	if(argc < 2)
	{
		//param_error(argv[0]);
		//return R_E_PARAM;
	}


	if(argc < 2 || !strcmp(argv[1], "list") || !strcmp(argv[1], "l"))
	{
		// script_list_test
		script_test_list();

	}
	else if(!strcmp(argv[1], "run") || !strcmp(argv[1], "r"))
	{
		// script_test_run(index)
		
		if(argc >= 3)
		{
			if (R_SUCC == script_test_run(pContext, atoi(argv[2])))
			{
				set_test_mode();
			}
		}
		else
		{
			if (R_SUCC == script_test_run(pContext, -1))
			{
				set_test_mode();
			}
		}
	}


	return R_DEF;
}

typedef RESULT (*FunCmd)(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent);


struct tagCmdDispatch
{
	const char* name;
	const char* sort_name;
	FunCmd  func;
	const char*  brief;
	const char*  detail;
};

static const struct tagCmdDispatch   s_cmdDispatch[] = {
	{ "help", "h", cmd_help,
		"help/h\n\tprint this message\n"
		"help/h <cmd>\n\tprint the <cmd> desc message.", 
		NULL},
	{ "quit", "q", cmd_quit,
		"quit/q\n\tquit the program.",
		NULL},
	{ "start", "s", cmd_start, 
		"start/s <cfg_num>\n\tstart a new game with <cfg_num> players config." , 
		"\tthe cfg_num can be one of 6,7 or 8:\n"
		"\t* 6 players: 1 master, 1 minister, 1 spy, 3 mutineers;\n"
		"\t* 7 players: 1 master, 2 ministers, 1 spy, 3 mutineers;\n"
		"\t* 8 players: 1 master, 2 ministers, 1 spy, 4 mutineers."},
	{ "info", "i",	cmd_info, 
		"info/i\n\tshow current player info, handle cards, aromo card, etc.\n"
		"info/i game/g\n\t show the game current global info\n"
		"info/i event/e\n\t show the game current event info\n"
		"info/i player/p <+n|-n|n>\n\t show the other player info\n"
		"info/i stack/s\n\t show the game stacks\n"
		"info/i card/c <name>\n\t show card info by name\n"
		"info/i hero/h <name>\n\t show hero info by name", 
		NULL},
	{ "get", "g",   cmd_get,
		"get/g\n\tget card from card stack.", 
		NULL},
	{ "out", "o",	cmd_out, 
		"out/o <card idx> ...\n\tout one or more card.", 
		NULL},
	{ "useskill", "u",	cmd_useskill, 
		"useskill/u <skill idx> ...\n\tuse a hero skill.\n" 
		"useskill/u w/weapon ...\n\tuse weapon skill.\n"
		"useskill/u a/armor ...\n\tuse armor skill.", 
		NULL},
	/*{ "usesweapon", "w",	cmd_useweapon, 
		"usesweapon/w ...\n\tuse the weapon card - some weapon can active in out card round - as a skill.", 
		NULL},*/
	{ "cancel", "c",	cmd_cancelskill, 
		"cancel/c \n\tcancel the skill in using.", 
		NULL},
	{ "pass", "p", cmd_pass,
		"pass/p\n\tno out card, so pass this round.", 
		NULL},
	{ "discard", "d", cmd_discard,
		"discard/d <card idx> ...\n\tdiscard card with idx.", 
		NULL},

	{ "save", NULL, cmd_save,
		"save <file_name>\n\tsave current game context to file.", 
		NULL},
	{ "load", NULL, cmd_load,
		"load <file_name>\n\tload current game context from file.", 
		NULL},
	{ "reload", "r", cmd_reload,
		"reload \n\treload script files.", 
		NULL},
	{ "test", "t", cmd_test,
		"test list/l\n\tlist all test cases.\n"
		"test run/r all/<id>\n\trun all or give index test case.", 
		NULL},
};

#define CMD_NUM (int)(COUNT(s_cmdDispatch))


static void cmd_help_i(const char* cmd)
{
	int n;
	if(cmd == NULL)
	{
		for(n = 0; n < CMD_NUM; n++)
		{
			if(s_cmdDispatch[n].brief != NULL)
			{
				MSG_OUT("%s\n", s_cmdDispatch[n].brief);
			}
		}
	}
	else
	{
		for(n = 0; n < CMD_NUM; n++)
		{
			if(!strcmp(cmd, s_cmdDispatch[n].name) || (s_cmdDispatch[n].sort_name && !strcmp(cmd, s_cmdDispatch[n].sort_name)))
			{
				if(s_cmdDispatch[n].brief != NULL)
				{
					MSG_OUT("%s\n", s_cmdDispatch[n].brief);
					if(s_cmdDispatch[n].detail != NULL)
					{
						MSG_OUT("%s\n", s_cmdDispatch[n].detail);
					}
				}
				else
				{
					MSG_OUT("no help info for cmd \'%s\'\n", cmd);
				}
				break;
			}
		}

		if(n >= CMD_NUM)
		{
			MSG_OUT("cmd \'%s\' not found!\n", cmd);
		}
	}
}

static RESULT cmd_do_script(GameContext* pGame, GameEventContext* pEvent)
{
	RESULT ret;
	int state;
	lua_State* L = get_game_script();
	
	lua_pushlightuserdata(L, pGame);
	lua_pushlightuserdata(L, pEvent);

	state = script_pcall(L, 2, 1);
	
	if(state != 0)
	{
		MSG_OUT("%s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return R_E_FAIL;
	}

	ret = (RESULT)lua_tointeger(L, -1);

	lua_pop(L, 1);

	return ret;
}

static char* get_cmd_line(GameContext* pGame, GameEventContext* pEvent, const char* prompt, char* buf, int len)
{
	int     mode;
	RESULT  ret;
	if(is_test_mode())
	{
		// contiue test proc
		mode = CMD_LINE_MODE_NORMAL;

		MSG_OUT("%s", prompt);

		ret = script_test_continue(s_out_messages,s_out_len, buf, len, &mode);
		if(R_SUCC == ret)
		{
			s_out_len = 0;
			s_out_messages[0] = 0;


			if(mode == CMD_LINE_MODE_SCRIPT)
			{
				if(R_SUCC != cmd_do_script(pGame, pEvent))
				{
					s_test_mode = 0;
					buf[0] = 0;
					MSG_OUT("%s\n", buf);

					script_test_error();

					if(pGame->status != Status_None)
					{
						//longjmp(pContext->__jb__, R_EXIT);
						//pGame->status = Status_GameAbort;
						luaL_error(get_game_script(), "Run test error!");
					}

					return buf;
				}
			}


			MSG_OUT("%s\n", buf);
			return buf;
		}

		s_out_len = 0;
		s_out_messages[0] = 0;

		// 失败退出测试模式
		s_test_mode = 0;
		buf[0] = 0;
		MSG_OUT("%s\n", buf);

		if(R_CANCEL == ret)
		{
			// test finished

		}
		else if(pGame->status != Status_None)
		{
			//longjmp(pContext->__jb__, R_EXIT);
			//pGame->status = Status_GameAbort;
			luaL_error(get_game_script(), "Run test error!");
		}


		return buf;
	}
	else
	{
		mode = CMD_LINE_MODE_NORMAL;
		return get_line(prompt, buf, len);
	}
}


RESULT cmd_loop(GameContext* pContext, GameEventContext* pEvent, YESNO force, const char* strAlter)
{
	char  prompt[MAX_NAME_LEN + 10];
	char  cmdline[MAX_CMD_LEN];
	const char*  argv[MAX_PARAM_NUM];
	int   argc;
	char  *next, *w;
	int   n;
	RESULT   ret;

	if(get_game_status(pContext) == Status_None)
	{
		snprintf(prompt, sizeof(prompt), "[SGS2010] $ ");
	}
	else
	{
		snprintf(prompt, sizeof(prompt), "[%s] $ ", CUR_PLAYER(pContext)->name);
	}

	while( (strAlter ? MSG_OUT("%s\n", strAlter) : 0) , get_cmd_line(pContext, pEvent, prompt, cmdline, sizeof(cmdline)))
	{
		next =  cmdline;
		argc = 0;
		memset(argv, 0, sizeof(argv));
		while( NULL != (w = get_word(next, &next) ) )
		{
			if(argc < (int)COUNT(argv))
			{
				argv[argc++] = w;
			}
		}

		if(*next != 0)
		{
			MSG_OUT("error cmd at col %d!\n", (int)(next - cmdline));
		}
		else if(argc > 0)
		{
			ret = R_DEF;

			//if(funper != NULL)
			//{
			//	ret = (*funper)(argv, argc, pContext, ud);
			//	if(ret < 0)
			//		return ret;
			//}

			if(ret == R_DEF)
			{
				for(n= 0; n < CMD_NUM; n++)
				{
					if(!strcmp(argv[0], s_cmdDispatch[n].name) || (s_cmdDispatch[n].sort_name && !strcmp(argv[0], s_cmdDispatch[n].sort_name)))
					{
						break;
					}
				}

				if(n < CMD_NUM)
				{

					ret = (*s_cmdDispatch[n].func)(argv, argc, pContext, pEvent);

					if(ret == R_CANCEL && force == YES)
					{
						MSG_OUT("当前不能取消（放弃）操作！\n");
					}
					else
					{
						switch(ret)
						{
						case R_SUCC:         // success and back to caller
						case R_BACK:         // back to caller no result
						case R_CANCEL:       // cancel current operator and back to caller
						case R_EXIT:         // exit game 
							return ret;
						default:
							break;
						}
					}
				}
				else
				{
					MSG_OUT("无效的命令：\'%s\'!\n", argv[0]);
				}
			}
		}
	}

	//  end of input ???
	return R_EXIT;
}

static const char*  strchr_notnull(const char* str,  char ch)
{
	while(*str && *str != ch)
		str++;
	return str;
}


RESULT select_loop(GameContext* pContext, GameEventContext* pEvent, const SelOption options[], int optnum, const char* strAlter, int* out_value)
{
	int n;
	const char* s;
	const char* p;
	char  buffer[1024];
	int   buflen;
	int   v;

	while(1)
	{
		if(strAlter)
			MSG_OUT("%s\n", strAlter);

		for(n = 0; n < optnum; n++)
		{
			buflen = 0;
			buffer[0] = 0;
			buflen += sprintf(buffer + buflen, "(");
			s = options[n].input;
			
			if(*s == '\n' || *s == '\0')
			{
				buflen += sprintf(buffer + buflen, "%d", options[n].value);
				if(*s != 0)
					s++;
			}
			else
			{
				p = strchr_notnull(s, '\n');
				strncpy(buffer + buflen, s, p - s);
				buflen += (p-s);

				if(*p == '\0')
					s = p;
				else
					s = p + 1;
			}

			while(*s != '\0' && *s != '\n' )
			{
				p = strchr_notnull(s, '\n');

				buffer[buflen++] = '/';

				strncpy(buffer + buflen, s, p - s);
				buflen += (p-s);

				if(*p == 0)
					break;

				s = p + 1;
			}
			buflen += sprintf(buffer + buflen, ") %s", options[n].text);
			buffer[buflen] = 0;

			MSG_OUT(" %s\n", buffer);


		}

		if(NULL == get_cmd_line(pContext, pEvent, "[请选择] : ", buffer, sizeof(buffer)))
			return R_E_FAIL;

		strtrim(buffer);
		buflen = strlen(buffer);

		if(R_SUCC == to_int(buffer, &v))
		{
			
			for(n = 0; n < optnum; n++)
			{
				if((options[n].input[0] == '\0' || options[n].input[0] == '\n') && v == options[n].value)
				{
					*out_value = options[n].value;
					return R_SUCC;
				}
			}
		}

		for(n = 0; n < optnum; n++)
		{
			s = options[n].input;

			if(*s == '\n')
				s++;


			while(*s != '\0' && *s != '\n')
			{
				p = strchr_notnull(s, '\n');

				if(buflen == (p-s) && !strncasecmp(s, buffer, buflen))
				{
					*out_value = options[n].value;
					return R_SUCC;
				}

				if(*p == 0)
					break;

				s = p + 1;
			}
		}

	}
	return R_E_FAIL;
}



