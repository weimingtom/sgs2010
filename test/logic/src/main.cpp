#include "config.h"
#include "card.h"
#include "player.h"


#define PROJ_NAME    "SGS. Test Logic"
#define VERSION_STR  "v0.1.0"


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

#define CONFIG_SIZE    (sizeof(s_configs)/sizeof(s_configs[0]))


static const PlayerConfig* select_config()
{
	int n; 
	const PlayerConfig* pConfig;

	while(true)
	{
		printf("Please input players config: \n");

		for(n = 0; n < CONFIG_SIZE; n++)
		{
			pConfig = &s_configs[n];
			printf("  (%d) Total %d: %d masters, %d minsters, %d spies, %d mutineers;\n", n+1, 
				pConfig->players, pConfig->masters, pConfig->minsters, pConfig->spies, pConfig->mutineers);
		}
		printf("CONFIG> ");

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
	const PlayerConfig* pConfig;

	CardStack  stCardStack;

	srand((int)time(NULL));

	printf(PROJ_NAME" "VERSION_STR"\n");


	pConfig = select_config();

	printf("your select is: Total %d: %d masters, %d minsters, %d spies, %d mutineers;\n",
		pConfig->players, pConfig->masters, pConfig->minsters, pConfig->spies, pConfig->mutineers);

	players = pConfig->players;


	init_card_stack(&stCardStack);
	card_stack_random(&stCardStack);
	card_stack_dump(&stCardStack);


	return 0;
}




