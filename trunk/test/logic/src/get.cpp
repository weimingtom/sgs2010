// get.cpp 


#include "config.h"
#include "comm.h"
#include "game.h"
#include "event.h"
#include "card.h"
#include "get.h"
#include "cmd.h"




// discard all card stack cards to out stack
static void discard_stack_card(GameContext* pGame)
{
	Card  card;
	if(pGame->cardStack.count > 0)
	{
		card_stack_pop(&pGame->cardStack, &card);
		card_stack_push(&pGame->cardOut, &card);
	}
}

static void refresh_card_stack(GameContext* pGame)
{
	while(pGame->cardStack.count > 0)
	{
		discard_stack_card(pGame);
	}

	pGame->cardStack = pGame->cardOut;
	card_stack_clear(&pGame->cardOut);

	card_stack_random(&pGame->cardStack);

	printf("card stack refresh: count=%d\n", pGame->cardStack.count);
}


// player get card
static RESULT get_hand_card(GameContext* pGame)
{
	Player* pPlayer = &pGame->players[pGame->nCurPlayer];
	if(pPlayer->nHandCardNum >= MAX_HAND_CARD)
	{
		printf("get_hand_card: player [%d] handle card full!\n", pGame->nCurPlayer);
		return R_E_FAIL;
	}

	if(card_stack_empty(&pGame->cardStack) )
	{
		// reflush card stack
		refresh_card_stack(pGame);
	}

	if(0 != card_stack_pop(&pGame->cardStack, &pPlayer->stHandCards[pPlayer->nHandCardNum]))
	{
		printf("get_hand_card: player [%d] get card failed!\n", pGame->nCurPlayer);
		return R_E_FAIL;
	}
	pPlayer->nHandCardNum++;

	printf("player [%d] [%s] get a card, hand card count [%d].\n", pGame->nCurPlayer, pPlayer->name, pPlayer->nHandCardNum);

	return R_SUCC;
}


// the new game first step , all player get 4 cards
RESULT game_first_getcard(GameContext* pGame)
{
	int n, k;

	printf("the first time each player get 4 cards.\n");

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

	return R_SUCC;
}

static RESULT  trigger_pergetcard_event(GameContext* pGame, GameEventContext* pEvent)
{
	GameEventContext  stEvent;
	INIT_EVENT(&stEvent, GameEvent_PostGetCard, pGame->nCurPlayer, 0, pEvent);
	trigger_game_event(pGame, &stEvent);

	return stEvent.result;
}

static RESULT  trigger_postgetcard_event(GameContext* pGame, GameEventContext* pEvent)
{
	GameEventContext  stEvent;
	INIT_EVENT(&stEvent, GameEvent_PerGetCard, pGame->nCurPlayer, 0, pEvent);
	trigger_game_event(pGame, &stEvent);

	return stEvent.result;

}


RESULT game_getcard(GameContext* pGame, GameEventContext* pEvent, int num)
{
	int n;

	if(game_status(pGame) == Status_Round_Get)
	{
		// in round get, must get 2 card

		if(R_CANCEL != trigger_pergetcard_event(pGame, pEvent))
		{
			// get 2 cards
			for(n = 0; n < 2; n++)
			{
				get_hand_card(pGame);

			}

			trigger_postgetcard_event(pGame, pEvent);
		}

		// goto out status

		pGame->status = Status_Round_Out;

		return R_BACK;
	}
	else if(pEvent->id == GameEvent_PassiveGetCard)
	{
		// passive get wihtout per/postgetcard event
		// get event.num cards
		for(n = 0; n < pEvent->num; n++)
		{
			get_hand_card(pGame);
		}

		return R_BACK;
	}
	
	return R_E_STATUS;
}




