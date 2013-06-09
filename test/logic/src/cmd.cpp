#include "config.h"
#include "cmd.h"
#include "game.h"

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
	if(0 != init_game_context(pContext, pConfig->mutineers, pConfig->spies, pConfig->mutineers))
	{
		printf("start init new game failed!");
		return CMD_RET_SUCC;
	}

	// start first step
	game_step(pContext);

	return CMD_RET_SUCC;
}

static int cmd_info(char* param, GameContext* pContext)
{
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
		"info/i game\n\t show the game current global info", 
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



