// out.cpp 
#include "config.h"
#include "game.h"
#include "event.h"
#include "cmd.h"
#include "comm.h"
#include "out.h"
#include "card.h"
//#include "hero.h"
#include "player.h"


// round out check event stack: GameEvent_None=>GameEvent_RoundOut=>GameEvent_OutCardCheck
// other out check event stack:.... =>GameEvent_OutCardCheck
static YESNO out_card_check_event(GameContext* pGame, GameEventContext* pParentEvent, int player, OutCard* pOut)
{
	const CardConfig* pCardConfig;
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_OutCardCheck, player, target, pParentEvent);
	event.pOut = pOut;

	pCardConfig = get_card_config(pOut->vcard.card.id);

	if(pCardConfig == NULL || pCardConfig->check == NULL)
		return NO;

	return (*pCardConfig->check)(pGame, &event, player);
}




static RESULT per_out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, OutCard* pOut)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PerOutCard, trigger, target, pParentEvent);
	event.pOut = pOut;

	trigger_game_event(pGame, &event);

	return event.result;
}

static RESULT post_out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, OutCard* pOut)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PostOutCard, trigger, target, pParentEvent);
	event.pOut = pOut;

	trigger_game_event(pGame, &event);

	return event.result;	
}

static RESULT remove_out_card(GameContext* pGame, OutCard* pOut)
{
	int n;
	char buf[128];
	Player* pPlayer = GAME_PLAYER(pGame, pOut->supply);
	
	if(pOut->nrcard == 0)
	{
		if(0 != player_remove_card(pPlayer, pOut->vcard.where, pOut->vcard.pos, NULL))
		{
			printf("remove out card [%s]  from player [%d] pos [%d] failed ", card_str(&pOut->vcard.card, buf, sizeof(buf)), pOut->supply, pOut->vcard.pos);
			return R_E_FAIL;
		}
	}
	else
	{
		for(n = 0; n < pOut->nrcard; n++)
		{
			player_remove_card(pPlayer, pOut->vcard.where, pOut->rcards[n].pos, NULL);
			printf("remove out card [%s] from player [%d] failed ", card_str(&pOut->rcards[n].card, buf, sizeof(buf)), pOut->supply, pOut->rcards[n].pos);
			return R_E_FAIL;
		}
	}
	return R_SUCC;
}

static RESULT add_out_stack(GameContext* pGame, OutCard* pOut)
{
	int n;
	char buf[128];
	if(pOut->nrcard > 0)
	{
		for(n = 0; n < pOut->nrcard; n++)
		{
			if(0 != card_stack_push(&pGame->cardOut, &pOut->rcards[n].card))
			{
				printf("add out card [%s] failed ", card_str(&pOut->rcards[n].card, buf, sizeof(buf)));
				return R_E_FAIL;
			}
		}
	}
	else if(pOut->vcard.card.id != CardID_None)
	{
		if(0 != card_stack_push(&pGame->cardOut, &pOut->vcard.card))
		{
			printf("add out card [%s] failed ", card_str(&pOut->vcard.card, buf, sizeof(buf)));
			return R_E_FAIL;
		}
	}
	return R_SUCC;
}

static RESULT round_out_card(GameContext* pGame, GameEventContext* pParentEvent, int player, OutCard* pOut)
{
	RESULT ret;
	const CardConfig* pCardConfig;
	GameEventContext  stEvent;

	if(R_CANCEL == per_out_card(pGame, pParentEvent, player, 0, pOut))
		return R_CANCEL;

	INIT_EVENT(&stEvent, GameEvent_RoundOutCard, player, 0, pParentEvent);

	// out procedure
	pCardConfig = get_card_config(pOut->vcard.card.id);
	if(pCardConfig != NULL && pCardConfig->out != NULL)
	{
		// if out proc heed select target , it must fill stEvent.target
		ret = (*pCardConfig->out)(pGame, &stEvent, player);
		CHECK_RET(ret, ret);

		ret = remove_out_card(pGame, pOut);
		CHECK_RET(ret, ret);

		if(pCardConfig->calc != NULL)
		{
			ret = (*pCardConfig->out)(pGame, &stEvent, player);
		}
	}

	// post out maybe modify out cards 
	post_out_card(pGame, pParentEvent, player, stEvent.target, pOut);

	add_out_stack(pGame, pOut);

	return R_SUCC;
}




RESULT game_round_outcard(GameContext* pGame, GameEventContext* pEvent, int player, OutCard* pOut)
{
	return round_out_card(pGame, pEvent, player, pOut);
}


RESULT game_outcard(GameContext* pGame, GameEventContext* pEvent,  int* idx, int num)
{
	char buffer[128];
	if(game_status(pGame) != Status_Round_Out)
		return R_E_STATUS;

	int where;
	int pos;

	if(0 != player_card_idx_to_pos(CUR_PLAYER(pGame), *idx, &where, &pos))
	{
		printf("input card idx error!\n");
		return R_E_PARAM;
	}

	if(where != PlayerCard_Hand)
	{
		printf("only can out hand card!\n");
		return R_E_PARAM;
	}

	// check can out?
	Card stCard = CUR_PLAYER(pGame)->stHandCards[pos];

	const CardConfig* pCardConfig = get_card_config(stCard.id);

	if(pCardConfig == NULL || pCardConfig->check == NULL || YES != (*pCardConfig->check)(pGame, NULL, pGame->nCurPlayer))
	{
		printf("can not out this card: %s!\n", card_str(&stCard, buffer, sizeof(buffer)));
		return R_E_PARAM;
	}


	return (*pCardConfig->out)(pGame, NULL, pGame->nCurPlayer);

	return R_SUCC;
}



RESULT game_pass(GameContext* pGame, GameEventContext* pEvent)
{
	return R_SUCC;
}


