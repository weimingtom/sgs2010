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
	while(pGame->get_card_stack.count > 0)
	{
		card_stack_pop(&pGame->get_card_stack, &card);
		card_stack_push(&pGame->get_card_stack, &card);
	}
}

static void refresh_card_stack(GameContext* pGame)
{
	while(pGame->get_card_stack.count > 0)
	{
		discard_stack_card(pGame);
	}

	pGame->get_card_stack = pGame->discard_card_stack;
	card_stack_clear(&pGame->discard_card_stack);

	card_stack_random(&pGame->get_card_stack);

	MSG_OUT("card stack refresh: count=%d\n", pGame->get_card_stack.count);
}



// player get card
RESULT game_pop_stack_card(GameContext* pGame, Card* pCard)
{
	if(card_stack_empty(&pGame->get_card_stack) )
	{
		// reflush card stack
		refresh_card_stack(pGame);
	}

	if(R_SUCC != card_stack_pop(&pGame->get_card_stack, pCard))
	{
		MSG_OUT("game_pop_stack_card:  failed! stack size=%d!\n", pGame->get_card_stack.count);
		return R_E_FAIL;
	}

	//MSG_OUT("player [%d] [%s] get a card, hand card count [%d].\n", pGame->cur_player, pPlayer->name, pPlayer->hand_card_num);

	return R_SUCC;
}


RESULT game_push_stack_card(GameContext* pGame, const Card* pCard, YESNO  top)
{
	return R_SUCC;
}
