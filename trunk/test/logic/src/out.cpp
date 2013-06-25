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

	pCardConfig = get_card_config(pOut->vcard.id);

	if(pCardConfig == NULL || pCardConfig->check == NULL)
		return NO;

	return (*pCardConfig->check)(pGame, &event, player);
}

static RESULT out_card_prepare(GameContext* pGame, GameEventContext* pParentEvent, int trigger, OutCard* pOut)
{
	RESULT  ret;
	const CardConfig* pCardConfig;

	GameEventContext  stEvent;
	

	pCardConfig = get_card_config(pOut->vcard.id);

	if(pCardConfig == NULL)
		return R_E_FAIL;


	if(pCardConfig->out)
	{
		INIT_EVENT(&stEvent, GameEvent_OutCardPrepare, trigger, 0, pParentEvent);
		stEvent.pOut = pOut;

		ret = (*pCardConfig->out)(pGame, pParentEvent, trigger);

		CHECK_RET(ret, ret);
	}

	return R_SUCC;
}


static RESULT per_out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, OutCard* pOut)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PerOutCard, trigger, target, pParentEvent);
	event.pOut = pOut;

	trigger_game_event(pGame, &event);

	return event.result;
}


static RESULT do_out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, OutCard* pOut)
{
	RESULT   ret;
	const CardConfig* pCardConfig;
	GameEventContext  stEvent;

	INIT_EVENT(&stEvent, GameEvent_OutCard, trigger, target, pParentEvent);

	// out procedure
	pCardConfig = get_card_config(pOut->vcard.id);
	if(pCardConfig != NULL && pCardConfig->out != NULL)
	{
		// if out proc heed select target , it must fill stEvent.target
		ret = (*pCardConfig->out)(pGame, &stEvent, trigger);
		CHECK_RET(ret, ret);

	}
	return R_SUCC;
}


static RESULT post_out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, OutCard* pOut)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PostOutCard, trigger, target, pParentEvent);
	event.pOut = pOut;

	trigger_game_event(pGame, &event);

	return event.result;	
}

static RESULT per_lost_card(GameContext* pGame, GameEventContext* pParentEvent, int player, PosCard* pCard)
{
	GameEventContext   event;
	INIT_EVENT(&event, GameEvent_PerLostCard, player, 0, pParentEvent);
	event.pPosCard = pCard;

	trigger_game_event(pGame, &event);

	return event.result;
}

static RESULT post_lost_card(GameContext* pGame, GameEventContext* pParentEvent, int player, PosCard* pCard)
{
	GameEventContext   event;
	INIT_EVENT(&event, GameEvent_PostLostCard, player, 0, pParentEvent);
	event.pPosCard = pCard;
	
	trigger_game_event(pGame, &event);
	
	return event.result;
}


static RESULT remove_out_card(GameContext* pGame, GameEventContext* pEvent, OutCard* pOut)
{
	int n;
	char buf[128];
	Player* pPlayer = GAME_PLAYER(pGame, pOut->supply);
	
	if(pOut->nrcard > 0)
	{
		for(n = 0; n < pOut->nrcard; n++)
		{
			if(CARD_VALID(&pOut->rcards[n].card))
			{
				// todo : perlostcard
				per_lost_card(pGame, pEvent, pOut->supply, &pOut->rcards[n]);
				if(R_SUCC != player_remove_card(pPlayer, pOut->rcards[n].where, pOut->rcards[n].pos, NULL))
				{
					printf("remove out card [%s] from player [%d] failed ", card_str(&pOut->rcards[n].card, buf, sizeof(buf)), pOut->supply, pOut->rcards[n].pos);
					return R_E_FAIL;
				}
				// todo : postlostcard
				per_lost_card(pGame, pEvent, pOut->supply, &pOut->rcards[n]);
			}
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
			if(R_SUCC != card_stack_push(&pGame->cardOut, &pOut->rcards[n].card))
			{
				printf("add out card [%s] failed ", card_str(&pOut->rcards[n].card, buf, sizeof(buf)));
				return R_E_FAIL;
			}
		}
	}

	return R_SUCC;
}

RESULT game_real_out_card(GameContext* pGame, GameEventContext* pEvent, int player, OutCard* pOut)
{
	RESULT ret;
	//int target;
	
	// prepare out card
	ret = out_card_prepare(pGame, pEvent, player, pOut);
	CHECK_RET(ret,ret);


	if(R_CANCEL == per_out_card(pGame, pEvent, player, pOut->target, pOut))
		return R_CANCEL;


	ret = remove_out_card(pGame, pEvent, pOut);

	CHECK_RET(ret,ret);

	ret = do_out_card(pGame, pEvent, player, pOut->target, pOut);


	// post out maybe modify out cards 
	post_out_card(pGame, pEvent, player, pOut->target, pOut);

	add_out_stack(pGame, pOut);

	return R_SUCC;
}



RESULT game_round_do_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	RESULT    ret;
	OutCard  out_card;
	GameEventContext  stEvent;

	ST_ZERO(out_card);

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

		pCard->flag = CardFlag_PrepareOut;

		pEvent->pOut->rcards[0].card = *pCard;
		pEvent->pOut->rcards[0].where = where;
		pEvent->pOut->rcards[0].pos = pos;
		pEvent->pOut->nrcard = 1;
		pEvent->pOut->vcard = *pCard;
		pEvent->pOut->trigger = pGame->nCurPlayer;
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

	if(ret == R_CANCEL)
		return R_CANCEL;

	INIT_EVENT(&event, GameEvent_PassiveOutCard, player, target, pParentEvent);
	event.pPassiveOut = &passive_out;

	pGame->nCurPlayer = player;

	ret = cmd_loop(pGame, &event, alter_text);

	if(ret != R_SUCC)
		return ret;

	ret = remove_out_card(pGame, pParentEvent, &passive_out.out);

	if(ret != R_SUCC)
		return ret;

	post_passive_out_card(pGame, pParentEvent, player, target, &passive_out);

	add_out_stack(pGame, &passive_out.out);

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

