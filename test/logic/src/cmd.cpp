#include "config.h"
#include "cmd.h"
#include "game.h"
#include "card.h"
#include "hero.h"



#define C2I(c)  ((int)(unsigned char)(c))

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

enum {
	GET_INT_SUCC = 0,
	GET_INT_ERR = -1,
	GET_INT_EOL  = -2,
};

static int get_int(char* cmd, int* val, char** next)
{
	char* p;
	char* ret = get_word(cmd, next);

	if(ret == NULL)
		return GET_INT_EOL;


	long lr = strtol(ret, &p, 0);

	if(*p)
		return GET_INT_ERR;

	*val = lr;

	return GET_INT_SUCC;
}


static char* get_line(char* buf, int size)
{
	int n,c;
	printf("$ ");

	for(n = 0; n < size-1; n++)
	{
		c = getc(stdin);
		if(c == EOF || c=='\n')
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
static int cmd_help(char* param, GameContext* pContext)
{
	printf(PROJ_NAME" "VERSION_STR"\n");
	cmd_help_i(get_word(param, NULL));
	return 0;
}

static void param_error(const char* cmd)
{
	printf("execute cmd \'%s\', get wrong params!\n", cmd);
	cmd_help_i(cmd);
}

static int cmd_quit(char* param, GameContext* pContext)
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


static int cmd_start(char* param, GameContext* pContext)
{

	int ret;
	int cfg;
	const PlayerConfig*  pConfig;
	if(pContext->status != Status_None)
	{
		printf("game has been started.");
		return CMD_RET_SUCC;
	}

	// read cfg num
	ret = get_int(param, &cfg, NULL);

	if(ret != GET_INT_SUCC)
	{
		param_error("start");
		return CMD_RET_SUCC;
	}

	// get config struct 
	pConfig = select_config(cfg);

	if(pConfig == NULL )
	{
		param_error("start");
		return CMD_RET_SUCC;
	}

	// new game
	if(0 != init_game_context(pContext, pConfig->minsters, pConfig->spies, pConfig->mutineers))
	{
		printf("start init new game failed!");
		return CMD_RET_SUCC;
	}

	// game loop

	ret = game_loop(pContext);

	// print game result


	return CMD_RET_SUCC;
}

static int cmd_info(char* param, GameContext* pContext)
{
	char* w = get_word(param, &param);
	if(w == NULL) // self info
	{
		if(pContext->status == Status_None)
		{
			printf("not in game!\n");
		}
		else
		{
			int n;
			char buf[128];
			Player* p = &pContext->players[pContext->nCurPlayer];
			// base info
			printf("Current Player: %d, %s, %s, life: %d/%d\n", pContext->nCurPlayer, player_id_str(p->id), p->name, p->curLife, p->maxLife);
			// hand cards
			printf("Hand cards (%d):\n",  p->nHandCardNum);
			for(n = 0; n < p->nHandCardNum; n++)
			{
				//if(n > 0 && n % 4 == 0) printf("\n           ");
				printf("  (%d) %s;\n", n+1, card_str(&p->stHandCards[n], buf, sizeof(buf)));
			}

			// equiped cards
			printf("Weapon : %s\n",  card_str_def(&p->stEquipCard[EquipIdx_Weapon], buf, sizeof(buf), "None") );
			printf("Armor : %s\n",  card_str_def(&p->stEquipCard[EquipIdx_Armor], buf, sizeof(buf), "None") );
			printf("Horse(+1) : %s\n", card_str_def(&p->stEquipCard[EquipIdx_HorseInc], buf, sizeof(buf), "None") );
			printf("Horse(-1) : %s\n",  card_str_def(&p->stEquipCard[EquipIdx_HorseDec], buf, sizeof(buf), "None") );

			// judgment cards
			printf("Judgment cards (%d):\n",  p->nJudgmentCardNum);
			for(n = 0; n < p->nJudgmentCardNum; n++)
			{
				//if(n > 0 && n % 4 == 0) printf("\n           ");
				printf("  (%d) %s;\n", n+1, card_str(&p->stJudgmentCards[n], buf, sizeof(buf)));
			}
		}
	}
	else if(!strcmp(w, "event") || !strcmp(w, "e")) // game global info
	{
		printf("current event: No\n");
	}
	else if(!strcmp(w, "game") || !strcmp(w, "g")) // game global info
	{
		if(pContext->status == Status_None)
		{
			printf("not in game!\n");
		}
		else
		{
			int n, k;
			char buf[128];
			Player* p;
			printf("game info: \n");

			printf("(*) %d players (%d+%d+%d+%d): \n", pContext->nPlayerCount, 1, pContext->nMinsterCount, pContext->nSpyCount, pContext->nMutineerCount);
			
			for(k = 0; k < pContext->nPlayerCount; k++)
			{
				p = &pContext->players[k];

				printf("  (%d) %s%s +%d -%d %s, %s, life: %d/%d, hand cards: %d\n", k + 1, pContext->nRoundPlayer == k ? "R":"-", pContext->nCurPlayer == k ? "C":"-",
					(k - pContext->nCurPlayer + pContext->nPlayerCount) % pContext->nPlayerCount, 
					(pContext->nCurPlayer - k + pContext->nPlayerCount) % pContext->nPlayerCount, 
					player_id_str(p->id), p->name, p->curLife, p->maxLife, p->nHandCardNum);

				// equiped cards
				printf("    Weapon : %s\n",  card_str_def(&p->stEquipCard[EquipIdx_Weapon], buf, sizeof(buf), "None") );
				printf("    Armor : %s\n",  card_str_def(&p->stEquipCard[EquipIdx_Armor], buf, sizeof(buf), "None") );
				printf("    Horse(+1) : %s\n", card_str_def(&p->stEquipCard[EquipIdx_HorseInc], buf, sizeof(buf), "None") );
				printf("    Horse(-1) : %s\n",  card_str_def(&p->stEquipCard[EquipIdx_HorseDec], buf, sizeof(buf), "None") );

				// judgment cards
				printf("    Judgment cards (%d):\n",  p->nJudgmentCardNum);
				for(n = 0; n < p->nJudgmentCardNum; n++)
				{
					//if(n > 0 && n % 4 == 0) printf("\n           ");
					printf("      (%d) %s;\n", n+1, card_str(&p->stJudgmentCards[n], buf, sizeof(buf)));
				}
			}
			printf("(*) stack cards: %d\n", pContext->cardStack.count);
			printf("(*) out   cards: %d\n", pContext->cardOut.count);

		}
	}
	else if(!strcmp(w, "gamefull") || !strcmp(w, "gf")) // game full info 
	{

	}
	else if(!strcmp(w, "out") || !strcmp(w, "o")) // list out cards
	{

	}
	else if(!strcmp(w, "player") || !strcmp(w, "p"))  // player info: -n - prev [n] player; +n - next n player; n - player index n info; 
	{
		const char* pp = get_word(param, &param);
		char cp = 0;
		int n = 0;
		char buf[128];

		if(pp == NULL)
		{
			param_error("info");
			return CMD_RET_SUCC;
		}

		if(*pp == '+' || *pp == '-')
		{
			cp = *pp;
			pp++;
		}


		if(*pp ==0 )
		{
			param_error("info");
			return CMD_RET_SUCC;
		}


		while(*pp >= '0' && *pp <= '9')
		{
			n = n * 10 + (*pp -'0');
			pp++;
		}

		if(*pp != 0 )
		{
			param_error("info");
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

		Player* pPlayer = &pContext->players[n];

		printf("  (%d) %s%s +%d -%d %s, %s, life: %d/%d, hand cards: %d\n", n + 1, pContext->nRoundPlayer == n ? "R":"-", pContext->nCurPlayer == n ? "C":"-",
			(n - pContext->nCurPlayer + pContext->nPlayerCount) % pContext->nPlayerCount, 
			(pContext->nCurPlayer - n + pContext->nPlayerCount) % pContext->nPlayerCount, 
			player_id_str(pPlayer->id), pPlayer->name, pPlayer->curLife, pPlayer->maxLife, pPlayer->nHandCardNum);

		// equiped cards
		printf("    Weapon : %s\n",  card_str_def(&pPlayer->stEquipCard[EquipIdx_Weapon], buf, sizeof(buf), "None") );
		printf("    Armor : %s\n",  card_str_def(&pPlayer->stEquipCard[EquipIdx_Armor], buf, sizeof(buf), "None") );
		printf("    Horse(+1) : %s\n", card_str_def(&pPlayer->stEquipCard[EquipIdx_HorseInc], buf, sizeof(buf), "None") );
		printf("    Horse(-1) : %s\n",  card_str_def(&pPlayer->stEquipCard[EquipIdx_HorseDec], buf, sizeof(buf), "None") );

		// judgment cards
		printf("    Judgment cards (%d):\n",  pPlayer->nJudgmentCardNum);
		for(n = 0; n < pPlayer->nJudgmentCardNum; n++)
		{
			//if(n > 0 && n % 4 == 0) printf("\n           ");
			printf("      (%d) %s;\n", n+1, card_str(&pPlayer->stJudgmentCards[n], buf, sizeof(buf)));
		}
	}
	else if(!strcmp(w, "card") || !strcmp(w, "c"))
	{
		const char* pp = get_word(param, &param);
		int id;
		const CardConfig *pCardCfg;
		if(pp ==NULL)
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
			id = atoi(pp);
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
	else if(!strcmp(w, "hero") || !strcmp(w, "h"))
	{
		const char* pp = get_word(param, &param);
		int id;
		int n;
		const HeroConfig *pHero;
		if(pp ==NULL)
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
			id = atoi(pp);
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
		param_error("info");
		return CMD_RET_SUCC;
	}

	return CMD_RET_SUCC;
}

static int cmd_get(char* param, GameContext* pContext)
{
	return CMD_RET_SUCC;
}

static int cmd_out(char* param, GameContext* pContext)
{
	return CMD_RET_SUCC;
}

static int cmd_pass(char* param, GameContext* pContext)
{
	return CMD_RET_SUCC;
}

typedef int (*FunCmd)(char* param, GameContext* pContext);


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


int cmd_loop(GameContext* pContext)
{
	char  cmdline[1024];
	char  *next, *w;
	int   n;
	int   ret;

	fflush(stdin);
	while(get_line(cmdline, sizeof(cmdline)))
	{
		w = get_word(cmdline, &next);

		if(w != NULL)
		{	
			for(n= 0; n < CMD_NUM; n++)
			{
				if(!strcmp(w, s_cmdDispatch[n].name) || (s_cmdDispatch[n].sort_name && !strcmp(w, s_cmdDispatch[n].sort_name)))
				{
					break;
				}
			}

			if(n < CMD_NUM)
			{
				ret = (*s_cmdDispatch[n].func)(next, pContext);

				// return < 0 back to parent caller
				if(ret < 0)
					return ret;
			}
			else
			{
				printf("invalid cmd \'%s\'!\n", w);
			}
		}
		else
		{
			if(*next != 0)
			{
				printf("error cmd at col %d!\n", (int)(next-cmdline));
			}
		}
	}
	return 0;
}



