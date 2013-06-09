#include "config.h"
#include "card.h"
#include "card_stack.h"
#include "player.h"
#include "game.h"
#include "cmd.h"



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

#define CONFIG_SIZE    (COUNT(s_configs))


static const PlayerConfig* select_config()
{
	int n; 
	const PlayerConfig* pConfig;

	while(1)
	{
		printf("Please input players config: \n");

		for(n = 0; n < CONFIG_SIZE; n++)
		{
			pConfig = &s_configs[n];
			printf("  (%d) Total %d: %d masters, %d minsters, %d spies, %d mutineers;\n", n+1, 
				pConfig->players, pConfig->masters, pConfig->minsters, pConfig->spies, pConfig->mutineers);
		}
		printf("$ ");

		n = -1;
		scanf("%d", &n);

		if(n >= 1 && n <= CONFIG_SIZE)
		{
			pConfig = &s_configs[n-1];
			break;
		}

		printf("error input select! \n");

		fflush(stdin);
	}

	return pConfig;
}


int main(int argc, char** argv)
{
	int players = 0;
	//const PlayerConfig* pConfig;

	GameContext   stGameContext;

	srand((int)time(NULL));

	printf(PROJ_NAME" "VERSION_STR"\n");


	//pConfig = select_config();

	//printf("your select is: Total %d: %d masters, %d minsters, %d spies, %d mutineers;\n",
	//	pConfig->players, pConfig->masters, pConfig->minsters, pConfig->spies, pConfig->mutineers);

	//players = pConfig->players;


	//init_game_context(&stGameContext, pConfig->minsters, pConfig->spies, pConfig->mutineers);

	//card_stack_dump(&stGameContext.cardStack);

	ST_ZREO(stGameContext);

	cmd_loop(&stGameContext);

	return 0;
}




