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
#include "discard.h"


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

		ret = (*pCardConfig->out)(pGame, &stEvent, trigger);

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
	stEvent.pOut = pOut;

	// out procedure
	pCardConfig = get_card_config(pOut->vcard.id);
	if(pCardConfig != NULL && pCardConfig->out != NULL)
	{
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
	//CardFlag  f;
	Player* pPlayer = GAME_PLAYER(pGame, pOut->supply);
	
	if(pOut->list.num > 0)
	{
		// test player cards with CardFlag_PrepareOut is match pOut
		if(match_out_card_list(pPlayer, &pOut->list))
			return R_E_FAIL;

		// log
		if(pOut->list.num == 1 && CARD_EQUAL(&pOut->list.cards[0], &pOut->vcard))
		{
			MSG_OUT("player [%s] out a card %s\n", GAME_PLAYER(pGame, pOut->trigger)->name, card_str(&pOut->list.cards[0], buf, sizeof(buf)));
		}
		else
		{
			buf2[0] = 0;
			for(n = 0; n < pOut->list.num; n++)
			{
				strcat(buf2, card_str(&pOut->list.cards[n], buf, sizeof(buf)));
			}

			MSG_OUT("player [%s] out %d cards %s as a card %s\n", GAME_PLAYER(pGame, pOut->trigger)->name, pOut->list.num, buf2, card_str(&pOut->vcard, buf, sizeof(buf)));
		}

		// real remove from supply
		for(n = pPlayer->nHandCardNum + EquipIdx_Max + pPlayer->nJudgmentCardNum - 1; n >= 0; n--)
		{
			if(n < pPlayer->nHandCardNum)
			{
				stCard.card = pPlayer->stHandCards[n];
				stCard.where = PlayerCard_Hand;
				stCard.pos = n;
				//f = CardFlag_FromHand;
			}
			else if(n < pPlayer->nHandCardNum + EquipIdx_Max)
			{
				stCard.card = pPlayer->stEquipCard[n-pPlayer->nHandCardNum];
				stCard.where = PlayerCard_Equip;
				stCard.pos = n-pPlayer->nHandCardNum;
				//if(!CARD_VALID(pCard))
				//	continue;
				//f = CardFlag_FromEquip;
			}
			else /*if(n < pPlayer->nHandCardNum + EquipIdx_Max + pPlayer->nJudgmentCardNum) */
			{
				stCard.card = pPlayer->stJudgmentCards[n - pPlayer->nHandCardNum - EquipIdx_Max];
				stCard.where = PlayerCard_Judgment;
				stCard.pos = n - pPlayer->nHandCardNum - EquipIdx_Max;
				//f = CardFlag_FromJudge;
			}
			
			if(CARD_VALID(&stCard.card) && stCard.card.flag == CardFlag_PrepareOut)
			{
				// todo : perlostcard
				per_lost_card(pGame, pEvent, pOut->supply, &stCard);
				if(R_SUCC != player_remove_card(pPlayer, stCard.where, stCard.pos, NULL))
				{
					// after check , fail is impossible
					MSG_OUT("remove out card [%s] from player [%d] failed ", card_str(&stCard.card, buf, sizeof(buf)), pOut->supply);
					return R_E_FAIL;
				}
				// todo : postlostcard
				post_lost_card(pGame, pEvent, pOut->supply, &stCard);
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
			if(R_SUCC != game_add_discard_cur(pGame, &pOut->list.cards[n]))
			{
				MSG_OUT("add out card [%s] failed ", card_str(&pOut->list.cards[n], buf, sizeof(buf)));
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
	
	if(ret != R_SUCC && ret != R_DEF)
		return ret;


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

	set_game_cur_player(pGame, player);

	ret = cmd_loop(pGame, &stEvent, "please out a card, or use a skill:");

	CHECK_RET(ret, ret);

	if(stEvent.result == R_CANCEL)
		return stEvent.result;

	game_real_out_card(pGame, pEvent, player, &out_card);

	return R_SUCC;
}


RESULT game_cmd_outcard(GameContext* pGame, GameEventContext* pEvent,  int* idx, int num)
{
	char buffer[128];
	int where[MAX_CARD_LIST_NUM];
	int pos[MAX_CARD_LIST_NUM];
	int n;
	Card stCard[MAX_CARD_LIST_NUM];
	//CardList   card_list;
	Player* pPlayer = CUR_PLAYER(pGame);
	//GameEventContext  event;

	if(pEvent->id == GameEvent_RoundOutCard)
	{
		// must out one card
		if(num != 1)
		{
			MSG_OUT("only can out one card!\n");
			return R_E_PARAM;
		}

		// any hand card , can check out out

		if(R_SUCC != player_card_idx_to_pos(pPlayer, idx[0], &where[0], &pos[0]))
		{
			MSG_OUT("input card idx [%d] is error!\n", idx[0]);
			return R_E_PARAM;
		}

		if(where[0] != PlayerCard_Hand)
		{
			MSG_OUT("only can out hand card!\n");
			return R_E_PARAM;
		}
		
		// get card
		//pCard = PLAYER_HANDCARD(CUR_PLAYER(pGame), pos);
		get_player_card(pPlayer, where[0], pos[0], &stCard[0]);

		// check can out?
		const CardConfig* pCardConfig = get_card_config(stCard[0].id);


		if(pCardConfig == NULL || pCardConfig->check == NULL
			|| YES != (*pCardConfig->check)(pGame, pEvent, pGame->nCurPlayer))
		{
			MSG_OUT("can not out this card: %s!\n", card_str(&stCard[0], buffer, sizeof(buffer)));
			return R_E_PARAM;
		}

		set_player_card_flag(pPlayer, where[0], pos[0], CardFlag_PrepareOut);

		pEvent->pOut->list.cards[0] = stCard[0];
		pEvent->pOut->list.num = 1;
		pEvent->pOut->vcard = pEvent->pOut->list.cards[0];
		pEvent->pOut->trigger = pGame->nCurPlayer;
		pEvent->pOut->supply = pGame->nCurPlayer;


		pEvent->result = R_SUCC;
		pEvent->block = YES;

		return R_BACK;

	}
	else if(pEvent->id == GameEvent_PassiveOutCard)
	{
		// check out pattern
		if(num != pEvent->pPatternOut->pattern.num)
		{
			MSG_OUT("out card number not match pattern [%d] cards!\n", pEvent->pPatternOut->pattern.num);
			return R_E_PARAM;
		}


		for(n = 0; n < num; n++)
		{
			if(R_SUCC != player_card_idx_to_pos(pPlayer, idx[n], &where[n], &pos[n]))
			{
				MSG_OUT("input card idx [%d] is error!\n", idx[n]);
				return R_E_PARAM;
			}

			if((where[n] & pEvent->pPatternOut->pattern.where) == 0)
			{
				MSG_OUT("card idx [%d] is invalid place!\n", idx[n]);
				return R_E_PARAM;
			}
			// must success
			get_player_card(pPlayer,where[n], pos[n], &stCard[n]);
		}

		// match pattern ?
		if(R_SUCC != card_match(stCard, num, pEvent->pPatternOut->pattern.patterns, pEvent->pPatternOut->pattern.num))
		{
			MSG_OUT("out card is not match pattern!\n");
			return R_E_PARAM;
		}
		
		// check ok. full out card struct

		for(n = 0; n < num; n++)
		{
			// must success
			get_player_card(pPlayer,where[n], pos[n], &pEvent->pPatternOut->out.list.cards[n]);
			set_player_card_flag(pPlayer, where[n], pos[n], CardFlag_PrepareOut);
		}
		pEvent->pPatternOut->out.list.num = num;
		if (num == 1)
		{
			pEvent->pPatternOut->out.vcard = pEvent->pPatternOut->out.list.cards[0];
		}
		pEvent->pPatternOut->out.supply = get_game_cur_player(pGame);
		pEvent->pPatternOut->out.trigger = get_game_cur_player(pGame);

		pEvent->result = R_SUCC;
		pEvent->block = YES;

		return R_BACK;

	}
	else if(pEvent->id == GameEvent_SupplyCard)
	{		
		// check out pattern

		// check out pattern
		if(num != pEvent->pPatternOut->pattern.num)
		{
			MSG_OUT("supply card number not match pattern [%d] cards!\n", pEvent->pPatternOut->pattern.num);
			return R_E_PARAM;
		}


		for(n = 0; n < num; n++)
		{
			if(R_SUCC != player_card_idx_to_pos(pPlayer, idx[n], &where[n], &pos[n]))
			{
				MSG_OUT("input card idx [%d] is error!\n", idx[n]);
				return R_E_PARAM;
			}

			if((where[n] & pEvent->pPatternOut->pattern.where) == 0)
			{
				MSG_OUT("card idx [%d] is invalid place!\n", idx[n]);
				return R_E_PARAM;
			}
			// must success
			get_player_card(pPlayer,where[n], pos[n], &stCard[n]);
		}

		// match pattern ?
		if(R_SUCC != card_match(stCard, num, pEvent->pPatternOut->pattern.patterns, pEvent->pPatternOut->pattern.num))
		{
			MSG_OUT("out card is not match pattern!\n");
			return R_E_PARAM;
		}

		// check ok. full out card struct


		for(n = 0; n < num; n++)
		{
			// must success
			get_player_card(pPlayer,where[n], pos[n], &pEvent->pPatternOut->out.list.cards[n]);
			set_player_card_flag(pPlayer, where[n], pos[n], CardFlag_PrepareOut);
		}
		pEvent->pPatternOut->out.list.num = num;
		if (num == 1)
		{
			pEvent->pPatternOut->out.vcard = pEvent->pPatternOut->out.list.cards[0];
		}
		pEvent->pPatternOut->out.supply = get_game_cur_player(pGame);
		pEvent->pPatternOut->out.trigger = get_game_cur_player(pGame);

		pEvent->result = R_SUCC;
		pEvent->block = YES;

		return R_BACK;

	}

	// other event can not out card
	return R_E_STATUS;
}



RESULT game_cmd_pass(GameContext* pGame, GameEventContext* pEvent)
{
	if(pEvent->id == GameEvent_RoundOutCard)
	{

		pEvent->result = R_CANCEL;
		return R_BACK;
	}
	else if(pEvent->id == GameEvent_PassiveOutCard)
	{
		pEvent->result = R_CANCEL;
		return R_BACK;
	}
	else if(pEvent->id == GameEvent_SupplyCard)
	{

		pEvent->result = R_CANCEL;
		return R_BACK;
	}

	return R_SUCC;
}

// pattern:  <flags>:<card pattern 1>;<card pattern 2>;...
// flags: one or more follow chars -
//         h : hand card is enable
//         e : equip card is enable
//         j : judgement card is enable
//         f : fixed mode, need real card, skill for gen card is disabled
// card  pattern: <{sid}><color><val>
//         each <...> can be [<from>-<to>] or [<p1><p2><p3>] ...
//        <{sid}> : the card sid name. can be empty, that means any sid is valid, equal to {none}
//        <color> : one of char - can be empty, that means any color
//             s  : spade
//             h  : heart
//             d  : diamond
//             c  : club
//             b  : black (spade or club)
//             r  : red (heart or diamond)
//             n  : none (any color)
//        <val> : one of '2 - 10, J, Q, K, A' , can use [from-to] format, if it is empty, means any value.
//             
//
static RESULT  load_pattern(GameContext* pGame, OutCardPattern* pPattern, const char* szPattern)
{
	//RESULT       ret;
	const char*  p;
	int          sp;
	char         tmp[256];
	int          tln;
	CardPattern* pcp;

	ST_ZERO(*pPattern);

	p = szPattern;
	
	// parse <flags>

	while(*p && *p != ':')
	{
		switch(*p)
		{
		case 'h' : pPattern->where |= PlayerCard_Hand; break;
		case 'e' : pPattern->where |= PlayerCard_Equip; break;
		case 'j' : pPattern->where |= PlayerCard_Judgment; break;
		case 'f' : pPattern->fixed = YES; break;
		default: return R_E_FAIL;  // invalid flsg char.
		}
		p++;
	}

	// :
	if(*p != ':')
	{
		// expected ':' splitter flags and card patterns
		return R_E_FAIL;
	}
	p++;

	pPattern->num = 0;


	// <card pattern n>; ...
	while(*p)
	{
		pcp = &pPattern->patterns[pPattern->num];
		pPattern->num++;

		sp = 0;

		// <{sid}>
		if(*p == '{')
		{
			// get {sid}
			tln = 0;
			while(*p && *p != '}')
			{
				if(tln > sizeof(tmp) - 1)
				{
					// too long sid string
					return R_E_FAIL;
				}
				tmp[tln] = *p;
				tln++;
				p++;
			}
			tmp[tln] = 0;

			if(*p != '}')
			{
				// expected '}' for end of sid
				return R_E_FAIL;
			}

			if(0 == strcmp(tmp, "none"))
			{
				pcp->id = CardID_None;
			}
			else
			{
				pcp->id = card_sid2id(pGame, tmp);
				if(pcp->id == (CardID)-1)
				{
					// invalid card sid
					return R_E_FAIL;
				}
			}
		}

		switch(*p)
		{
		case 's': pcp->color = CardColor_Spade; break;
		case 'h': pcp->color = CardColor_Heart; break;
		case 'd': pcp->color = CardColor_Diamond; break;
		case 'c': pcp->color = CardColor_Club; break;
		case 'b': pcp->color = CardColor_GeneralBlack; break;
		case 'r': pcp->color = CardColor_GeneralRed; break;
		case 'n': pcp->color = CardColor_None; break;
		default:  goto  __v_parse;
		}
		p++;

__v_parse:
		switch(*p)
		{
		case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
			pcp->value_min = (CardValue)(CardValue_2 + (*p - '2'));
			break;
		case '1':
			if(*(p+1) == '0')
			{
				p++;
				pcp->value_min = CardValue_10;
			}
			else
			{
				// expected '0' for card value 10
				return R_E_FAIL;
			}
			break;
		case 'J': pcp->value_min = CardValue_J; break;
		case 'Q': pcp->value_min = CardValue_J; break;
		case 'K': pcp->value_min = CardValue_J; break;
		case 'A': pcp->value_min = CardValue_J; break;
		case 'N': pcp->value_min = CardValue_None; break;
		default: goto __fini_parse;
		}
		p++;

		if(*p == '-')
		{
			p++;

			switch(*p)
			{
			case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
				pcp->value_max = (CardValue)(CardValue_2 + (*p - '2'));
				break;
			case '1':
				if(*(p+1) == '0')
				{
					p++;
					pcp->value_max = CardValue_10;
				}
				else
				{
					// expected '0' for card value 10
					return R_E_FAIL;
				}
				break;
			case 'J': pcp->value_max = CardValue_J; break;
			case 'Q': pcp->value_max = CardValue_J; break;
			case 'K': pcp->value_max = CardValue_J; break;
			case 'A': pcp->value_max = CardValue_J; break;
			case 'N': pcp->value_max = CardValue_None; break;
			default: return R_E_FAIL;
			}
			p++;

		}
		else
		{
			pcp->value_max = pcp->value_min;
		}

__fini_parse:

		if(*p == ';')
			p++;
		else if(*p != '\0')
		{
			// expected ';' or end of pattern
			return R_E_FAIL;
		}
	}


	if(pPattern->num == 0)
	{
		// expected at least one card pattern
		return R_E_FAIL;
	}

	return R_SUCC;
}


static RESULT per_passive_out_card(GameContext* pGame, GameEventContext* pParentEvent, int player, int target, PatternOut* pPatternOut)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PerPassiveOutCard, player, target, pParentEvent);
	event.pPatternOut = pPatternOut;

	trigger_game_event(pGame, &event);

	return event.result;
}

static RESULT post_passive_out_card(GameContext* pGame, GameEventContext* pParentEvent, int player, int target, PatternOut* pPatternOut)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PostPassiveOutCard, player, target, pParentEvent);
	event.pPatternOut = pPatternOut;

	trigger_game_event(pGame, &event);

	return event.result;

}


RESULT game_passive_out(GameContext* pGame, GameEventContext* pParentEvent, int player, int target, const char* pattern, const char* alter_text)
{
	PatternOut   pattern_out;
	GameEventContext  event;
	RESULT ret;

	ST_ZERO(pattern_out);
	if(R_SUCC != load_pattern(pGame, &pattern_out.pattern, pattern))
	{
		MSG_OUT("error OudCardPattern \"%s\"\n", pattern);
		return R_E_FAIL;
	}


	ret = per_passive_out_card(pGame, pParentEvent, player, target, &pattern_out);

	if(ret == R_CANCEL)
		return R_CANCEL;

	// be success directly
	if(ret == R_SKIP)
		return R_SUCC;


	INIT_EVENT(&event, GameEvent_PassiveOutCard, player, target, pParentEvent);
	event.pPatternOut = &pattern_out;

	set_game_cur_player(pGame, player);

	ret = cmd_loop(pGame, &event, alter_text);

	if(ret != R_SUCC)
		return ret;

	if(event.result == R_CANCEL)
		return event.result;

	ret = remove_out_card(pGame, pParentEvent, &pattern_out.out);

	if(ret != R_SUCC)
		return ret;

	ret = post_passive_out_card(pGame, pParentEvent, player, target, &pattern_out);

	add_out_stack(pGame, &pattern_out.out);

	// the passive out is not effect
	if(ret == R_CANCEL)
		return R_CANCEL;

	return R_SUCC;
}


RESULT game_supply_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int player, const char* pattern,const char* alter_text, OutCard* pOut)
{
	char   text[1024];
	char   temp[128];
	GameEventContext  event;
	PatternOut   pattern_out;
	RESULT ret;


	INIT_EVENT(&event, GameEvent_SupplyCard, trigger, player, pParentEvent);

	ST_ZERO(pattern_out);
	if(R_SUCC != load_pattern(pGame, &pattern_out.pattern, pattern))
	{
		MSG_OUT("error OudCardPattern \"%s\"\n", pattern);
		return R_E_FAIL;
	}

	event.pPatternOut = &pattern_out;

	set_game_cur_player(pGame, player);

	if(alter_text == NULL)
	{
		snprintf(text, sizeof(text), "player [%s] supply card [%s], please 'out specified card' or 'cancel'", 
			CUR_PLAYER(pGame)->name, card_pattern_str_n(pattern_out.pattern.patterns, pattern_out.pattern.num, temp, sizeof(temp)));
		alter_text = text;
	}

	ret = cmd_loop(pGame, &event, alter_text);
	(void)ret;

	CHECK_RET(event.result, event.result);


	*pOut = pattern_out.out;

	return R_SUCC;
}

