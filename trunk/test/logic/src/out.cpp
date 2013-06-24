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
	INIT_EVENT(&event, GameEvent_OutCardCheck, player, 0, pParentEvent);
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

RESULT game_real_out_card(GameContext* pGame, GameEventContext* pEvent, int player, OutCard* pOut)
{
	RESULT ret;
	const CardConfig* pCardConfig;
	GameEventContext  stEvent;

	if(R_CANCEL == per_out_card(pGame, pEvent, player, 0, pOut))
		return R_CANCEL;

	INIT_EVENT(&stEvent, GameEvent_OutCard, player, 0, pEvent);

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
	post_out_card(pGame, pEvent, player, stEvent.target, pOut);

	add_out_stack(pGame, pOut);

	return R_SUCC;
}



RESULT game_round_do_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	RESULT    ret;
	OutCard  out_card;
	GameEventContext  stEvent;


	INIT_EVENT(&stEvent, GameEvent_RoundOutCard, player, 0, pEvent);
	stEvent.pOut = &out_card;

	ret = cmd_loop(pGame, &stEvent, "please out a card, or use a skill:");

	CHECK_RET(ret, ret);

	game_real_out_card(pGame, pEvent, player, &out_card);

	return R_SUCC;
}


RESULT game_cmd_outcard(GameContext* pGame, GameEventContext* pEvent,  int* idx, int num)
{
	char buffer[128];
	int where;
	int pos;
	Card* pCard;
	//GameEventContext  event;

	if(pEvent->id == GameEvent_RoundOutCard)
	{
		// must out one card
		if(num != 1)
		{
			printf("only can out one card!\n");
			return R_E_PARAM;
		}

		// any hand card , can check out out

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
		
		// get card
		pCard = PLAYER_HANDCARD(CUR_PLAYER(pGame), pos);

		// check can out?
		const CardConfig* pCardConfig = get_card_config(pCard->id);

		//INIT_EVENT(&event, GameEvent_OutCardCheck, pGame->nCurPlayer, 0, pEvent);

		if(pCardConfig == NULL || pCardConfig->check == NULL
			|| YES != (*pCardConfig->check)(pGame, pEvent, pGame->nCurPlayer))
		{
			printf("can not out this card: %s!\n", card_str(pCard, buffer, sizeof(buffer)));
			return R_E_PARAM;
		}

		pEvent->pOut->rcards[0].card = *pCard;
		pEvent->pOut->rcards[0].where = where;
		pEvent->pOut->rcards[0].pos = pos;
		pEvent->pOut->nrcard = 1;
		pEvent->pOut->vcard = pEvent->pOut->rcards[0];
		pEvent->pOut->supply = pGame->nCurPlayer;

		pEvent->result = R_SUCC;
		pEvent->block = YES;

		return R_BACK;

	}
	else if(pEvent->id == GameEvent_PassiveOutCard)
	{
		// check out pattern
	}
	else if(pEvent->id == GameEvent_SupplyCard)
	{		
		// check out pattern
	}

	// other event can not out card
	return R_E_STATUS;
}



RESULT game_cmd_pass(GameContext* pGame, GameEventContext* pEvent)
{

	return R_SUCC;
}


static RESULT per_passive_out_card(GameContext* pGame, GameEventContext* pParentEvent, int player, int target, PassiveOut* pPassiveOut)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PerOutCard, player, target, pParentEvent);
	event.pPassiveOut = pPassiveOut;

	trigger_game_event(pGame, &event);

	return event.result;
}

static RESULT post_passive_out_card(GameContext* pGame, GameEventContext* pParentEvent, int player, int target, PassiveOut* pPassiveOut)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PostOutCard, player, target, pParentEvent);
	event.pPassiveOut = pPassiveOut;

	trigger_game_event(pGame, &event);

	return event.result;

}


RESULT game_passive_out(GameContext* pGame, GameEventContext* pParentEvent, int player, int target, const OutCardPattern* pattern, const char* alter_text)
{
	PassiveOut   passive_out;
	GameEventContext  event;
	RESULT ret;

	ST_ZERO(passive_out);
	passive_out.pattern = *pattern;


	ret = per_passive_out_card(pGame, pParentEvent, player, target, &passive_out);

	CHECK_RET(ret, ret);

	INIT_EVENT(&event, GameEvent_PassiveOutCard, player, target, pParentEvent);
	event.pPassiveOut = &passive_out;

	pGame->nCurPlayer = player;

	ret = cmd_loop(pGame, &event, alter_text);
	CHECK_RET(event.result, event.result);

	post_passive_out_card(pGame, pParentEvent, player, target, &passive_out);

	ret = remove_out_card(pGame, &passive_out.out);
	CHECK_RET(ret, ret);

	return R_SUCC;
}


RESULT game_supply_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int player, const OutCardPattern* pattern,const char* alter_text, OutCard* pOut)
{
	char   text[1024];
	char   temp[128];
	GameEventContext  event;
	PassiveOut  passive_out;
	RESULT ret;


	INIT_EVENT(&event, GameEvent_SupplyCard, trigger, player, pParentEvent);

	ST_ZERO(passive_out);
	passive_out.pattern = *pattern;
	event.pPassiveOut = &passive_out;

	pGame->nCurPlayer = player;

	if(alter_text == NULL)
	{
		snprintf(text, sizeof(text), "player [%s] supply card [%s], please 'out req card' or 'cancel'", CUR_PLAYER(pGame)->name, card_pattern_str_n(pattern->patterns, pattern->num, temp, sizeof(temp)));
		alter_text = text;
	}

	ret = cmd_loop(pGame, &event, text);

	CHECK_RET(event.result, event.result);


	*pOut = passive_out.out;

	return R_SUCC;
}

