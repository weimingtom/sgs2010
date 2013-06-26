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

static RESULT per_lost_card(GameContext* pGame, GameEventContext* pParentEvent, int player, PosCard* pPosCard)
{
	GameEventContext   event;
	INIT_EVENT(&event, GameEvent_PerLostCard, player, 0, pParentEvent);
	event.pPosCard = pPosCard;

	trigger_game_event(pGame, &event);

	return event.result;
}

static RESULT post_lost_card(GameContext* pGame, GameEventContext* pParentEvent, int player, PosCard* pPosCard)
{
	GameEventContext   event;
	INIT_EVENT(&event, GameEvent_PostLostCard, player, 0, pParentEvent);
	event.pPosCard = pPosCard;
	
	trigger_game_event(pGame, &event);
	
	return event.result;
}


static RESULT match_out_card_list(Player* pPlayer, CardList* pList)
{
	int    n, m, k;
	char   check[MAX_CARD_LIST_NUM];
	Card*  pCard;
	CardFlag  f;

	ST_ZERO(check);
	k = 0;

	// check hand
	for(n = pPlayer->nHandCardNum + EquipIdx_Max + pPlayer->nJudgmentCardNum - 1; n >= 0; n--)
	{
		if(n < pPlayer->nHandCardNum)
		{
			pCard = &pPlayer->stHandCards[n];
			f = CardFlag_FromHand;
		}
		else if(n < pPlayer->nHandCardNum + EquipIdx_Max)
		{
			pCard = &pPlayer->stEquipCard[n-pPlayer->nHandCardNum];
			//if(!CARD_VALID(pCard))
			//	continue;
			f = CardFlag_FromEquip;
		}
		else /*if(n < pPlayer->nHandCardNum + EquipIdx_Max + pPlayer->nJudgmentCardNum) */
		{
			pCard = &pPlayer->stJudgmentCards[n - pPlayer->nHandCardNum - EquipIdx_Max];
			f = CardFlag_FromJudge;
		}

		if(CARD_VALID(pCard) && pCard->flag == CardFlag_PrepareOut)
		{
			for(m = 0; m < pList->num; m++)
			{
				if(check[m] == 0 && pList->cards[m].flag == f)
				{
					if(CARD_EQUAL(pCard, &pList->cards[m]))
					{
						check[m] = 1;
						k++;
						break;
					}
				}
			}
		}
	}


	if(k == pList->num)
		return R_SUCC;
	return R_E_FAIL;
}


static RESULT remove_out_card(GameContext* pGame, GameEventContext* pEvent, OutCard* pOut)
{
	int n;
	char buf[128];
	char buf2[512];
	PosCard   stCard;
	CardFlag  f;
	Player* pPlayer = GAME_PLAYER(pGame, pOut->supply);
	
	if(pOut->list.num > 0)
	{
		// test player cards with CardFlag_PrepareOut is match pOut
		if(match_out_card_list(pPlayer, &pOut->list))
			return R_E_FAIL;

		// log
		if(pOut->list.num == 1 && CARD_EQUAL(&pOut->list.cards[0], &pOut->vcard))
		{
			printf("player [%s] out a card %s\n", GAME_PLAYER(pGame, pOut->trigger)->name, card_str(&pOut->list.cards[0], buf, sizeof(buf)));
		}
		else
		{
			buf2[0] = 0;
			for(n = 0; n < pOut->list.num; n++)
			{
				strcat(buf2, card_str(&pOut->list.cards[n], buf, sizeof(buf)));
			}

			printf("player [%s] out %d cards %s as a card %s\n", GAME_PLAYER(pGame, pOut->trigger)->name, buf2, card_str(&pOut->vcard, buf, sizeof(buf)));
		}

		// real remove from supply
		for(n = pPlayer->nHandCardNum + EquipIdx_Max + pPlayer->nJudgmentCardNum - 1; n >= 0; n--)
		{
			if(n < pPlayer->nHandCardNum)
			{
				stCard.card = pPlayer->stHandCards[n];
				stCard.where = PlayerCard_Hand;
				stCard.pos = n;
				f = CardFlag_FromHand;
			}
			else if(n < pPlayer->nHandCardNum + EquipIdx_Max)
			{
				stCard.card = pPlayer->stEquipCard[n-pPlayer->nHandCardNum];
				stCard.where = PlayerCard_Equip;
				stCard.pos = n-pPlayer->nHandCardNum;
				//if(!CARD_VALID(pCard))
				//	continue;
				f = CardFlag_FromEquip;
			}
			else /*if(n < pPlayer->nHandCardNum + EquipIdx_Max + pPlayer->nJudgmentCardNum) */
			{
				stCard.card = pPlayer->stJudgmentCards[n - pPlayer->nHandCardNum - EquipIdx_Max];
				stCard.where = PlayerCard_Judgment;
				stCard.pos = n - pPlayer->nHandCardNum - EquipIdx_Max;
				f = CardFlag_FromJudge;
			}
			
			if(CARD_VALID(&stCard.card) && stCard.card.flag == CardFlag_PrepareOut)
			{
				// todo : perlostcard
				per_lost_card(pGame, pEvent, pOut->supply, &stCard);
				if(R_SUCC != player_remove_card(pPlayer, stCard.where, stCard.pos, NULL))
				{
					// after check , fail is impossible
					printf("remove out card [%s] from player [%d] failed ", card_str(&stCard.card, buf, sizeof(buf)), pOut->supply);
					return R_E_FAIL;
				}
				// todo : postlostcard
				per_lost_card(pGame, pEvent, pOut->supply, &stCard);
			}
		}
	}
	return R_SUCC;
}

static RESULT add_out_stack(GameContext* pGame, OutCard* pOut)
{
	int n;
	char buf[128];
	if(pOut->list.num > 0)
	{
		for(n = 0; n < pOut->list.num; n++)
		{
			pOut->list.cards[n].flag = CardFlag_None;
			if(R_SUCC != card_stack_push(&pGame->cardOut, &pOut->list.cards[n]))
			{
				printf("add out card [%s] failed ", card_str(&pOut->list.cards[n], buf, sizeof(buf)));
				return R_E_FAIL;
			}
			ST_ZERO(pOut->list.cards[n]);
		}
	}
	pOut->list.num = 0;

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
	ret = post_out_card(pGame, pEvent, player, pOut->target, pOut);

	add_out_stack(pGame, pOut);

	// the out is not effect
	if(ret == R_CANCEL)
		return ret;

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


		if(pCardConfig == NULL || pCardConfig->check == NULL
			|| YES != (*pCardConfig->check)(pGame, pEvent, pGame->nCurPlayer))
		{
			printf("can not out this card: %s!\n", card_str(pCard, buffer, sizeof(buffer)));
			return R_E_PARAM;
		}

		pCard->flag = CardFlag_PrepareOut;

		pEvent->pOut->list.cards[0] = *pCard;
		pEvent->pOut->list.cards[0].flag = CardFlag_FromHand;
		pEvent->pOut->list.num = 1;
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

	ret = post_passive_out_card(pGame, pParentEvent, player, target, &passive_out);

	add_out_stack(pGame, &passive_out.out);

	// the passive out is not effect
	if(ret == R_CANCEL)
		return R_CANCEL;

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

