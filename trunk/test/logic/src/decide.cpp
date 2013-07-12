#include "config.h"
#include "game.h"
#include "event.h"
#include "card.h"
#include "stack.h"
#include "decide.h"
#include "discard.h"



YESNO game_decide_card(GameContext* pGame, GameEventContext* pParentEvent,int player, const CardPattern* pPattern)
{
	GameEventContext  event;
	RESULT ret;
	Card   stCard;

	// GameEvent_PerDecideCard
	INIT_EVENT(&event, GameEvent_PerDecideCard, player, 0, pParentEvent);

	trigger_game_event(pGame, &event);

	if(event.result == R_CANCEL)
		return NO;

	if(event.result == R_SKIP)
		return YES;

	ret = game_pop_stack_card(pGame, &stCard);

	CHECK_RET(ret, NO);


	// GameEvent_PerDecideCardCalc 
	INIT_EVENT(&event, GameEvent_PerDecideCardCalc, player, 0, pParentEvent);
	event.pCard = &stCard;
	trigger_game_event(pGame, &event);

	ret = card_match(&stCard, 1, pPattern, 1);

	// GameEvent_PostDecideCard
	INIT_EVENT(&event, GameEvent_PostDecideCard, player, 0, pParentEvent);
	event.pCard = &stCard;
	trigger_game_event(pGame, &event);


	if(CARD_VALID(&stCard))
	{
		game_add_discard_cur(pGame, &stCard);
	}

	return NO;
}


