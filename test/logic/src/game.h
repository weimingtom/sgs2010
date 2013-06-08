#ifndef __GAME_H__
#define __GAME_H__


#include "config.h"
#include "player.h"
#include "card_stack.h"




typedef struct tagGameContext
{
	int        nPlayerCount;
	int        nMinsterCount;
	int        nSpyCount;
	int        nMutineerCount;
	Player     players[MAX_PLAYER_NUM];
	CardStack  cardStack;
	CardStack  cardOut;
	
} GameContext;




int init_game_context(GameContext* pGame, int minsters, int spies, int mutineers);



#endif /* __GAME_H__ */


