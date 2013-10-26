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
#include "script.h"
#include "select.h"


static RESULT per_out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, OutCard* out_card)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PerOutCard, trigger, INVALID_PLAYER, pParentEvent);
	event.out_card = out_card;

	trigger_game_event(pGame, &event);

	return event.result;
}

static RESULT out_card_drive(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, OutCard* out_card, int skip_drive)
{
	RESULT   ret;

	GameEventContext  stEvent;


	// out procedure (start do effect)

	// result is skip means skip the out card drive , calc card directly. (disable the chance of target response the out card)
	if(skip_drive == 0)  
	{
		// out card drive
		INIT_EVENT(&stEvent, GameEvent_OutCard, trigger, target, pParentEvent);
		stEvent.out_card = out_card;
		ret = call_card_event(out_card->vcard.id, pGame, &stEvent, trigger);

		// response the out card , if event return R_CANCEL, out card is broken
		RET_CHECK_CANCEL_RET(ret, R_CANCEL);
	}

	// per calc
	INIT_EVENT(&stEvent, GameEvent_PerOutCardCalc, trigger, target, pParentEvent);
	stEvent.out_card = out_card;

	trigger_game_event(pGame, &stEvent);

	RET_CHECK_CANCEL_RET(stEvent.result, R_CANCEL);	

	// some skill can skip the calc of out card
	if(stEvent.result != R_SKIP)
	{
		// calc card
		INIT_EVENT(&stEvent, GameEvent_OutCardCalc, trigger, target, pParentEvent);
		stEvent.out_card = out_card;

		ret = call_card_event(out_card->vcard.id, pGame, &stEvent, trigger);
		RET_CHECK_CANCEL_RET(ret, R_CANCEL);
	}

	// post calc
	INIT_EVENT(&stEvent, GameEvent_PostOutCardCalc, trigger, target, pParentEvent);
	stEvent.out_card = out_card;

	trigger_game_event(pGame, &stEvent);

	// event return cancel, so out card is invalid.  
	RET_CHECK_CANCEL_RET(stEvent.result, R_CANCEL);	

	return R_SUCC;
}


static RESULT do_out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, OutCard* out_card)
{
	//RESULT   ret;

	GameEventContext  stEvent;

	if(target != INVALID_PLAYER)
	{

		MSG_OUT("%s 的目标设置为【%s】\n", get_card_str(&out_card->vcard), GAME_PLAYER(pGame, target)->name);

		INIT_EVENT(&stEvent, GameEvent_OutCardSetTarget, trigger, target, pParentEvent);
		stEvent.out_card = out_card;
		out_card->cur_target = target;

		trigger_game_event(pGame, &stEvent);

		// can cancel out card
		RET_CHECK_CANCEL_RET(stEvent.result, R_CANCEL);	

		// 可能修改目标
		target = out_card->cur_target;
	}

	// before out card effect (to each target)
	INIT_EVENT(&stEvent, GameEvent_BeforeOutCardEffect, trigger, target, pParentEvent);
	stEvent.out_card = out_card;

	trigger_game_event(pGame, &stEvent);

	// can cancel out card
	RET_CHECK_CANCEL_RET(stEvent.result, R_CANCEL);	

	// out procedure (start do effect)
	return out_card_drive(pGame, pParentEvent, trigger, target, out_card, stEvent.result == R_SKIP);
}


static RESULT post_out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, OutCard* out_card)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PostOutCard, trigger, INVALID_PLAYER, pParentEvent);
	event.out_card = out_card;

	trigger_game_event(pGame, &event);

	return event.result;	
}

static RESULT per_lost_card(GameContext* pGame, GameEventContext* pParentEvent, int player, const Card* vcard, const PosCardList* pcard_list)
{
	GameEventContext   event;
	PosVCard    pos_vcard;

	//set_posvcard_from_poscard(&vcard, pos_card);
	pos_vcard.vcard = *vcard;
	pos_vcard.list = *pcard_list;


	INIT_EVENT(&event, GameEvent_PerLostCard, player, INVALID_PLAYER, pParentEvent);
	event.pos_vcard = &pos_vcard;

	trigger_game_event(pGame, &event);

	return event.result;
}

static RESULT post_lost_card(GameContext* pGame, GameEventContext* pParentEvent, int player, const Card* vcard, const PosCardList* pcard_list)
{
	GameEventContext   event;
	PosVCard    pos_vcard;

	//set_posvcard_from_poscard(&vcard, pos_card);
	pos_vcard.vcard = *vcard;
	pos_vcard.list = *pcard_list;

	INIT_EVENT(&event, GameEvent_PostLostCard, player, INVALID_PLAYER, pParentEvent);
	event.pos_vcard = &pos_vcard;
	
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
	// 不能打出判定区的牌（可能是虚拟牌）
	for(n = pPlayer->hand_card_num + EquipIdx_Max/* + pPlayer->judgment_card_num*/ - 1; n >= 0; n--)
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
			pCard = &pPlayer->judgment_cards[n - pPlayer->hand_card_num - EquipIdx_Max].vcard;
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
				MSG_OUT("【%s】出牌 %s。\n", get_game_player(pGame, out_card->trigger)->name, card_str(&out_card->list.pcards[0].card, buf, sizeof(buf)));
			}
			else
			{
				MSG_OUT("【%s】打出由【%s】提供的牌 %s。\n", get_game_player(pGame, out_card->trigger)->name,
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
				MSG_OUT("【%s】将[%d]张牌 %s 当作 %s 打出。\n", get_game_player(pGame, out_card->trigger)->name, 
					out_card->list.num, buf2, card_str(&out_card->vcard, buf, sizeof(buf)));
			}
			else
			{
				MSG_OUT("【%s】将由【%s】提供的[%d]张牌 %s 当作 %s 打出。\n", get_game_player(pGame, out_card->trigger)->name, 
					get_game_player(pGame, out_card->supply)->name, out_card->list.num, buf2, card_str(&out_card->vcard, buf, sizeof(buf)));
			}
		}

		// per_lost_card event
		per_lost_card(pGame, pEvent, out_card->supply, &out_card->vcard, &out_card->list);

		// real remove from supply
		// 不能打出判定区的牌（可能是虚拟牌）
		for(n = pPlayer->hand_card_num + EquipIdx_Max/* + pPlayer->judgment_card_num*/ - 1; n >= 0; n--)
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
				//stCard.card = pPlayer->judgment_cards[n - pPlayer->hand_card_num - EquipIdx_Max];
				//stCard.where = CardWhere_PlayerJudgment;
				//stCard.pos = n - pPlayer->hand_card_num - EquipIdx_Max;
				//f = CardFlag_FromJudge;
			}
			
			if(CARD_VALID(&stCard.card) && stCard.card.flag == CardFlag_PrepareOut)
			{
				// todo : perlostcard
				//per_lost_card(pGame, pEvent, out_card->supply, &stCard);
				if(R_SUCC != player_remove_card(pPlayer, stCard.where, stCard.pos, NULL))
				{
					// after check , fail is impossible
					MSG_OUT("remove out card [%s] from player [%d] failed ", card_str(&stCard.card, buf, sizeof(buf)), out_card->supply);
					return R_E_FAIL;
				}
				// todo : postlostcard
				//post_lost_card(pGame, pEvent, out_card->supply, &stCard);
			}
		}
		post_lost_card(pGame, pEvent, out_card->supply, &out_card->vcard, &out_card->list);
	}
	return R_SUCC;
}

static RESULT clear_prepare_out_flag(GameContext* pGame, GameEventContext* pEvent, OutCard* out_card)
{
	int n;
	Player* pPlayer = get_game_player(pGame, out_card->supply);

	for(n = 0; n < out_card->list.num; n++)
	{
		if(get_player_card_flag(pPlayer, out_card->list.pcards[n].where, out_card->list.pcards[n].pos) == CardFlag_PrepareOut)
		{
			set_player_card_flag(pPlayer, out_card->list.pcards[n].where, out_card->list.pcards[n].pos, CardFlag_None);
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

static RESULT out_card_prepare(GameContext* pGame, GameEventContext* pParentEvent, int trigger, OutCard* out_card)
{
	RESULT  ret;
	int     n;
	//const CardConfig* pCardConfig;

	GameEventContext  stEvent;

	// out_card->target_num = 0;  // for require fill the target

	INIT_EVENT(&stEvent, GameEvent_PerOutCardPrepare, trigger, INVALID_PLAYER, pParentEvent);
	stEvent.out_card = out_card;

	trigger_game_event(pGame, &stEvent);

	RET_CHECK_CANCEL_RET(stEvent.result, R_CANCEL);

	// some skill can skip the out card prepare, and set targets directly
	if(stEvent.result == R_SKIP)
		return R_SUCC;

	// 如果是指定目标的出牌，则不再调用OutCardPrepare事件,但是需要检查目标是否合法，不合法则出牌失败
	if(IS_FLAG_SET(out_card->flag, OutCardFlag_WithTarget))
	{
		for(n = 0;  n< out_card->target_num; n++)
		{
			// 如果目标检查不能通过，则直接取消出牌
			if(R_SUCC != game_check_attack(pGame, pParentEvent, trigger, out_card->targets[n], out_card->vcard.id, 1))
			{
				return R_CANCEL;
			}
		}
	}
	else
	{
		INIT_EVENT(&stEvent, GameEvent_OutCardPrepare, trigger, INVALID_PLAYER, pParentEvent);
		stEvent.out_card = out_card;

		ret = call_card_event(out_card->vcard.id, pGame, &stEvent, trigger);

		// if return R_CANCEL, break out card
		RET_CHECK_CANCEL_RET(ret, R_CANCEL);
	}

	return R_SUCC;
}


RESULT game_real_out(lua_State* L, GameContext* pGame, GameEventContext* pEvent, int player, OutCard* out_card)
{
	int    n;
	RESULT ret;
	//int target;

	if(!IS_PLAYER_VALID(pGame, player))
	{
		luaL_error(GL(L), "game_real_out: invalid player index - %d", player );
		return R_E_PARAM;
	}

	
	// prepare out card
	ret = out_card_prepare(pGame, pEvent, player, out_card);

	;
	RET_CHECK_RET(ret, (clear_prepare_out_flag(pGame, pEvent, out_card),ret));


	ret = remove_out_card(pGame, pEvent, out_card);
	RET_CHECK_RET(ret, (clear_prepare_out_flag(pGame, pEvent, out_card),ret));

	add_out_stack(pGame, out_card);

	// post prepare , out card message . out put target select alter text.
	if(out_card->message[0] != '\0')
	{
		out_card->message[sizeof(out_card->message)-1] = 0;
		MSG_OUT("%s\n", out_card->message);
		ST_ZERO(out_card->message);
	}


	ret = per_out_card(pGame, pEvent, player, out_card);

	// 到了这里不应该被打断了
	// RET_CHECK_CANCEL_RET(ret, R_CANCEL);
	
	if(out_card->target_num == 0)
	{

		// NO target out card drive
		ret = do_out_card(pGame, pEvent, player, INVALID_PLAYER, out_card);
		//CHECK_RET(ret,ret);
	}
	else
	{
		// for each target out card drive
		for(n = 0; n < out_card->target_num; n++)
		{
			if(IS_PLAYER_VALID(pGame, out_card->targets[n]))
			{
				ret = do_out_card(pGame, pEvent, player, out_card->targets[n], out_card);
				//CHECK_RET(ret,ret);
			}
			else
			{
				luaL_error(GL(NULL), "game_real_out: Invalid target %d", out_card->targets[n]);
			}
		}
	}

	// post out maybe modify out cards 
	post_out_card(pGame, pEvent, player, out_card);

	// ignore event result

	// the out is not effect
	RET_CHECK_RET(ret, R_CANCEL);

	return R_SUCC;
}



RESULT game_round_do_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	RESULT    ret;
	//OutCard  out_card;
	//GameEventContext  stEvent;

	//ST_ZERO(out_card);

	//INIT_EVENT(&stEvent, GameEvent_RoundOutCard, player, INVALID_PLAYER, pEvent);
	//stEvent.out_card = &out_card;

	if(R_SUCC != check_player_event(pGame, pEvent, player, 0))
	{
		// 没有可出的牌和可使用的技能，进入下一阶段
		return R_CANCEL;
	}

	set_game_cur_player(pGame, player);

	ret = cmd_loop(pGame, pEvent, NO, "请出一张牌或者发动技能:");

	CHECK_PLAYER_DEAD_RET(pGame, player, R_CANCEL);
	RET_CHECK_RET(ret, ret);
	// 有没有可能在使用技能或者出牌之后，当前出牌阶段立刻中止？
	EVENT_CHECK_BREAK_RET(pEvent,R_BACK);

	return R_SUCC;
}


RESULT game_cmd_outcard(GameContext* pGame, GameEventContext* pEvent,  int* idx, int num)
{
	char buffer[128];
	VCard vcard;
	//CardWhere where[MAX_CARD_LIST_NUM];
	//int pos[MAX_CARD_LIST_NUM];
	PosCard stCard[MAX_CARD_LIST_NUM];
	Player* pPlayer = CUR_PLAYER(pGame);
	int n, m;


	if(pEvent->id == GameEvent_PassiveOutCard)
	{
		// check out pattern
		if(R_SUCC != out_card_pattern_match_num(&pEvent->pattern_out->pattern, num))
		{
			MSG_OUT("出牌的数量不符合要求!\n");
			return R_E_PARAM;
		}


		for(n = 0; n < num; n++)
		{
			for(m = 0; m < n; m++)
			{
				if(idx[m] == idx[n])
				{
					MSG_OUT("索引[%d]重复!\n", idx[n]);
					return R_E_PARAM;
				}
			}
			if(R_SUCC != player_card_idx_to_pos(pPlayer, idx[n], &stCard[n].where, &stCard[n].pos))
			{
				MSG_OUT("索引[%d]无效!\n", idx[n]);
				return R_E_PARAM;
			}

			if(!CHECK_WHERE_PATTERN(stCard[n].where, pEvent->pattern_out->pattern.where))
			{
				MSG_OUT("索引[%d]的牌的位置不符合要求!\n", idx[n]);
				return R_E_PARAM;
			}

			if(get_player_card_flag(pPlayer, stCard[n].where, stCard[n].pos) != CardFlag_None)
			{
				MSG_OUT("索引[%d]的牌正在使用中!\n", idx[n]);
				return R_E_PARAM;
			}

			// must success
			get_player_card(pPlayer,stCard[n].where, stCard[n].pos, &vcard);
			
			// 不能是虚拟牌
			if(!VCARD_IS_REAL(&vcard))
			{
				MSG_OUT("索引[%d]的牌不能打出!\n", idx[n]);
				return R_E_PARAM;
			}
			stCard[n].card = vcard.vcard;
		}

		// match pattern ?
		if(R_SUCC != out_card_pattern_match_cards(&pEvent->pattern_out->pattern, &stCard[0].card, sizeof(stCard[0]), num))
		{
			MSG_OUT("你出的牌类型或者花色不符合要求!\n");
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
		pEvent->block = YES;
		pEvent->result = R_SUCC;

		return R_SUCC;

	}
	else if(pEvent->id == GameEvent_SupplyCard)
	{		
		// check out pattern

		// check out pattern
		if(R_SUCC != out_card_pattern_match_num(&pEvent->pattern_out->pattern, num))
		{
			MSG_OUT("提供的牌的数量不符合要求!\n");
			return R_E_PARAM;
		}


		for(n = 0; n < num; n++)
		{
			for(m = 0; m < n; m++)
			{
				if(idx[m] == idx[n])
				{
					MSG_OUT("索引[%d]重复!\n", idx[n]);
					return R_E_PARAM;
				}
			}
			if(R_SUCC != player_card_idx_to_pos(pPlayer, idx[n], &stCard[n].where, &stCard[n].pos))
			{
				MSG_OUT("索引[%d]无效!\n", idx[n]);
				return R_E_PARAM;
			}

			if(!CHECK_WHERE_PATTERN(stCard[n].where, pEvent->pattern_out->pattern.where))
			{
				MSG_OUT("索引[%d]的牌的位置不符合要求!\n", idx[n]);
				return R_E_PARAM;
			}

			if(get_player_card_flag(pPlayer, stCard[n].where, stCard[n].pos) != CardFlag_None)
			{
				MSG_OUT("索引[%d]的牌正在使用中!\n", idx[n]);
				return R_E_PARAM;
			}

			// must success
			get_player_card(pPlayer,stCard[n].where, stCard[n].pos, &vcard);

			// 不能是虚拟牌
			if(!VCARD_IS_REAL(&vcard))
			{
				MSG_OUT("索引[%d]的牌不能打出!\n", idx[n]);
				return R_E_PARAM;
			}
			stCard[n].card = vcard.vcard;

		}

		// match pattern ?
		if(R_SUCC != out_card_pattern_match_cards(&pEvent->pattern_out->pattern, &stCard[0].card, sizeof(stCard[0]), num))
		{
			MSG_OUT("你提供的牌类型或者花色不符合要求!\n");
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

		pEvent->block = YES;
		pEvent->result = R_SUCC;

		return R_SUCC;

	}
	else /*	if(pEvent->id == GameEvent_RoundOutCard) */  // in any event can out is allowed
	{
		// must out one card
		if(num != 1)
		{
			MSG_OUT("你只能出一张牌!\n");
			return R_E_PARAM;
		}

		// any hand card , can check out out

		if(R_SUCC != player_card_idx_to_pos(pPlayer, idx[0], &stCard[0].where, &stCard[0].pos))
		{
			MSG_OUT("索引[%d]无效!\n", idx[0]);
			return R_E_PARAM;
		}

		if(stCard[0].where != CardWhere_PlayerHand)
		{
			MSG_OUT("你只能将手牌打出!\n");
			return R_E_PARAM;
		}

		if(get_player_card_flag(pPlayer, stCard[0].where, stCard[0].pos) != CardFlag_None)
		{
			MSG_OUT("索引[%d]的牌正在使用中!\n", idx[0]);
			return R_E_PARAM;
		}

		// get card
		//pCard = PLAYER_HANDCARD(CUR_PLAYER(pGame), pos);
		get_player_card(pPlayer, stCard[0].where, stCard[0].pos, &vcard);

		// 不能是虚拟牌
		if(!VCARD_IS_REAL(&vcard))
		{
			MSG_OUT("索引[%d]的牌不能打出!\n", idx[0]);
			return R_E_PARAM;
		}
		stCard[0].card = vcard.vcard;

		// check can out?
		// const CardConfig* pCardConfig = get_card_config(stCard[0].id);


		//if(pCardConfig == NULL || pCardConfig->check == NULL
		//	|| YES != (*pCardConfig->check)(pGame, pEvent, pGame->cur_player))
		if(YES != game_card_can_out(pGame, pEvent, pGame->cur_player, &stCard[0]))
		{
			MSG_OUT("你当前不能出这张牌 %s!\n", card_str(&stCard[0].card, buffer, sizeof(buffer)));
			return R_E_PARAM;
		}

		set_player_card_flag(pPlayer, stCard[0].where, stCard[0].pos, CardFlag_PrepareOut);

		OutCard   out_card;

		ST_ZERO(out_card);

		out_card.list.pcards[0] = stCard[0];
		out_card.list.num = 1;
		out_card.vcard = out_card.list.pcards[0].card;
		out_card.trigger = pGame->cur_player;
		out_card.supply = pGame->cur_player;
		out_card.flag = OutCardFlag_None;
		out_card.target_num = 0;

		RESULT ret = game_real_out(NULL, pGame, pEvent, pGame->cur_player, &out_card);

		// when out card is canceled , no effects
		//RET_CHECK_CANCEL_RET(ret, R_CANCEL);
		(void)ret;

		// out card must return R_SUCC, because cmd_loop need back to its caller for next out ot use skill
		return R_SUCC;
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


YESNO game_card_can_out(GameContext* pGame, GameEventContext* pEvent, int player, PosCard* pPosCard)
{
	Player* pPlayer;
	GameEventContext   event;

	pPlayer = get_game_player(pGame, player);

	if( pPlayer == NULL) 
		return NO;

	// 使用中的牌不能出
	if(CardFlag_None != get_player_card_flag(pPlayer, pPosCard->where, pPosCard->pos))
		return NO;


	INIT_EVENT(&event, GameEvent_CheckCardCanOut, player, INVALID_PLAYER, pEvent);
	event.pos_card = pPosCard;

	call_game_event(pGame, &event);
	
	RET_CHECK_CANCEL_RET(event.result, NO);
	RET_CHECK_SUCC_RET(event.result, YES);
	
	return call_card_can_out(pPosCard->card.id, pGame, pEvent, player, pPosCard);
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
//         a : any cards and each card must be match one of given card patterns. 
//         <num> : num cards ...
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
	pPattern-> num_type = 0;

	while(*p && *p != ':')
	{
		switch(*p)
		{
		case 'h' : pPattern->where |= PatternWhere_Hand; break;
		case 'e' : pPattern->where |= PatternWhere_Equip; break;
		case 'j' : pPattern->where |=PatternWhere_Judgment; break;
		case 'f' : pPattern->fixed = YES; break;
		case 'a' : // a/n only be one 
			if (pPattern-> num_type != 0)
				return R_E_FAIL;
			pPattern-> num_type = NUM_ANY; break;
		default: 
			if(*p >= '1' && *p <= '9')
			{
				// a/n only be one
				if (pPattern-> num_type != 0)
					return R_E_FAIL;

				pPattern-> num_type = (*p - '0');
				while( p[1] >= '0' && p[1] <='9')
				{
					p++;
					pPattern-> num_type = pPattern-> num_type * 10 + (*p - '0');
					//if(pPattern-> num_type > MAX_CARD_LIST_NUM)
					//	return R_E_FAIL;
				}

				break;
			}
			return R_E_FAIL;  // invalid flag char.
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
		case 'A': pcp->value_min = CardValue_A; break;
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
		case 'Q': pcp->value_min = CardValue_Q; break;
		case 'K': pcp->value_min = CardValue_K; break;
		case 'N': pcp->value_min = CardValue_None; break;
		default: goto __fini_parse;
		}
		p++;

		if(*p == '-')
		{
			p++;

			switch(*p)
			{
			case 'A': pcp->value_max = CardValue_A; break;
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
			case 'Q': pcp->value_max = CardValue_Q; break;
			case 'K': pcp->value_max = CardValue_K; break;
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

void game_load_out_pattern(lua_State* L, OutCardPattern* out_pattern, const char* s_pattern)
{
	if(R_SUCC != load_out_pattern(out_pattern, s_pattern))
	{
		luaL_error(GL(L), "game_load_out_pattern: error card pattern \"%s\"", s_pattern);
	}
}


RESULT  out_card_pattern_match_num(const OutCardPattern* out_card_pattern, int num)
{
	switch(out_card_pattern->num_type)
	{
	case  0: return (num == out_card_pattern->num) ? R_SUCC : R_E_FAIL;
	case NUM_ANY: return (num > 0) ? R_SUCC : R_E_FAIL;	
	default: 
		if(out_card_pattern->num_type > 0 )
		{
			return (num == out_card_pattern->num_type) ? R_SUCC : R_E_FAIL;
		}
		else
		{
			return R_E_FAIL;
		}
	}
}

RESULT  out_card_pattern_match_cards(const OutCardPattern* out_card_pattern, const Card*  cards, size_t offset, int card_num)
{
	int n;
	RESULT ret;
	if(0 == out_card_pattern->num_type )
	{
		return card_match(cards, offset, card_num, out_card_pattern->patterns, out_card_pattern->num);
	}
	else
	{
		for(n = 0; n < card_num; n++)
		{
			ret = card_match((Card*)((char*)cards + ( n * offset )), offset, 1, out_card_pattern->patterns, out_card_pattern->num);
			if(ret != R_SUCC)
			{
				return ret;
			}
		}

		return R_SUCC;
	}
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
	strncpy(pattern_out.alter_text, alter_text, sizeof(pattern_out.alter_text));


	if(YES != pattern_out.pattern.fixed)
	{

		INIT_EVENT(&event, GameEvent_PerPassiveOutCard, player, target, pParentEvent);
		event.pattern_out = &pattern_out;

		trigger_game_event(pGame, &event);
		//ret = per_passive_out_card(pGame, pParentEvent, player, target, &pattern_out);

		CHECK_PLAYER_DEAD_RET(pGame, player, R_CANCEL);

		RET_CHECK_CANCEL_RET(event.result, R_CANCEL);

		// be success directly (for example armor card skill may defend the attack of {sha} )
		RET_CHECK_SUCC_RET(event.result, R_SUCC);
	}



	ret = R_SUCC;

	while(ret == R_SUCC)
	{
		INIT_EVENT(&event, GameEvent_PassiveOutCard, player, target, pParentEvent);
		event.pattern_out = &pattern_out;

		set_game_cur_player(pGame, player);
		ret = cmd_loop(pGame, &event, NO, pattern_out.alter_text);
		CHECK_PLAYER_DEAD_RET(pGame, player, ret);
		RET_CHECK_RET(ret, ret);
		EVENT_CHECK_BREAK( &event);
	}


	//RET_CHECK_BACK(event.result);

	ret = remove_out_card(pGame, pParentEvent, &pattern_out.out); 

	RET_CHECK_RET(ret, (clear_prepare_out_flag(pGame, pParentEvent,  &pattern_out.out),ret));

	add_out_stack(pGame, &pattern_out.out);


	//ret = post_passive_out_card(pGame, pParentEvent, player, target, &pattern_out);

	INIT_EVENT(&event, GameEvent_PostPassiveOutCard, player, target, pParentEvent);
	event.pattern_out = &pattern_out;

	trigger_game_event(pGame, &event);

	// the passive out is not effect
	RET_CHECK_CANCEL_RET(event.result, R_CANCEL);

	return R_SUCC;
}


RESULT game_passive_out(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, int target, OutCardPattern* out_pattern, const char* alter_text)
{
	BeforePassiveOut   before_pout;
	OutCardPattern     t_out_pattern;
	GameEventContext  event;
	RESULT ret;
	int n,  k;

	if(pGame == NULL || pParentEvent == NULL)
	{
		luaL_error(GL(L), "game_passive_out: invalid null param");
		return R_E_PARAM;
	}

	if(!IS_PLAYER_VALID(pGame, player))
	{
		luaL_error(GL(L), "game_passive_out: invalid player index - %d", player );
		return R_E_PARAM;
	}


	ST_ZERO(before_pout);
	/*
	if(R_SUCC != load_out_pattern(&before_pout.pattern, pattern))
	{
		if(L) {
			luaL_error(L, "game_passive_out: error card pattern \"%s\"", pattern);
		} else {
			MSG_OUT("game_passive_out: error card pattern \"%s\"\n", pattern);
		}
		return R_E_FAIL;
	}
	*/

	before_pout.pattern = *out_pattern;
	before_pout.rep_num = 1;
	strncpy(before_pout.alter_text, alter_text, sizeof(before_pout.alter_text));

	// before passive out , some skill can adjust the card pattern, or avoid the passive out.
	INIT_EVENT(&event, GameEvent_BeforePassiveOut, player, target, pParentEvent);
	event.before_pout = &before_pout;

	trigger_game_event(pGame, &event);


	memcpy(out_pattern->ud, before_pout.pattern.ud, sizeof(out_pattern->ud));

	CHECK_PLAYER_DEAD_RET(pGame, player, R_CANCEL);

	// avoid the passive but result is cancel.
	RET_CHECK_CANCEL_RET(event.result, R_CANCEL);

	// be success directly (for example armor card skill may defend the attack of {sha} )
	RET_CHECK_SUCC_RET(event.result, R_SUCC);

	// is pattern card is empty, success directly
	if(before_pout.rep_num == 0 || before_pout.pattern.num == 0)
		return R_SUCC;

	// fix 模式，或者 数量模式为不为0 或者 样式数量为1
	if( (YES == before_pout.pattern.fixed || before_pout.pattern.num_type != 0 || before_pout.pattern.num == 1))
	{
		// passive out whill complete out all the required cards;
		for(k = 0; k < before_pout.rep_num; k++)
		{
			ret = game_passive_out_card(L, pGame, pParentEvent, player, target, &before_pout.pattern, before_pout.alter_text);

			memcpy(out_pattern->ud, before_pout.pattern.ud, sizeof(out_pattern->ud));

			CHECK_PLAYER_DEAD_RET(pGame, player, ret);

			RET_CHECK_RET(ret, ret);
		}
	}
	else
	{
		// for each passive pattern card .do real out
		for(n = 0; n < before_pout.pattern.num; n++)
		{
			ST_ZERO(t_out_pattern);
			t_out_pattern.fixed = NO;
			t_out_pattern.where = before_pout.pattern.where;
			t_out_pattern.num_type = 0;
			t_out_pattern.num = 1;
			t_out_pattern.patterns[0] = before_pout.pattern.patterns[n];
			memcpy(t_out_pattern.ud, before_pout.pattern.ud, sizeof(t_out_pattern.ud));

			ret = game_passive_out_card(L, pGame, pParentEvent, player, target, &t_out_pattern, before_pout.alter_text);

			memcpy(out_pattern->ud, t_out_pattern.ud, sizeof(out_pattern->ud));

			RET_CHECK_RET(ret, ret);
		}

		memcpy(before_pout.pattern.ud, t_out_pattern.ud, sizeof(before_pout.pattern.ud));

	}


	// after passive out.
	INIT_EVENT(&event, GameEvent_AfterPassiveOut, player, target, pParentEvent);
	event.card_pattern = &before_pout.pattern;

	trigger_game_event(pGame, &event);

	memcpy(out_pattern->ud, before_pout.pattern.ud, sizeof(out_pattern->ud));

	// the passive out is not effect
	RET_CHECK_CANCEL_RET(event.result, R_CANCEL);

	return R_SUCC;
}



RESULT game_supply_card(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int trigger, int player, OutCardPattern* out_pattern, const char* alter_text, OutCard* out_card)
{
	GameEventContext  event;
	PatternOut   t_pattern_out;
	RESULT ret;

// 	if(pGame == NULL || pParentEvent == NULL)
// 	{
// 		luaL_error(GL(L), "game_supply_card: invalid null param");
// 		return R_E_PARAM;
// 	}


	if(!IS_PLAYER_VALID(pGame, player))
	{
		luaL_error(GL(L), "game_supply_card: invalid player index - %d", player );
		return R_E_PARAM;
	}

	ST_ZERO(t_pattern_out);
	/*
	if(R_SUCC != load_out_pattern(&t_pattern_out.pattern, pattern))
	{
		if(L) {
			luaL_error(L, "game_supply_card: error card pattern \"%s\"", pattern);
		} else {
			MSG_OUT("game_supply_card: error card pattern \"%s\"\n", pattern);
		}
		return R_E_PARAM;
	}
	*/

	//if(alter_text == NULL)
	//{
		//snprintf(text, sizeof(text), "player [%s] supply card [%s], please 'out specified card' or 'cancel'", 
		//	CUR_PLAYER(pGame)->name, card_pattern_str_n(pattern_out.pattern.patterns, pattern_out.pattern.num, temp, sizeof(temp)));
		//alter_text = text;
	//}
	t_pattern_out.pattern = *out_pattern;
	if(alter_text)
	{
		strncpy(t_pattern_out.alter_text, alter_text, sizeof(t_pattern_out.alter_text));
	}

	// ignore fixed flag, supply is always fixed
	//if(YES != pattern_out.pattern.fixed) 
	{

		INIT_EVENT(&event, GameEvent_PerSupplyCard, player, trigger, pParentEvent);
		event.pattern_out = &t_pattern_out;

		trigger_game_event(pGame, &event);
		//ret = per_passive_out_card(pGame, pParentEvent, player, target, &pattern_out);

		memcpy(out_pattern->ud, t_pattern_out.pattern.ud, sizeof(out_pattern->ud));

		RET_CHECK_CANCEL_RET(event.result, R_CANCEL);

		// be success directly (for example armor card skill may defend the attack of {sha} )
		RET_CHECK_SUCC_RET(event.result, R_SUCC);
	}



	ret = R_SUCC;

	while(ret == R_SUCC)
	{
		INIT_EVENT(&event, GameEvent_SupplyCard, player, trigger, pParentEvent);
		event.pattern_out = &t_pattern_out;

		set_game_cur_player(pGame, player);
		ret = cmd_loop(pGame, &event, NO, t_pattern_out.alter_text);
		CHECK_PLAYER_DEAD_RET(pGame, player, ret);
		RET_CHECK_RET(ret, ret);
		EVENT_CHECK_BREAK( &event);
	}

	memcpy(out_pattern->ud, t_pattern_out.pattern.ud, sizeof(out_pattern->ud));

	//RET_CHECK_BACK(event.result);

	// 提供牌的过程，并不会真正从提供者手里删除，直到真正使用者在使用该牌的时候
	*out_card = t_pattern_out.out;


	INIT_EVENT(&event, GameEvent_PostSupplyCard, player, trigger, pParentEvent);
	event.pattern_out = &t_pattern_out;

	trigger_game_event(pGame, &event);

	memcpy(out_pattern->ud, t_pattern_out.pattern.ud, sizeof(out_pattern->ud));

	// the supply cards is not effect
	RET_CHECK_CANCEL_RET(event.result, (clear_prepare_out_flag(pGame, pParentEvent, out_card) , R_CANCEL));

	return R_SUCC;
}

void game_init_outcard(OutCard* out_card)
{
	ST_ZERO(out_card);
}




RESULT game_call_out_drive(lua_State* L, tolua_notnull GameContext* pGame, tolua_notnull GameEventContext* pEvent, int player, int target, tolua_notnull OutCard* out_card)
{
	if(!IS_PLAYER_VALID(pGame, player))
	{
		luaL_error(GL(L), "game_supply_card: invalid player index - %d", player );
		return R_E_PARAM;
	}


	return out_card_drive(pGame, pEvent, player, target, out_card, 0);

}


