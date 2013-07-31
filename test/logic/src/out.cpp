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


static RESULT out_card_prepare(GameContext* pGame, GameEventContext* pParentEvent, int trigger, OutCard* out_card)
{
	RESULT  ret;
	//const CardConfig* pCardConfig;

	GameEventContext  stEvent;
	

	//pCardConfig = get_card_config(out_card->vcard.id);

	//if(pCardConfig == NULL)
	//	return R_E_FAIL;


	//if(pCardConfig->out)
	{
		INIT_EVENT(&stEvent, GameEvent_OutCardPrepare, trigger, INVALID_PLAYER, pParentEvent);
		stEvent.out_card = out_card;
		out_card->target = -1;  // for require fill the target

		//ret = (*pCardConfig->out)(pGame, &stEvent, trigger);
		ret = call_card_event(out_card->vcard.id, pGame, &stEvent, trigger);

		CHECK_RET(ret, ret);
	}

	return R_SUCC;
}


static RESULT per_out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, OutCard* out_card)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PerOutCard, trigger, target, pParentEvent);
	event.out_card = out_card;

	trigger_game_event(pGame, &event);

	return event.result;
}


static RESULT do_out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, OutCard* out_card)
{
	RESULT   ret;
	//const CardConfig* pCardConfig;
	GameEventContext  stEvent;


	// out procedure
	//pCardConfig = get_card_config(out_card->vcard.id);
	//if(pCardConfig != NULL && pCardConfig->out != NULL)
	{
		// out card drive
		INIT_EVENT(&stEvent, GameEvent_OutCard, trigger, target, pParentEvent);
		stEvent.out_card = out_card;
		//ret = (*pCardConfig->out)(pGame, &stEvent, trigger);
		ret = call_card_event(out_card->vcard.id, pGame, &stEvent, trigger);
		CHECK_RET(ret, ret);
	}

	// per calc
	INIT_EVENT(&stEvent, GameEvent_PerOutCardCalc, trigger, target, pParentEvent);
	stEvent.out_card = out_card;
	
	trigger_game_event(pGame, &stEvent);

	CHECK_BACK_RET(stEvent.result);

	// calc card
	INIT_EVENT(&stEvent, GameEvent_OutCardCalc, trigger, target, pParentEvent);
	stEvent.out_card = out_card;

	ret = call_card_event(out_card->vcard.id, pGame, &stEvent, trigger);
	CHECK_RET(ret, ret);

	// post calc
	INIT_EVENT(&stEvent, GameEvent_PostOutCardCalc, trigger, target, pParentEvent);
	stEvent.out_card = out_card;

	trigger_game_event(pGame, &stEvent);

	CHECK_BACK_RET(stEvent.result);

	return R_SUCC;
}


static RESULT post_out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, OutCard* out_card)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PostOutCard, trigger, target, pParentEvent);
	event.out_card = out_card;

	trigger_game_event(pGame, &event);

	return event.result;	
}

static RESULT per_lost_card(GameContext* pGame, GameEventContext* pParentEvent, int player, PosCard* pos_card)
{
	GameEventContext   event;
	INIT_EVENT(&event, GameEvent_PerLostCard, player, INVALID_PLAYER, pParentEvent);
	event.pos_card = pos_card;

	trigger_game_event(pGame, &event);

	return event.result;
}

static RESULT post_lost_card(GameContext* pGame, GameEventContext* pParentEvent, int player, PosCard* pos_card)
{
	GameEventContext   event;
	INIT_EVENT(&event, GameEvent_PostLostCard, player, INVALID_PLAYER, pParentEvent);
	event.pos_card = pos_card;
	
	trigger_game_event(pGame, &event);
	
	return event.result;
}


static RESULT match_out_card_list(Player* pPlayer, PosCardList* pList)
{
	int    n, m, k;
	char   check[MAX_CARD_LIST_NUM];
	Card*  pCard;
	CardFlag  f;

	ST_ZERO(check);
	k = 0;

	// check hand
	for(n = pPlayer->hand_card_num + EquipIdx_Max + pPlayer->judgment_card_num - 1; n >= 0; n--)
	{
		if(n < pPlayer->hand_card_num)
		{
			pCard = &pPlayer->hand_cards[n];
			f = CardFlag_FromHand;
		}
		else if(n < pPlayer->hand_card_num + EquipIdx_Max)
		{
			pCard = &pPlayer->equip_cards[n-pPlayer->hand_card_num];
			//if(!CARD_VALID(pCard))
			//	continue;
			f = CardFlag_FromEquip;
		}
		else /*if(n < pPlayer->hand_card_num + EquipIdx_Max + pPlayer->judgment_card_num) */
		{
			pCard = &pPlayer->judgment_cards[n - pPlayer->hand_card_num - EquipIdx_Max];
			f = CardFlag_FromJudge;
		}

		if(CARD_VALID(pCard) && pCard->flag == CardFlag_PrepareOut)
		{
			for(m = 0; m < pList->num; m++)
			{
				if(check[m] == 0 && pList->pcards[m].card.flag == f)
				{
					if(CARD_EQUAL(pCard, &pList->pcards[m].card))
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


static RESULT remove_out_card(GameContext* pGame, GameEventContext* pEvent, OutCard* out_card)
{
	int n;
	char buf[128];
	char buf2[512];
	PosCard   stCard;
	//CardFlag  f;
	Player* pPlayer = get_game_player(pGame, out_card->supply);
	
	if(out_card->list.num > 0)
	{
		// test player cards with CardFlag_PrepareOut is match out_card
		if(match_out_card_list(pPlayer, &out_card->list))
			return R_E_FAIL;

		// log
		if(out_card->list.num == 1 && CARD_EQUAL(&out_card->list.pcards[0].card, &out_card->vcard))
		{
			if(out_card->trigger == out_card->supply)
			{
				MSG_OUT("��%s������ %s��\n", get_game_player(pGame, out_card->trigger)->name, card_str(&out_card->list.pcards[0].card, buf, sizeof(buf)));
			}
			else
			{
				MSG_OUT("��%s������ɡ�%s���ṩ���� %s��\n", get_game_player(pGame, out_card->trigger)->name,
					get_game_player(pGame, out_card->supply)->name, card_str(&out_card->list.pcards[0].card, buf, sizeof(buf)));
			}
		}
		else
		{
			buf2[0] = 0;
			for(n = 0; n < out_card->list.num; n++)
			{
				strcat(buf2, card_str(&out_card->list.pcards[n].card, buf, sizeof(buf)));
			}

			if(out_card->trigger == out_card->supply)
			{
				MSG_OUT("��%s����[%d]���� %s ���� %s �����\n", get_game_player(pGame, out_card->trigger)->name, 
					out_card->list.num, buf2, card_str(&out_card->vcard, buf, sizeof(buf)));
			}
			else
			{
				MSG_OUT("��%s�����ɡ�%s���ṩ��[%d]���� %s ���� %s �����\n", get_game_player(pGame, out_card->trigger)->name, 
					get_game_player(pGame, out_card->supply)->name, out_card->list.num, buf2, card_str(&out_card->vcard, buf, sizeof(buf)));
			}
		}

		// real remove from supply
		for(n = pPlayer->hand_card_num + EquipIdx_Max + pPlayer->judgment_card_num - 1; n >= 0; n--)
		{
			if(n < pPlayer->hand_card_num)
			{
				stCard.card = pPlayer->hand_cards[n];
				stCard.where = CardWhere_PlayerHand;
				stCard.pos = n;
				//f = CardFlag_FromHand;
			}
			else if(n < pPlayer->hand_card_num + EquipIdx_Max)
			{
				stCard.card = pPlayer->equip_cards[n-pPlayer->hand_card_num];
				stCard.where = CardWhere_PlayerEquip;
				stCard.pos = n-pPlayer->hand_card_num;
				//if(!CARD_VALID(pCard))
				//	continue;
				//f = CardFlag_FromEquip;
			}
			else /*if(n < pPlayer->hand_card_num + EquipIdx_Max + pPlayer->judgment_card_num) */
			{
				stCard.card = pPlayer->judgment_cards[n - pPlayer->hand_card_num - EquipIdx_Max];
				stCard.where = CardWhere_PlayerJudgment;
				stCard.pos = n - pPlayer->hand_card_num - EquipIdx_Max;
				//f = CardFlag_FromJudge;
			}
			
			if(CARD_VALID(&stCard.card) && stCard.card.flag == CardFlag_PrepareOut)
			{
				// todo : perlostcard
				per_lost_card(pGame, pEvent, out_card->supply, &stCard);
				if(R_SUCC != player_remove_card(pPlayer, stCard.where, stCard.pos, NULL))
				{
					// after check , fail is impossible
					MSG_OUT("remove out card [%s] from player [%d] failed ", card_str(&stCard.card, buf, sizeof(buf)), out_card->supply);
					return R_E_FAIL;
				}
				// todo : postlostcard
				post_lost_card(pGame, pEvent, out_card->supply, &stCard);
			}
		}
	}
	return R_SUCC;
}

static RESULT add_out_stack(GameContext* pGame, OutCard* out_card)
{
	int n;
	int  pos;
	char buf[128];
	if(out_card->list.num > 0)
	{
		for(n = 0; n < out_card->list.num; n++)
		{
			// out_card->list.pcards[n].card.flag = CardFlag_None;
			if(R_SUCC != game_add_discard_cur(pGame, &out_card->list.pcards[n].card, &pos))
			{
				MSG_OUT("add out card [%s] failed ", card_str(&out_card->list.pcards[n].card, buf, sizeof(buf)));
				return R_E_FAIL;
			}
			out_card->list.pcards[n].where = CardWhere_CurDiscardStack;
			out_card->list.pcards[n].pos = pos;
			//ST_ZERO(out_card->list.pcards[n]);
		}
	}
	//out_card->list.num = 0;

	return R_SUCC;
}

RESULT game_real_out_card(GameContext* pGame, GameEventContext* pEvent, int player, OutCard* out_card)
{
	RESULT ret;
	//int target;
	
	// prepare out card
	ret = out_card_prepare(pGame, pEvent, player, out_card);
	
	if(ret != R_SUCC && ret != R_DEF)
		return ret;


	ret = per_out_card(pGame, pEvent, player, out_card->target, out_card);

	CHECK_BACK_RET(ret);


	ret = remove_out_card(pGame, pEvent, out_card);
	CHECK_RET(ret,ret);

	add_out_stack(pGame, out_card);


	ret = do_out_card(pGame, pEvent, player, out_card->target, out_card);
	CHECK_RET(ret,ret);

	// post out maybe modify out cards 
	ret = post_out_card(pGame, pEvent, player, out_card->target, out_card);


	// the out is not effect
	CHECK_BACK_RET(ret);

	return R_SUCC;
}



RESULT game_round_do_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	RESULT    ret;
	//OutCard  out_card;
	GameEventContext  stEvent;

	//ST_ZERO(out_card);

	INIT_EVENT(&stEvent, GameEvent_RoundOutCard, player, INVALID_PLAYER, pEvent);
	//stEvent.out_card = &out_card;

	set_game_cur_player(pGame, player);

	ret = cmd_loop(pGame, &stEvent, YES, "���һ���ƻ��߷�������:");

	CHECK_RET(ret, ret);

	CHECK_BACK_RET(stEvent.result);

	return R_SUCC;
}


RESULT game_cmd_outcard(GameContext* pGame, GameEventContext* pEvent,  int* idx, int num)
{
	char buffer[128];
	//CardWhere where[MAX_CARD_LIST_NUM];
	//int pos[MAX_CARD_LIST_NUM];
	PosCard stCard[MAX_CARD_LIST_NUM];
	Player* pPlayer = CUR_PLAYER(pGame);
	int n, m;


	if(pEvent->id == GameEvent_PassiveOutCard)
	{
		// check out pattern
		if(num != pEvent->pattern_out->pattern.num)
		{
			MSG_OUT("���Ƶ�����������Ҫ��Ӧ�ó�[%d]����!\n", pEvent->pattern_out->pattern.num);
			return R_E_PARAM;
		}


		for(n = 0; n < num; n++)
		{
			for(m = 0; m < n; m++)
			{
				if(idx[m] == idx[n])
				{
					MSG_OUT("����[%d]�ظ�!\n", idx[n]);
					return R_E_PARAM;
				}
			}
			if(R_SUCC != player_card_idx_to_pos(pPlayer, idx[n], &stCard[n].where, &stCard[n].pos))
			{
				MSG_OUT("����[%d]��Ч!\n", idx[n]);
				return R_E_PARAM;
			}

			if(!CHECK_WHERE_PATTERN(stCard[n].where, pEvent->pattern_out->pattern.where))
			{
				MSG_OUT("����[%d]���Ƶ�λ�ò�����Ҫ��!\n", idx[n]);
				return R_E_PARAM;
			}
			// must success
			get_player_card(pPlayer,stCard[n].where, stCard[n].pos, &stCard[n].card);
		}

		// match pattern ?
		if(R_SUCC != card_match(&stCard[0].card, sizeof(PosCard), num, pEvent->pattern_out->pattern.patterns, pEvent->pattern_out->pattern.num))
		{
			MSG_OUT("����������ͻ��߻�ɫ������Ҫ��!\n");
			return R_E_PARAM;
		}
		
		// check ok. full out card struct

		for(n = 0; n < num; n++)
		{
			// must success
			//get_player_card(pPlayer,where[n], pos[n], &pEvent->pattern_out->out.list.pcards[n].card);
			//pEvent->pattern_out->out.list.pcards[n].where = where[n];
			//pEvent->pattern_out->out.list.pcards[n].pos = pos[n];
			pEvent->pattern_out->out.list.pcards[n] = stCard[n];
			set_player_card_flag(pPlayer, stCard[n].where, stCard[n].pos, CardFlag_PrepareOut);
		}
		pEvent->pattern_out->out.list.num = num;
		if (num == 1)
		{
			pEvent->pattern_out->out.vcard = pEvent->pattern_out->out.list.pcards[0].card;
		}
		pEvent->pattern_out->out.supply = get_game_cur_player(pGame);
		pEvent->pattern_out->out.trigger = get_game_cur_player(pGame);

		pEvent->result = R_SUCC;
		pEvent->block = YES;

		return R_BACK;

	}
	else if(pEvent->id == GameEvent_SupplyCard)
	{		
		// check out pattern

		// check out pattern
		if(num != pEvent->pattern_out->pattern.num)
		{
			MSG_OUT("�ṩ���Ƶ�����������Ҫ��Ӧ���ṩ[%d]����!\n", pEvent->pattern_out->pattern.num);
			return R_E_PARAM;
		}


		for(n = 0; n < num; n++)
		{
			for(m = 0; m < n; m++)
			{
				if(idx[m] == idx[n])
				{
					MSG_OUT("����[%d]�ظ�!\n", idx[n]);
					return R_E_PARAM;
				}
			}
			if(R_SUCC != player_card_idx_to_pos(pPlayer, idx[n], &stCard[n].where, &stCard[n].pos))
			{
				MSG_OUT("����[%d]��Ч!\n", idx[n]);
				return R_E_PARAM;
			}

			if(!CHECK_WHERE_PATTERN(stCard[n].where, pEvent->pattern_out->pattern.where))
			{
				MSG_OUT("����[%d]���Ƶ�λ�ò�����Ҫ��!\n", idx[n]);
				return R_E_PARAM;
			}
			// must success
			get_player_card(pPlayer,stCard[n].where, stCard[n].pos, &stCard[n].card);
		}

		// match pattern ?
		if(R_SUCC != card_match(&stCard[0].card, sizeof(PosCard), num, pEvent->pattern_out->pattern.patterns, pEvent->pattern_out->pattern.num))
		{
			MSG_OUT("���ṩ�������ͻ��߻�ɫ������Ҫ��!\n");
			return R_E_PARAM;
		}

		// check ok. full out card struct


		for(n = 0; n < num; n++)
		{
			// must success
			//get_player_card(pPlayer,where[n], pos[n], &pEvent->pattern_out->out.list.pcards[n].card);
			//pEvent->pattern_out->out.list.pcards[n].where = where[n];
			//pEvent->pattern_out->out.list.pcards[n].pos = pos[n];
			pEvent->pattern_out->out.list.pcards[n] = stCard[n];
			set_player_card_flag(pPlayer, stCard[n].where, stCard[n].pos, CardFlag_PrepareOut);
		}
		pEvent->pattern_out->out.list.num = num;
		if (num == 1)
		{
			pEvent->pattern_out->out.vcard = pEvent->pattern_out->out.list.pcards[0].card;
		}
		pEvent->pattern_out->out.supply = get_game_cur_player(pGame); //suply card  is c current player
		pEvent->pattern_out->out.trigger = pEvent->target;    // the supply card target player is real out card player

		pEvent->result = R_SUCC;
		pEvent->block = YES;

		return R_BACK;

	}
	else /*	if(pEvent->id == GameEvent_RoundOutCard) */  // in any event can out is allowed
	{
		// must out one card
		if(num != 1)
		{
			MSG_OUT("��ֻ�ܳ�һ����!\n");
			return R_E_PARAM;
		}

		// any hand card , can check out out

		if(R_SUCC != player_card_idx_to_pos(pPlayer, idx[0], &stCard[0].where, &stCard[0].pos))
		{
			MSG_OUT("����[%d]��Ч!\n", idx[0]);
			return R_E_PARAM;
		}

		if(stCard[0].where != CardWhere_PlayerHand)
		{
			MSG_OUT("��ֻ�ܽ����ƴ��!\n");
			return R_E_PARAM;
		}

		// get card
		//pCard = PLAYER_HANDCARD(CUR_PLAYER(pGame), pos);
		get_player_card(pPlayer, stCard[0].where, stCard[0].pos, &stCard[0].card);

		// check can out?
		// const CardConfig* pCardConfig = get_card_config(stCard[0].id);


		//if(pCardConfig == NULL || pCardConfig->check == NULL
		//	|| YES != (*pCardConfig->check)(pGame, pEvent, pGame->cur_player))
		if(YES != call_card_can_out(stCard[0].card.id, pGame, pEvent, pGame->cur_player, &stCard[0]))
		{
			MSG_OUT("�㵱ǰ���ܳ������� %s!\n", card_str(&stCard[0].card, buffer, sizeof(buffer)));
			return R_E_PARAM;
		}

		set_player_card_flag(pPlayer, stCard[0].where, stCard[0].pos, CardFlag_PrepareOut);

		OutCard   out_card;

		out_card.list.pcards[0] = stCard[0];
		out_card.list.num = 1;
		out_card.vcard = out_card.list.pcards[0].card;
		out_card.trigger = pGame->cur_player;
		out_card.supply = pGame->cur_player;

		game_real_out_card(pGame, pEvent, pGame->cur_player, &out_card);


		//pEvent->result = R_SUCC;
		//pEvent->block = YES;

		return R_BACK;
	}

	// other event can not out card
	return R_E_STATUS;
}



RESULT game_cmd_pass(GameContext* pGame, GameEventContext* pEvent)
{
	/*
	if(pEvent->id == GameEvent_PassiveOutCard)
	{
		pEvent->result = R_CANCEL;
		return R_BACK;
	}
	else if(pEvent->id == GameEvent_SupplyCard)
	{

		pEvent->result = R_CANCEL;
		return R_BACK;
	}
	else  // out card in any status is allowed
	{

		pEvent->result = R_CANCEL;
		return R_BACK;
	}
	*/

	return R_CANCEL;
}


//////////////////////////////////////////////////////////////////////
// script exports


///////////////////////////////////////////////////////////////////////



// pattern:  <flags>:<card pattern 1>;<card pattern 2>;...?<custom_str>
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
static RESULT  load_out_pattern(OutCardPattern* pPattern, const char* szPattern)
{
	//RESULT       ret;
	const char*  p;
	//int          sp;
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
		case 'h' : pPattern->where |= PatternWhere_Hand; break;
		case 'e' : pPattern->where |= PatternWhere_Equip; break;
		case 'j' : pPattern->where |=PatternWhere_Judgment; break;
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
	while(*p && *p != ';' && *p != '\?')
	{
		if(pPattern->num>= MAX_CARD_LIST_NUM)
		{
			// too many pattern cards
			return R_E_FAIL;
		}
		pcp = &pPattern->patterns[pPattern->num];
		pPattern->num++;

		//sp = 0;

		// <{sid}>
		if(*p == '{')
		{
			p++;
			// get {sid}
			tln = 0;
			while(*p && *p != '}')
			{
				if(tln > (int)sizeof(tmp) - 1)
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

			p++;

			if(0 == strcmp(tmp, "none"))
			{
				pcp->id = CardID_None;
			}
			else
			{
				pcp->id = card_sid2id(tmp);
				if(pcp->id == (CardID_None))
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
		{
			p++;
		}
		else if(*p == '\?')
		{
			break;
		}
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


	if(*p == '\?')
	{
		p++;
		strncpy(pPattern->ud, p, sizeof(pPattern->ud));
	}

	return R_SUCC;
}

/*
static RESULT per_passive_out_card(GameContext* pGame, GameEventContext* pParentEvent, int player, int target, PatternOut* pattern_out)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PerPassiveOutCard, player, target, pParentEvent);
	event.pattern_out = pattern_out;

	trigger_game_event(pGame, &event);

	return event.result;
}

static RESULT post_passive_out_card(GameContext* pGame, GameEventContext* pParentEvent, int player, int target, PatternOut* pattern_out)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PostPassiveOutCard, player, target, pParentEvent);
	event.pattern_out = pattern_out;

	trigger_game_event(pGame, &event);

	return event.result;

}
*/



static RESULT game_passive_out_card(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, int target, OutCardPattern* out_pattern, const char* alter_text)
{
	RESULT  ret;
	GameEventContext event;
	PatternOut   pattern_out;

	ST_ZERO(pattern_out);
	pattern_out.pattern = *out_pattern;

	if(YES != pattern_out.pattern.fixed)
	{

		INIT_EVENT(&event, GameEvent_PerPassiveOutCard, player, target, pParentEvent);
		event.pattern_out = &pattern_out;

		trigger_game_event(pGame, &event);
		//ret = per_passive_out_card(pGame, pParentEvent, player, target, &pattern_out);

		CHECK_BACK_RET(event.result);

		// be success directly (for example armor card skill may defend the attack of {sha} )
		if(event.result == R_SKIP)
			return R_SUCC;
	}



	INIT_EVENT(&event, GameEvent_PassiveOutCard, player, target, pParentEvent);
	event.pattern_out = &pattern_out;

	set_game_cur_player(pGame, player);

	ret = cmd_loop(pGame, &event, YES, alter_text);

	if(ret != R_SUCC)
		return ret;

	CHECK_BACK_RET(event.result);

	ret = remove_out_card(pGame, pParentEvent, &pattern_out.out); 

	if(ret != R_SUCC)
		return ret;

	add_out_stack(pGame, &pattern_out.out);


	//ret = post_passive_out_card(pGame, pParentEvent, player, target, &pattern_out);

	INIT_EVENT(&event, GameEvent_PostPassiveOutCard, player, target, pParentEvent);
	event.pattern_out = &pattern_out;

	trigger_game_event(pGame, &event);


	// the passive out is not effect
	CHECK_BACK_RET(event.result);

	return R_SUCC;
}


RESULT game_passive_out(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, int target, const char* pattern, const char* alter_text)
{
	BeforePassiveOut   before_pout;
	OutCardPattern     out_pattern;
	GameEventContext  event;
	RESULT ret;
	int n;

	if(pGame == NULL || pParentEvent == NULL)
	{
		if(L) {
			luaL_error(L, "game_passive_out: invalid null param");
		} else {
			MSG_OUT("game_passive_out: invalid null param\n");
		}
		return R_E_PARAM;
	}

	if(!IS_PLAYER_VALID(pGame, player))
	{
		if(L) {
			luaL_error(L, "game_passive_out: invalid player index - %d", player );
		} else {
			MSG_OUT("game_passive_out: invalid player index - %d\n", player );
		}
		return R_E_PARAM;
	}


	ST_ZERO(before_pout);
	if(R_SUCC != load_out_pattern(&before_pout.pattern, pattern))
	{
		if(L) {
			luaL_error(L, "game_passive_out: error card pattern \"%s\"", pattern);
		} else {
			MSG_OUT("game_passive_out: error card pattern \"%s\"\n", pattern);
		}
		return R_E_FAIL;
	}


	strncpy(before_pout.alter_text, alter_text, sizeof(before_pout.alter_text));

	// before passive out , some skill can adjust the card pattern, or avoid the passive out.
	INIT_EVENT(&event, GameEvent_BeforePassiveOut, player, target, pParentEvent);
	event.before_pout = &before_pout;

	trigger_game_event(pGame, &event);

	// avoid the passive but result is cancel.
	CHECK_BACK_RET(event.result);

	// be success directly (for example armor card skill may defend the attack of {sha} )
	if(event.result == R_SKIP)
		return R_SUCC;

	// is pattern card is empty, success directly
	if(before_pout.pattern.num == 0)
		return R_SUCC;

	if(YES == before_pout.pattern.fixed)
	{
		// passive out whill complete out all the required cards;

		ret = game_passive_out_card(L, pGame, pParentEvent, player, target, &before_pout.pattern, before_pout.alter_text);
		if(ret != R_SUCC)
			return ret;

	}
	else
	{
		// for each passive pattern card .do real out

		for(n = 0; n < before_pout.pattern.num; n++)
		{
			ST_ZERO(out_pattern);
			out_pattern.fixed = before_pout.pattern.fixed;
			out_pattern.where = before_pout.pattern.where;
			out_pattern.num = 1;
			out_pattern.patterns[0] = before_pout.pattern.patterns[n];
			memcpy(out_pattern.ud, before_pout.pattern.ud, sizeof(out_pattern.ud));

			ret = game_passive_out_card(L, pGame, pParentEvent, player, target, &out_pattern, before_pout.alter_text);
			if(ret != R_SUCC)
				return ret;
		}
	}


	// after passive out.
	INIT_EVENT(&event, GameEvent_AfterPassiveOut, player, target, pParentEvent);
	event.card_pattern = &before_pout.pattern;

	trigger_game_event(pGame, &event);

	// the passive out is not effect
	CHECK_BACK_RET(event.result);

	return R_SUCC;
}



RESULT game_supply_card(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int trigger, int player, const char* pattern,const char* alter_text, OutCard* out_card)
{
	//char   text[1024];
	//char   temp[128];
	GameEventContext  event;
	PatternOut   pattern_out;
	RESULT ret;

	if(pGame == NULL || pParentEvent == NULL)
	{
		if(L) {
			luaL_error(L, "game_supply_card: invalid null param");
		} else {
			MSG_OUT("game_supply_card: invalid null param\n");
		}
		return R_E_PARAM;
	}


	if(!IS_PLAYER_VALID(pGame, player))
	{
		if(L) {
			luaL_error(L, "game_supply_card: invalid player index - %d", player );
		} else {
			MSG_OUT("game_supply_card: invalid player index - %d\n", player );
		}
		return R_E_PARAM;
	}


	ST_ZERO(pattern_out);
	if(R_SUCC != load_out_pattern(&pattern_out.pattern, pattern))
	{
		if(L) {
			luaL_error(L, "game_supply_card: error card pattern \"%s\"", pattern);
		} else {
			MSG_OUT("game_supply_card: error card pattern \"%s\"\n", pattern);
		}
		return R_E_PARAM;
	}

	// ignore fixed flag
	//if(YES != pattern_out.pattern.fixed) 
	{

		INIT_EVENT(&event, GameEvent_PerSupplyCard, player, trigger, pParentEvent);
		event.pattern_out = &pattern_out;

		trigger_game_event(pGame, &event);
		//ret = per_passive_out_card(pGame, pParentEvent, player, target, &pattern_out);

		CHECK_BACK_RET(event.result);

		// be success directly (for example armor card skill may defend the attack of {sha} )
		if(event.result == R_SKIP)
			return R_SUCC;
	}


	INIT_EVENT(&event, GameEvent_SupplyCard, player, trigger, pParentEvent);
	event.pattern_out = &pattern_out;

	set_game_cur_player(pGame, player);

	if(alter_text == NULL)
	{
		//snprintf(text, sizeof(text), "player [%s] supply card [%s], please 'out specified card' or 'cancel'", 
		//	CUR_PLAYER(pGame)->name, card_pattern_str_n(pattern_out.pattern.patterns, pattern_out.pattern.num, temp, sizeof(temp)));
		//alter_text = text;
	}

	ret = cmd_loop(pGame, &event, YES, alter_text);
	
	CHECK_RET(ret,ret);

	CHECK_BACK_RET(event.result);

	// �ṩ�ƵĹ��̣��������������ṩ������ɾ����ֱ������ʹ������ʹ�ø��Ƶ�ʱ��
	*out_card = pattern_out.out;


	INIT_EVENT(&event, GameEvent_PostSupplyCard, player, trigger, pParentEvent);
	event.pattern_out = &pattern_out;

	trigger_game_event(pGame, &event);

	// the passive out is not effect
	CHECK_BACK_RET(event.result);


	return R_SUCC;
}


