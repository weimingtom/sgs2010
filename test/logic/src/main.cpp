#include "config.h"
#include "card.h"
#include "card_stack.h"
#include "player.h"
#include "game.h"
#include "cmd.h"
#include "comm.h"
#include "event.h"
#include "script.h"

/*

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

#define CONFIG_SIZE   (int)(COUNT(s_configs))


static const PlayerConfig* select_config()
{
	int n; 
	const PlayerConfig* pConfig;

	while(1)
	{
		MSG_OUT("Please input players config: \n");

		for(n = 0; n < CONFIG_SIZE; n++)
		{
			pConfig = &s_configs[n];
			MSG_OUT("  (%d) Total %d: %d masters, %d minsters, %d spies, %d mutineers;\n", n+1, 
				pConfig->players, pConfig->masters, pConfig->minsters, pConfig->spies, pConfig->mutineers);
		}
		MSG_OUT("$ ");

		n = -1;
		scanf("%d", &n);

		if(n >= 1 && n <= CONFIG_SIZE)
		{
			pConfig = &s_configs[n-1];
			break;
		}

		MSG_OUT("error input select! \n");

		fflush(stdin);
	}

	return pConfig;
}

*/


int main(int argc, char** argv)
{
	//int players = 0;
	//const PlayerConfig* pConfig;

	GameContext   stGameContext;
	GameEventContext  stEvent;

	srand((int)time(NULL));

	MSG_OUT(PROJ_NAME" "VERSION_STR"\n");

	if(R_SUCC != init_game_script())
	{
		return 1;
	}


	ST_ZERO(stGameContext);

	INIT_EVENT(&stEvent, GameEvent_None, INVALID_PLAYER, INVALID_PLAYER, NULL);

	cmd_loop(&stGameContext, &stEvent, YES, NULL);

	close_game_script();

	return 0;
}




