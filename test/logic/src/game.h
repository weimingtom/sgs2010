#ifndef __GAME_H__
#define __GAME_H__


#include "config.h"
#include "player.h"
#include "card_stack.h"


enum Status
{
	Status_None = 0, // not start game yet
	Status_FirstGetCard=1, // the first time all player get card
	Status_Round_Begin,
	Status_Round_Judge,
	Status_Round_Get,
	Status_Round_Out,
	Status_Round_Discard,
	Status_Round_End,
};





typedef struct tagGameContext
{
	int        nPlayerCount;
	int        nMinsterCount;
	int        nSpyCount;
	int        nMutineerCount;
	Player     players[MAX_PLAYER_NUM];
	CardStack  cardStack;
	CardStack  cardOut;
	int        nRoundNum;
	int        nRoundPlayer;
	int        nCurPlayer;
	int        status;
} GameContext;




int init_game_context(GameContext* pGame, int minsters, int spies, int mutineers);


int game_step(GameContext* pGame);

#endif /* __GAME_H__ */


