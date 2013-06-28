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


#define MAX_PARAM_NUM   64
#define MAX_CMD_LEN     4096


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

static RESULT cmd_help(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	printf(PROJ_NAME" "VERSION_STR"\n");
	cmd_help_i(argc > 1 ? argv[1] : NULL);
	return R_SUCC;
}

static void param_error(const char* cmd)
{
	printf("execute cmd \'%s\', get wrong params!\n", cmd);
	cmd_help_i(cmd);
}

static RESULT cmd_quit(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	//exit(0);

	if(pContext->status != Status_None)
	{
		longjmp(pContext->__jb__, R_EXIT);
	}

	return R_EXIT;
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


static RESULT cmd_start(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{

	RESULT  ret;
	int cfg;
	const PlayerConfig*  pConfig;
	if(pContext->status != Status_None)
	{
		printf("game has been started.");
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
	ret = init_game_context(pContext, pConfig->minsters, pConfig->spies, pConfig->mutineers);

	if(ret != R_SUCC)
	{
		printf("start init new game failed!");
		return ret;
	}

	// game loop
	ret = (RESULT)setjmp( pContext->__jb__);

	if(ret == 0)
	{
		ret = game_loop(pContext, pEvent);
		//ret = CMD_RET_SUCC;
	}

	// print game result
	return ret;
}

static RESULT cmd_info(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	if(argc <= 1) // self info
	{
		if(pContext->status == Status_None)
		{
			printf("not in game!\n");
			return R_E_STATUS;
		}
		else
		{
			game_cur_info(pContext, pEvent);
		}
	}
	else if(!strcmp(argv[1], "event") || !strcmp(argv[1], "e")) // game global info
	{
		printf("current event: %d\n", pEvent->id);
	}
	else if(!strcmp(argv[1], "game") || !strcmp(argv[1], "g")) // game global info
	{
		if(pContext->status == Status_None)
		{
			printf("not in game!\n");
			return R_E_STATUS;
		}
		else
		{
			game_global_info(pContext, pEvent);
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

		game_other_player_info(pContext, pEvent, n);
	}
	else if(!strcmp(argv[1], "card") || !strcmp(argv[1], "c"))
	{
		int id;
		const CardConfig *pCardCfg;
		if(argc < 3)
		{
			for(id = 1; id < CardID_Max; id++)
			{
				pCardCfg = get_card_config((CardID)id);

				if(pCardCfg)
				{
					printf("(%d) %s, %s\n", pCardCfg->id, pCardCfg->name, card_type_str(pCardCfg->type));
				}
			}
		}
		else
		{
			id = atoi(argv[2]);
			pCardCfg = get_card_config((CardID)id);
			if(pCardCfg == NULL)
			{
				printf("no card id  is %d!\n", id);
				return R_E_PARAM;
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
				pHero = get_hero_config((HeroID)id);

				if(pHero)
				{
					printf("(%d) %s, %s, %s, life %d\n", pHero->id, pHero->name, hero_group_str(pHero->group), hero_sex_str(pHero->sex), pHero->life);
				}
			}
		}
		else
		{
			id = atoi(argv[2]);
			pHero = get_hero_config((HeroID)id);
			if(pHero == NULL)
			{
				printf("no card id  is %d!\n", id);
				return R_E_PARAM;
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
		return R_E_PARAM;
	}

	return R_SUCC;
}

static RESULT cmd_get(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	if(game_status(pContext) != Status_Round_Get)
	{
		printf("not in get status!\n");
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
	if(game_status(pContext) != Status_Round_Out)
	{
		printf("not in get status!\n");
		return R_E_STATUS;
	}

	int idx[MAX_PARAM_NUM];
	int cnt = 0;
	int n;

	if(argc < 2)
	{
		param_error(argv[0]);
		return R_E_PARAM;
	}



	for(n = 1; n < argc && cnt < MAX_PARAM_NUM; n++)
	{
		if(0 != to_int(argv[1], &idx[cnt]))
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
	if(pContext->status == Status_None)
	{
		printf("not in game!\n");
		return R_E_STATUS;
	}
	int idx;


	if(argc >= 2 && 0 == to_int(argv[1], &idx))
	{
		return  game_cmd_useskill(pContext, pEvent, idx);

	}
	else
	{
		param_error(argv[0]);
		return R_E_PARAM;
	}
	
	return R_SUCC;
}


static RESULT cmd_useweapon(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	// no need param
	return R_SUCC;
}

static RESULT cmd_cancelskill(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	if(pContext->status == Status_None)
	{
		printf("not in game!\n");
		return R_E_STATUS;
	}


	return game_cmd_cancelskill(pContext, pEvent);

	//printf("not in skill using!\n");

	//return CMD_RET_SUCC;
}


static RESULT cmd_pass(const char** argv, int argc, GameContext* pContext, GameEventContext* pEvent)
{
	if(pContext->status == Status_None)
	{
		printf("not in game!\n");
		return R_E_STATUS;
	}


	return game_cmd_pass(pContext, pEvent);

	//return CMD_RET_SUCC;
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
		"useskill/u <skill idx> ...\n\tuse a skill.", 
		NULL},
	{ "usesweapon", "w",	cmd_useweapon, 
		"usesweapon/w ...\n\tuse the weapon card - some weapon can active in out card round - as a skill.", 
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


RESULT cmd_loop(GameContext* pContext, GameEventContext* pEvent, const char* strAlter)
{
	char  cmdline[MAX_CMD_LEN];
	const char*  argv[MAX_PARAM_NUM];
	int   argc;
	char  *next, *w;
	int   n;
	RESULT   ret;

	while( (strAlter ? printf("%s\n", strAlter) : 0), 
		get_line(cmdline, sizeof(cmdline)))
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
			printf("error cmd at col %d!\n", (int)(next - cmdline));
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

					// return follow code back to parent caller
					switch(ret)
					{
					case R_BACK:    // spec return R_BACK means back to caller with success
						return R_SUCC;
					case R_EXIT:
					case R_CANCEL:
					case R_RETRY:
						return ret;
					default:
						// continue loop ...
						break;
					}
				}
				else
				{
					printf("invalid cmd \'%s\'!\n", argv[0]);
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
			printf("%s\n", strAlter);

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

			printf(" %s\n", buffer);


		}

		printf("input select: ");

		fflush(stdin);

		if(NULL == fgetln(buffer, sizeof(buffer), stdin))
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
