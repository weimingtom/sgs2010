#include "config.h"
#include "game.h"
#include "event.h"
#include "player.h"
#include "card.h"
#include "discard.h"
#include "stack.h"
#include "comm.h"
#include "cmd.h"


RESULT game_add_discard_cur(GameContext* pGame, const Card* pCard, int* pos)
{
	if(pGame->cur_discard_card_num >= MAX_CUR_DISCARD_NUM)
		return R_E_FAIL;

	pGame->cur_discard_cards[pGame->cur_discard_card_num] = *pCard;

	if(pos) *pos = pGame->cur_discard_card_num;

	pGame->cur_discard_card_num++;
	
	return R_SUCC;
}

RESULT game_get_discard_cur(GameContext* pGame, int pos, Card* pCard)
{
	if(pos < 0 || pos >= pGame->cur_discard_card_num || !CARD_VALID(&pGame->cur_discard_cards[pos]))
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
	GameEventContext  event;
	char buffer[128];
	Player* p;

	INIT_EVENT(&event, GameEvent_RoundDiscardCard, player, INVALID_PLAYER, pParentEvent);
	
	p = get_game_player(pGame, player);

	if(p == NULL)
		return R_E_FAIL;

	while(p->hand_card_num > p->cur_life)
	{
		set_game_cur_player(pGame, player);
		
		snprintf(buffer, sizeof(buffer), "请弃[%d]张牌:", p->hand_card_num - p->cur_life);
		
		ret = cmd_loop(pGame, &event, buffer);
	}

	
	(void)ret;
	
	return R_SUCC;
}

RESULT game_cmd_discard_card(GameContext* pGame, GameEventContext* pParentEvent, int* idx, int num)
{
	int n;
	PosCard stCard[MAX_CARD_LIST_NUM];

	Player* pPlayer = CUR_PLAYER(pGame);

	if(pPlayer->hand_card_num - pPlayer->cur_life < num || num > MAX_CARD_LIST_NUM)
	{
		MSG_OUT("discard card count %d is too many!\n", num);
		return R_E_PARAM;
	}

	for(n = 0; n < num; n++)
	{
		if(R_SUCC != player_card_idx_to_pos(pPlayer, idx[n], &stCard[n].where, &stCard[n].pos))
		{
			MSG_OUT("input card idx [%d] is error!\n", idx[n]);
			return R_E_PARAM;
		}

		if(stCard[n].where != CardWhere_PlayerHand)
		{
			MSG_OUT("card idx [%d] is invalid place!\n", idx[n]);
			return R_E_PARAM;
		}

		get_player_card(pPlayer,stCard[n].where, stCard[n].pos, &stCard[n].card);
	}

	for(n = 0; n < num; n++)
	{
		set_player_card_flag(pPlayer, stCard[n].where, stCard[n].pos, CardFlag_PrepareDiscard);
	}

	return R_E_STATUS;
}



RESULT game_player_discard_card(GameContext* pGame, GameEventContext* pParentEvent, int player, CardWhere where, int pos)
{
	GameEventContext    event;
	PosCard  stCard;
	char buf[256];
	Player* pPlayer = get_game_player(pGame, player);

	if(R_SUCC != get_player_card(pPlayer, where, pos, &stCard.card))
		return R_E_FAIL;

	stCard.where = where;
	stCard.pos = pos;

	// event: per discard card 
	INIT_EVENT(&event, GameEvent_PerDiscardCard, player, INVALID_PLAYER, pParentEvent);
	event.pos_card = &stCard;

	trigger_game_event(pGame, &event);

	if(R_SUCC == player_remove_card(pPlayer, where, pos, &stCard.card))
	{
		//stCard.card.flag = CardFlag_None;
		//stCard.where = where;
		//stCard.pos = pos;
		game_add_discard_cur(pGame, &stCard.card, &stCard.pos);
		stCard.where = CardWhere_CurDiscardStack;

		MSG_OUT("【%s】弃牌 %s\n", pPlayer->name, card_str(&stCard.card, buf, sizeof(buf)));

		// event: post discard card
		
		INIT_EVENT(&event, GameEvent_PostDiscardCard, player, INVALID_PLAYER, pParentEvent);
		event.pos_card = &stCard;
		trigger_game_event(pGame, &event);

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

RESULT add_cur_card_to_player_hand(GameContext* pGame, CardWhere where, int pos, int player)
{
	Player* p;
	char buf[128];

	if(YES != is_cur_card_valid(pGame, where, pos))
	{
		return R_E_FAIL;				
	}

	p = get_game_player(pGame, player);
		
	if(p == NULL)
	{
		return R_E_FAIL;
	}

	if(R_SUCC != player_add_hand_card(p, &pGame->cur_discard_cards[pos]))
		return R_E_FAIL;


	MSG_OUT("玩家【%s】获得手牌 %s\n", p->name, card_str(&pGame->cur_discard_cards[pos], buf, sizeof(buf)));
	RESET_CARD(&pGame->cur_discard_cards[pos]);

	return R_SUCC;
}

RESULT add_cur_card_to_player_judgment(GameContext* pGame, CardWhere where, int pos, int player)
{
	Player* p;
	char buf[128];

	if(YES != is_cur_card_valid(pGame, where, pos))
	{
		return R_E_FAIL;				
	}

	p = get_game_player(pGame, player);

	if(p == NULL)
	{
		return R_E_FAIL;
	}

	if(R_SUCC != player_add_judgment_card(p, &pGame->cur_discard_cards[pos]))
		return R_E_FAIL;


	MSG_OUT("玩家【%s】获得手牌 %s\n", p->name, card_str(&pGame->cur_discard_cards[pos], buf, sizeof(buf)));
	RESET_CARD(&pGame->cur_discard_cards[pos]);


	return R_SUCC;
}
