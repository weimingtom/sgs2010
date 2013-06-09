#include "config.h"
#include "game.h"
#include "comm.h"
#include "hero.h"

#define NEXT_PLAYER(pGame)

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


	pGame->status = Status_FirstGetCard;
	pGame->nRoundNum = 0;
	pGame->nRoundPlayer = 0;
	pGame->nCurPlayer = 0;


	return 0;
}


static void discard_stack_card(GameContext* pGame)
{
	if(pGame->cardStack.count > 0)
	{
		pGame->cardStack.count--;

		pGame->cardOut.cards[pGame->cardOut.count] = pGame->cardStack.cards[pGame->cardStack.count];
		pGame->cardOut.count++;
	}
}

static int refresh_card_stack(GameContext* pGame)
{
	while(pGame->cardStack.count > 0)
	{
		discard_stack_card(pGame);
	}

	pGame->cardStack = pGame->cardOut;
	pGame->cardOut->count = 0;

	card_stack_random(&pGame->cardStack);

	return 0;
}

// player get card
static int get_hand_card(GameContext* pGame)
{
	if(pGame->players[pGame->nCurPlayer].nHandCardNum >= MAX_HAND_CARD)
	{
		printf("get_hand_card: player [%d] handle card id full!\n", pGame->nCurPlayer);
		return -1;
	}

	if(pGame->cardStack.count  == 0)
	{
		// reflush card stack
		refresh_card_stack(pGame);
	}

	pGame->players[pGame->nCurPlayer].stHandCards

	return 0;
}


// the new game first step , all player get 4 cards
static void get_first_hand_card(GameContext* pGame)
{
	int n, k;
	// first player will get card
	for(n = 0; n < pGame->nPlayerCount; n++)
	{
		if(pGame->players[n].id == PlayerID_Master)
		{
			pGame->nCurPlayer = n;
			break;
		}
	}

	for(n = 0; n < pGame->nPlayerCount; n++)
	{
		// get 4 card each player
		for(k = 0; k < 4; k++)
		{
			get_hand_card(pGame);
		}

		// next player
		pGame->nCurPlayer = (pGame->nCurPlayer + 1) % pGame->nPlayerCount;
	}

	pGame->nRoundNum = 1;
	pGame->nRoundPlayer = pGame->nCurPlayer;
	pGame->status  = Status_Round_Begin;
}



int game_step(GameContext* pGame)
{
	switch(pGame->status)
	{
	case Status_FirstGetCard:
		get_first_hand_card();
		break;
	case Status_Round_Begin:

	}

	return 0;
}


