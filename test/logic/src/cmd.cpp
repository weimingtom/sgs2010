#include "config.h"
#include "cmd.h"
#include "game.h"
#include "card.h"
#include "hero.h"
#include "comm.h"



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


static char* get_line(char* buf, int size)
{
	int n,c;


	fflush(stdin);

	printf("$ ");

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
	return buf;
}


static void cmd_help_i(const char* cmd);

static int cmd_help(const char** argv, int argc, GameContext* pContext)
{
	printf(PROJ_NAME" "VERSION_STR"\n");
	cmd_help_i(argc > 1 ? argv[1] : NULL);
	return 0;
}

static void param_error(const char* cmd)
{
	printf("execute cmd \'%s\', get wrong params!\n", cmd);
	cmd_help_i(cmd);
}

static int cmd_quit(const char** argv, int argc, GameContext* pContext)
{
	//exit(0);
	return CMD_RET_EXIT;
}


// player number configs
typedef struct tagPlayerConfig
{
	int players;
	int masters;
	int minsters;
	int spies;
	int mutineers;
}PlayerConfig;


static const PlayerConfig s_configs [] = {
	{ 6, 1, 1, 1, 3 },
	{ 7, 1, 2, 1, 3 },
	{ 8, 1, 2, 1, 4 },
};

#define CONFIG_SIZE    (int)(COUNT(s_configs))


const PlayerConfig* select_config(int players)
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


static int cmd_start(const char** argv, int argc, GameContext* pContext)
{

	int ret;
	int cfg;
	const PlayerConfig*  pConfig;
	if(pContext->status != Status_None)
	{
		printf("game has been started.");
		return CMD_RET_SUCC;
	}

	if(argc < 2 || 0 != to_int(argv[1], &cfg))
	{
		param_error(argv[0]);
		return CMD_RET_SUCC;
	}

	// get config struct 
	pConfig = select_config(cfg);

	if(pConfig == NULL )
	{
		param_error(argv[0]);
		return CMD_RET_SUCC;
	}

	// new game
	if(0 != init_game_context(pContext, pConfig->minsters, pConfig->spies, pConfig->mutineers))
	{
		printf("start init new game failed!");
		return CMD_RET_SUCC;
	}

	// game loop

	ret = game_continue(pContext);

	// print game result


	return CMD_RET_SUCC;
}

static int cmd_info(const char** argv, int argc, GameContext* pContext)
{
	if(argc <= 1) // self info
	{
		if(pContext->status == Status_None)
		{
			printf("not in game!\n");
		}
		else
		{
			game_cur_info(pContext);
		}
	}
	else if(!strcmp(argv[1], "event") || !strcmp(argv[1], "e")) // game global info
	{
		printf("current event: No\n");
	}
	else if(!strcmp(argv[1], "game") || !strcmp(argv[1], "g")) // game global info
	{
		if(pContext->status == Status_None)
		{
			printf("not in game!\n");
		}
		else
		{
			game_global_info(pContext);
		}
	}
	else if(!strcmp(argv[1], "gamefull") || !strcmp(argv[1], "gf")) // game full info 
	{

	}
	else if(!strcmp(argv[1], "out") || !strcmp(argv[1], "o")) // list out cards
	{

	}
	else if(!strcmp(argv[1], "player") || !strcmp(argv[1], "p"))  // player info: -n - prev [n] player; +n - next n player; n - player index n info; 
	{
		const char* pp;
		char cp = 0;
		int n = 0;

		if(argc < 3)
		{
			param_error(argv[0]);
			return CMD_RET_SUCC;
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
			return CMD_RET_SUCC;
		}


		while(*pp >= '0' && *pp <= '9')
		{
			n = n * 10 + (*pp -'0');
			pp++;
		}

		if(*pp != 0 )
		{
			param_error(argv[0]);
			return CMD_RET_SUCC;
		}

		if(cp == '+')
		{
			n = (n + pContext->nCurPlayer) % pContext->nPlayerCount;
		}
		else if(cp == '-')
		{
			n = (pContext->nCurPlayer - n % pContext->nPlayerCount + pContext->nPlayerCount) % pContext->nPlayerCount;
		}
		else
		{
			n = n % pContext->nPlayerCount;
		}

		game_other_player_info(pContext, n);
	}
	else if(!strcmp(argv[1], "card") || !strcmp(argv[1], "c"))
	{
		int id;
		const CardConfig *pCardCfg;
		if(argc < 3)
		{
			for(id = 1; id < CardID_Max; id++)
			{
				pCardCfg = get_card_config(id);

				if(pCardCfg)
				{
					printf("(%d) %s, %s\n", pCardCfg->id, pCardCfg->name, card_type_str(pCardCfg->type));
				}
			}
		}
		else
		{
			id = atoi(argv[2]);
			pCardCfg = get_card_config(id);
			if(pCardCfg == NULL)
			{
				printf("no card id  is %d!\n", id);
			}
			else
			{
				printf("(%d) %s, %s\n%s\n", pCardCfg->id, pCardCfg->name, card_type_str(pCardCfg->type), pCardCfg->desc);
			}
		}				
	}
	else if(!strcmp(argv[1], "hero") || !strcmp(argv[1], "h"))
	{
		int id;
		int n;
		const HeroConfig *pHero;
		if(argc < 3)
		{
			for(id = 1; id < HeroID_Max; id++)
			{
				pHero = get_hero_config(id);

				if(pHero)
				{
					printf("(%d) %s, %s, %s, life %d\n", pHero->id, pHero->name, hero_group_str(pHero->group), hero_sex_str(pHero->sex), pHero->life);
				}
			}
		}
		else
		{
			id = atoi(argv[2]);
			pHero = get_hero_config(id);
			if(pHero == NULL)
			{
				printf("no card id  is %d!\n", id);
			}
			else
			{
				printf("(%d) %s, %s, %s, life %d\n", pHero->id, pHero->name, hero_group_str(pHero->group), hero_sex_str(pHero->sex), pHero->life);
				for(n = 0; n < pHero->skillNum; n++)
				{
					printf(" skill (%d) %s: %s\n", n + 1, pHero->skills[n].name, pHero->skills[n].desc);
				}
			}
		}				

	}
	else
	{
		param_error(argv[0]);
		return CMD_RET_SUCC;
	}

	return CMD_RET_SUCC;
}

static int cmd_get(const char** argv, int argc, GameContext* pContext)
{
	if(game_status(pContext) != Status_Round_Get)
	{
		printf("not in get status!\n");
		return CMD_RET_SUCC;
	}


	game_getcard(pContext);

	return CMD_RET_SUCC;
}

static int cmd_out(const char** argv, int argc, GameContext* pContext)
{
	if(game_status(pContext) != Status_Round_Out)
	{
		printf("not in get status!\n");
		return CMD_RET_SUCC;
	}

	int idx;


	if(argc >= 2 && 0 == to_int(argv[1], &idx))
	{
		game_outcard(pContext,idx);

	}
	else
	{
		param_error(argv[0]);
	}


	return CMD_RET_SUCC;
}


static int cmd_useskill(const char** argv, int argc, GameContext* pContext)
{
	if(pContext->status == Status_None)
	{
		printf("not in game!\n");
		return CMD_RET_SUCC;
	}
	int idx;


	if(argc >= 2 && 0 == to_int(argv[1], &idx))
	{
		game_useskill(pContext, idx);

	}
	else
	{
		param_error(argv[0]);
	}
	
	return CMD_RET_SUCC;
}

static int cmd_cancelskill(const char** argv, int argc, GameContext* pContext)
{
	if(pContext->status == Status_None)
	{
		printf("not in game!\n");
		return CMD_RET_SUCC;
	}

	printf("not in skill using!\n");

	return CMD_RET_SUCC;
}


static int cmd_pass(const char** argv, int argc, GameContext* pContext)
{
	return CMD_RET_SUCC;
}

typedef int (*FunCmd)(const char** argv, int argc, GameContext* pContext);


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
		"\t* 6 players: 1 master, 1 minster, 1 spy, 3 mutineers;\n"
		"\t* 7 players: 1 master, 2 minsters, 1 spy, 3 mutineers;\n"
		"\t* 8 players: 1 master, 2 minsters, 1 spy, 4 mutineers."},
	{ "info", "i",	cmd_info, 
		"info/i\n\tshow current player info, handle cards, aromo card, etc.\n"
		"info/i game/g\n\t show the game current global info\n"
		"info/i event/e\n\t show the game current event info\n"
		"info/i player/p <+n|-n|n>\n\t show the other player info\n"
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
		"useskill/u <skill idx> ...\n\use a skill.", 
		NULL},
	{ "cancel", "c",	cmd_cancelskill, 
		"cancel/c \n\tcancel the skill in using.", 
		NULL},
	{ "pass", "p", cmd_pass,
		"pass/p\n\tno out card, so pass this round.", 
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
				printf("%s\n", s_cmdDispatch[n].brief);
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
					printf("%s\n", s_cmdDispatch[n].brief);
					if(s_cmdDispatch[n].detail != NULL)
					{
						printf("%s\n", s_cmdDispatch[n].detail);
					}
				}
				else
				{
					printf("no help info for cmd \'%s\'\n", cmd);
				}
				break;
			}
		}

		if(n >= CMD_NUM)
		{
			printf("cmd \'%s\' not found!\n", cmd);
		}
	}
}


int cmd_loop(GameContext* pContext, const char* alter_text, FunCmdPerProc  funper, void* ud)
{
	char  cmdline[1024];
	const char*  argv[64];
	int   argc;
	char  *next, *w;
	int   n;
	int   ret;

	while( (alter_text ? printf("%s\n", alter_text) : 0), 
		get_line(cmdline, sizeof(cmdline)))
	{
		next =  cmdline;
		argc = 0;
		memset(argv, 0, sizeof(argv));
		while( NULL != (w = get_word(next, &next) ) )
		{
			if(argc < COUNT(argv))
			{
				argv[argc++] = w;
			}
		}

		if(*next != 0)
		{
			printf("error cmd at col %d!\n", (int)(next - cmdline));
		}
		else if(argc > 0)
		{
			ret = CMD_RET_DEF;

			if(funper != NULL)
			{
				ret = (*funper)(argv, argc, pContext, ud);


				if(ret < 0)
					return ret;

			}

			if(ret == CMD_RET_DEF)
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

					ret = (*s_cmdDispatch[n].func)(argv, argc, pContext);

					// return < 0 back to parent caller
					if(ret < 0)
						return ret;
				}
				else
				{
					printf("invalid cmd \'%s\'!\n", argv[0]);
				}
			}
		}
	}
	return CMD_RET_SUCC;
}



