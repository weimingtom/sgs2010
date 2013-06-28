#include "config.h"
#include "game.h"
#include "card.h"
#include "stack.h"
#include "card_stack.h"
#include "comm.h"




// discard all card stack cards to out stack
static void discard_stack_card(GameContext* pGame)
{
	Card  card;
	while(pGame->stGetCardStack.count > 0)
	{
		card_stack_pop(&pGame->stGetCardStack, &card);
		card_stack_push(&pGame->stGetCardStack, &card);
	}
}

static void refresh_card_stack(GameContext* pGame)
{
	while(pGame->stGetCardStack.count > 0)
	{
		discard_stack_card(pGame);
	}

	pGame->stGetCardStack = pGame->stDiscardCardStack;
	card_stack_clear(&pGame->stDiscardCardStack);

	card_stack_random(&pGame->stGetCardStack);

	MSG_OUT("card stack refresh: count=%d\n", pGame->stGetCardStack.count);
}



// player get card
RESULT game_pop_stack_card(GameContext* pGame, Card* pCard)
{
	if(card_stack_empty(&pGame->stGetCardStack) )
	{
		// reflush card stack
		refresh_card_stack(pGame);
	}

	if(R_SUCC != card_stack_pop(&pGame->stGetCardStack, pCard))
	{
		MSG_OUT("game_pop_stack_card:  failed! stack size=%d!\n", pGame->stGetCardStack.count);
		return R_E_FAIL;
	}

	//MSG_OUT("player [%d] [%s] get a card, hand card count [%d].\n", pGame->nCurPlayer, pPlayer->name, pPlayer->nHandCardNum);

	return R_SUCC;
}


RESULT game_push_stack_card(GameContext* pGame, const Card* pCard, YESNO  top)
{
	return R_SUCC;
}
