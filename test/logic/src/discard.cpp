#include "config.h"
#include "game.h"
#include "event.h"
#include "player.h"
#include "card.h"
#include "discard.h"
#include "stack.h"
#include "comm.h"
#include "cmd.h"
#include "script.h"


RESULT game_add_discard_cur(GameContext* pGame, const Card* pCard, int* pos)
{
	if(pGame->cur_discard_card_num >= MAX_CUR_DISCARD_NUM)
		return R_E_FAIL;

	pGame->cur_discard_cards[pGame->cur_discard_card_num] = *pCard;

	if(pos) *pos = pGame->cur_discard_card_num;

	pGame->cur_discard_card_num++;
	
	return R_SUCC;
}

/*
RESULT game_get_discard_cur(GameContext* pGame, int pos, Card* pCard)
{
	if(pos < 0 || pos >= pGame->cur_discard_card_num  || !CARD_VALID(&pGame->cur_discard_cards[pos]) )
		return R_E_FAIL;

	*pCard = pGame->cur_discard_cards[pos];
	
	return R_SUCC;
}

RESULT game_clr_discard_cur(GameContext* pGame, int pos)
{
	if(pos < 0 || pos >= pGame->cur_discard_card_num)
		return R_E_FAIL;
 
	ST_ZERO(pGame->cur_discard_cards[pos]);
	return R_SUCC;
}
*/

void game_flush_discard_cur(GameContext* pGame)
{
	int n;
	for(n = 0; n < pGame->cur_discard_card_num; n++)
	{
		if(CARD_VALID(&pGame->cur_discard_cards[n]))
		{
			card_stack_push(&pGame->discard_card_stack, &pGame->cur_discard_cards[n]);
		}
	}
	pGame->cur_discard_card_num = 0;
	ST_ZERO(pGame->cur_discard_cards);

}

RESULT game_round_discard_card(GameContext* pGame, GameEventContext* pParentEvent, int player)
{
	RESULT ret;
	DiscardPattern   dis;
	GameEventContext  event;
	//char buffer[128];
	Player* p;

	
	p = get_game_player(pGame, player);

	if(p == NULL)
		return R_E_FAIL;

	// 如果手牌数量大于当前体力值，则需要弃置部分手牌，直到手牌数量等于当前体力，不能弃多，也不能不弃
	if(p->hand_card_num > p->cur_life)
	{
		ST_ZERO(dis);
		dis.num = p->hand_card_num - p->cur_life;
		dis.where = PatternWhere_Hand;
		dis.force = YES;
		snprintf(dis.alter_text, sizeof(dis.alter_text), "请弃[%d]张手牌:", dis.num);

		INIT_EVENT(&event, GameEvent_RoundDiscardCard, player, INVALID_PLAYER, pParentEvent);
		event.discard_pattern = &dis;

		ret = R_SUCC;
		while (ret == R_SUCC)
		{
			set_game_cur_player(pGame, player);

			
			ret = cmd_loop(pGame, &event,dis.force, dis.alter_text);
			game_flush_discard_cur(pGame);

			CHECK_PLAYER_DEAD_RET(pGame, player, ret);
			RET_CHECK_RET(ret, ret);
			EVENT_CHECK_BREAK( &event);
		}
	}
	
	return R_SUCC;
}

static RESULT remove_discard_cards(GameContext* pGame, GameEventContext* pEvent, int player)
{
	int n;

	Player* pPlayer;

	pPlayer = get_game_player(pGame, player);

	if(pPlayer == NULL)
		return R_E_FAIL;

	// remove hand discard cards
	for(n = 0; n < pPlayer->hand_card_num; n++)
	{
		if(pPlayer->hand_cards[n].flag ==  CardFlag_PrepareDiscard)
		{
			game_player_discard_card(pGame, pEvent, player, CardWhere_PlayerHand, n);
			n--;
		}
	}

	// remove equip discard cards
	for(n = 0; n < EquipIdx_Max; n++)
	{
		if(pPlayer->equip_cards[n].flag ==  CardFlag_PrepareDiscard)
		{
			game_player_discard_card(pGame, pEvent, player, CardWhere_PlayerEquip, n);
		}
	}

	// remove judgment discard cards
	for(n = 0; n < pPlayer->judgment_card_num; n++)
	{
		if(pPlayer->judgment_cards[n].vcard.flag ==  CardFlag_PrepareDiscard)
		{
			game_player_discard_card(pGame, pEvent, player, CardWhere_PlayerJudgment, n);
			n--;
		}
	}
	return R_SUCC;
}

// 处理弃牌的命令
RESULT game_cmd_discard_card(GameContext* pGame, GameEventContext* pParentEvent, int* idx, int num)
{
	RESULT  ret;
	int     n, m;
	PosCard stCard[MAX_CARD_LIST_NUM];
	VCard vcard;
	Player* pPlayer;
	
	// 只有在弃牌阶段或者被动弃牌事件响应时才有效
	if(pParentEvent->id == GameEvent_RoundDiscardCard || pParentEvent->id == GameEvent_PassiveDiscardCard)
	{
		pPlayer = CUR_PLAYER(pGame);

		if(num > MAX_CARD_LIST_NUM)
		{
			MSG_OUT("discard card count %d is too many!\n", num);
			return R_E_PARAM;
		}

		if(pParentEvent->discard_pattern->num != NUM_ANY && num != pParentEvent->discard_pattern->num)
		{
			MSG_OUT("弃牌的数量不正确!\n");
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

			if(!CHECK_WHERE_PATTERN(stCard[n].where, pParentEvent->discard_pattern->where))
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
		}

		for(n = 0; n < num; n++)
		{
			set_player_card_flag(pPlayer, stCard[n].where, stCard[n].pos, CardFlag_PrepareDiscard);
		}

		ret = remove_discard_cards(pGame, pParentEvent, get_game_cur_player(pGame));
		if(R_SUCC != ret)
		{
			return R_E_FAIL;
		}

		// real discard cards number
		pParentEvent->discard_pattern->num = num;
		pParentEvent->block = YES;
		pParentEvent->result = R_SUCC;

		return R_SUCC;
	}

	return R_E_STATUS;
}


RESULT game_passive_discard(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent
							, int player, int where, int num, YESNO force, const char* alter_text, int* real_num)
{
	RESULT ret;
	char  snum[16];
	const char*  swhere;
	GameEventContext  event;
	DiscardPattern   dis;

	if(!IS_PLAYER_VALID(pGame, player))
	{
		luaL_error(GL(L), "game_passive_discard: invalid player index - %d", player );
		return R_E_PARAM;
	}

	if((where & (PatternWhere_Equip|PatternWhere_Hand|PatternWhere_Judgment)) == 0)
	{
		luaL_error(GL(L), "game_passive_discard: invalid where - %d", where );
		return R_E_PARAM;
	}

	if( NUM_ANY != num && (num <= 0 || num > player_count_card(get_game_player(pGame, player), where)) )
	{
		luaL_error(GL(L), "game_passive_discard: invalid num - %d", num );
		return R_E_PARAM;

	}
	


	ST_ZERO(dis);
	dis.num = num;
	dis.where = where;
	dis.force = force;
	if(alter_text != NULL && alter_text[0] != 0)
	{
		strncpy(dis.alter_text, alter_text, sizeof(dis.alter_text));
	}
	else
	{
		// 自动生成一个提示串
		if(num == NUM_ANY)
		{
			snprintf(snum, sizeof(snum), "%s", "任意");
		}
		else
		{
			snprintf(snum, sizeof(snum), "[%d]", num);
		}

		swhere = "";
		if(where == CardWhere_PlayerHand)
			swhere = "手";
		else if(where == CardWhere_PlayerEquip)
			swhere = "装备";
		else if(where == CardWhere_PlayerJudgment)
			swhere = "判定区";
		
		snprintf(dis.alter_text, sizeof(dis.alter_text), "请弃%s张%s牌：", snum, swhere);
	}

	INIT_EVENT(&event, GameEvent_PerPassiveDiscardCard, player, INVALID_PLAYER, pParentEvent);
	event.discard_pattern = &dis;

	trigger_game_event(pGame, &event);

	CHECK_PLAYER_DEAD_RET(pGame, player, R_CANCEL);

	// passive discard card can be canceled by skills
	RET_CHECK_CANCEL_RET(event.result, R_CANCEL);
	// passive discard card can be skipped directly return R_SUCC
	RET_CHECK_SUCC_RET(event.result, R_SUCC);

	INIT_EVENT(&event, GameEvent_PassiveDiscardCard, player, INVALID_PLAYER, pParentEvent);
	event.discard_pattern = &dis;

	ret = R_SUCC;
	while (ret == R_SUCC)
	{

		set_game_cur_player(pGame, player);
		ret = cmd_loop(pGame, &event, dis.force, dis.alter_text);

		CHECK_PLAYER_DEAD_RET(pGame, player, ret);
		RET_CHECK_RET(ret, ret);
		EVENT_CHECK_BREAK( &event);
		
	}

	//RET_CHECK_BACK(event.result);


	INIT_EVENT(&event, GameEvent_PostPassiveDiscardCard, player, INVALID_PLAYER, pParentEvent);
	event.discard_pattern = &dis;

	trigger_game_event(pGame, &event);

	// passive discard card can be canceled by skills
	RET_CHECK_CANCEL_RET(event.result, R_CANCEL);

	if(real_num) *real_num =  dis.num;

	return R_SUCC;
}

RESULT game_player_discard_card(GameContext* pGame, GameEventContext* pParentEvent, int player, CardWhere where, int pos)
{
	int n;
	GameEventContext    event;
	VCard     stCard;
	PosVCard  disCard;
	const char* swhere = "";

	Player* pPlayer = get_game_player(pGame, player);

	if(R_SUCC != get_player_card(pPlayer, where, pos, &stCard))
		return R_E_FAIL;

	disCard.vcard = stCard.vcard;
	disCard.list.num = stCard.rnum;

	for(n = 0; n < stCard.rnum; n++)
	{
		disCard.list.pcards[n].card = stCard.rcards[n];
		disCard.list.pcards[n].where = where;
		disCard.list.pcards[n].pos = pos;
	}


	// event: per discard card 
	INIT_EVENT(&event, GameEvent_PerDiscardCard, player, INVALID_PLAYER, pParentEvent);
	event.pos_vcard = &disCard;

	trigger_game_event(pGame, &event);

	// 暂时这里先不管返回值是否能防止弃牌。困为如果从这里返回，remove_discard_cards会进入死循环
	//CHECK_BACK_RET(event.result);

	if(R_SUCC == player_remove_card(pPlayer, where, pos, NULL))
	{
		//stCard.card.flag = CardFlag_None;
		//stCard.where = where;
		//stCard.pos = pos;
		for(n = 0; n < disCard.list.num; n++)
		{
			game_add_discard_cur(pGame, &disCard.list.pcards[n].card, &disCard.list.pcards[n].pos);
			disCard.list.pcards[n].where = CardWhere_CurDiscardStack;
		}

		if(where == CardWhere_PlayerHand)
			swhere = "手";
		else if(where == CardWhere_PlayerEquip)
			swhere = "装备";
		else if(where == CardWhere_PlayerJudgment)
			swhere = "判定区";

		MSG_OUT("【%s】弃%s牌 %s\n", pPlayer->name, swhere, get_vcard_str(&stCard));

		// event: post discard card
		
		INIT_EVENT(&event, GameEvent_PostDiscardCard, player, INVALID_PLAYER, pParentEvent);
		event.pos_vcard = &disCard;
		trigger_game_event(pGame, &event);

		// ignore event result

	}
	return R_SUCC;
}


YESNO is_cur_card_valid(GameContext* pGame, CardWhere where, int pos)
{
	if(where != CardWhere_CurDiscardStack)
		return NO;

	if(pos >= 0 && pos < pGame->cur_discard_card_num && CARD_VALID(&pGame->cur_discard_cards[pos]))
	{
		return YES;
	}

	return NO;
}

RESULT add_cur_card_to_player_hand(lua_State* L, GameContext* pGame, CardWhere where, int pos, int player)
{
	Player* p;
	char buf[128];

	if(YES != is_cur_card_valid(pGame, where, pos))
	{
		luaL_error(GL(L), "add_cur_card_to_player_hand - error: where=%d, pos=%d invalid card!", where, pos);
		return R_E_FAIL;				
	}

	p = get_game_player(pGame, player);
		
	if(p == NULL)
	{
		luaL_error(GL(L), "add_cur_card_to_player_hand - error: invalid player %d", player);
		return R_E_FAIL;
	}

	if(p->hand_card_num + 1 > MAX_HAND_CARD)
	{
		luaL_error(GL(L),"add_cur_card_to_player_hand - error: player %d hand full", player);
		return R_E_FAIL;
	}


	pGame->cur_discard_cards[pos].flag =  CardFlag_None;
	if(R_SUCC != player_add_hand_card(p, &pGame->cur_discard_cards[pos]))
	{
		luaL_error(GL(L),"add_cur_card_to_player_hand - error: player %d player_add_hand_card failed", player);
		return R_E_FAIL;
	}

	MSG_OUT("【%s】获得手牌 %s\n", p->name, card_str(&pGame->cur_discard_cards[pos], buf, sizeof(buf)));


	ST_ZERO(pGame->cur_discard_cards[pos]);



	return R_SUCC;
}


RESULT add_cur_card_to_player_judgment(lua_State* L, GameContext* pGame, Card* v, PosCardList* list, int player)
{
	int n;
	Player* p;
	char buf[128];
	VCard   vcard;

	if(list->num > MAX_RCARD_NUM)
	{
		luaL_error(GL(L),"add_cur_card_to_player_judgment - error:  too many list number %d", list->num);
		return R_E_FAIL;				
	}
	for(n = 0; n < list->num; n++)
	{
		if(YES != is_cur_card_valid(pGame, list->pcards[n].where, list->pcards[n].pos))
		{
			luaL_error(GL(L),"add_cur_card_to_player_judgment - error: where=%d, pos=%d invalid card!", list->pcards[n].where, list->pcards[n].pos);
			return R_E_FAIL;
		}
	}

	p = get_game_player(pGame, player);

	if(p == NULL)
	{
		luaL_error(GL(L),"add_cur_card_to_player_judgment - error: invalid player %d", player);
		return R_E_FAIL;
	}

	if(p->judgment_card_num + 1 > MAX_JUDGMENT_CARD)
	{
		luaL_error(GL(L),"add_cur_card_to_player_judgment - error: player %d hand full", player);
		return R_E_FAIL;
	}

	ST_ZERO(vcard);
	vcard.vcard = *v;
	vcard.rnum = list->num;
	vcard.vcard.flag = CardFlag_None;

	for(n = 0; n < list->num; n++)
	{
		vcard.rcards[n] = pGame->cur_discard_cards[list->pcards[n].pos];
		vcard.rcards[n].flag = CardFlag_None;
	}

	if(R_SUCC != player_add_judgment_card(p, &vcard))
	{
		luaL_error(GL(L),"add_cur_card_to_player_judgment - error: player %d player_add_hand_card failed", player);
		return R_E_FAIL;
	}


	MSG_OUT("【%s】获得判定区牌 %s\n", p->name, vcard_str(&vcard, buf, sizeof(buf)));

	for(n = 0; n < list->num; n++)
	{
		ST_ZERO(pGame->cur_discard_cards[list->pcards[n].pos]);
	}

	return R_SUCC;
}


int  game_pop_stack_card_dis(GameContext* pGame)
{
	int pos;
	Card  card;
	if(R_SUCC != game_pop_stack_card(pGame, &card)  || R_SUCC != game_add_discard_cur(pGame, &card, &pos))
	{
		return -1;
	}
	return pos;
}



Card*  game_get_discard_cur(GameContext* pGame, int pos)
{
	if(pos < 0 || pos >= pGame->cur_discard_card_num  || !CARD_VALID(&pGame->cur_discard_cards[pos]) )
		return NULL;

	return &pGame->cur_discard_cards[pos];

}
