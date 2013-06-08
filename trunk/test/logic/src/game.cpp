#include "config.h"
#include "game.h"
#include "comm.h"
#include "hero.h"


int init_game_context(GameContext* pGame, int minsters, int spies, int mutineers)
{
	int n, c;
	int pids[MAX_PLAYER_NUM];

	if(minsters < 1 || spies < 1 || mutineers < 1 || minsters + spies + mutineers + 1 > MAX_PLAYER_NUM)
		return -1;

	memset(pGame, 0, sizeof(*pGame));

	pGame->nPlayerCount = minsters + spies + mutineers + 1;
	pGame->nMinsterCount = minsters;
	pGame->nSpyCount = spies;
	pGame->nMutineerCount = mutineers;
	
	// init players
	c = 0;
	pids[c++] = PlayerID_Master;
	for(n = 0; n < minsters; n++) pids[c++] = PlayerID_Minster;
	for(n = 0; n < spies; n++) pids[c++] = PlayerID_Spy;
	for(n = 0; n < mutineers; n++) pids[c++] = PlayerID_Mutineer;

	rand_array_i(pids, c, c * 2);

	for(n = 0; n < pGame->nPlayerCount; ++n)
	{
		init_player(&pGame->players[n], pids[n], random_hero_id(pids[n] == PlayerID_Master));
	}

	// init card stack

	init_card_stack(&pGame->cardStack);
	card_stack_random(&pGame->cardStack);

	// clear out cards;
	pGame->cardOut.count = 0;



	return 0;
}


