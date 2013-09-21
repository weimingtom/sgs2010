// get.cpp 


#include "config.h"
#include "comm.h"
#include "game.h"
#include "event.h"
#include "card.h"
#include "get.h"
#include "cmd.h"
#include "stack.h"
#include "script.h"
#include "discard.h"

// the new game first step , all player get 4 cards
RESULT game_first_getcard(GameContext* pGame)
{
	int n, k, player;
	RESULT  ret;
	Card stCard;

	MSG_OUT("起始手牌每玩家摸[4]张.\n");

	player = get_game_master_player(pGame);

	for(n = 0; n < pGame->player_count; n++)
	{
		// get 4 card each player
		for(k = 0; k < 4; k++)
		{
			RET_CHECK_RET(ret = game_pop_stack_card(pGame, &stCard), ret);
			RET_CHECK_RET(ret = player_add_hand_card(GAME_PLAYER(pGame, player), &stCard), ret);
		}

		// next player
		player = game_next_player(pGame, player);
	}

	//pGame->round_num = 1;
	//pGame->round_player = get_game_master_player(pGame);

	return R_SUCC;
}

static RESULT  trigger_pergetcard_event(GameContext* pGame, GameEventContext* pEvent)
{
	GameEventContext  stEvent;
	INIT_EVENT(&stEvent, GameEvent_PerGetCard, pGame->cur_player, INVALID_PLAYER, pEvent);
	trigger_game_event(pGame, &stEvent);

	return stEvent.result;
}

static RESULT  trigger_postgetcard_event(GameContext* pGame, GameEventContext* pEvent, PosCard* pos_card)
{
	GameEventContext  stEvent;
	INIT_EVENT(&stEvent, GameEvent_PostGetCard, pGame->cur_player, INVALID_PLAYER, pEvent);
	stEvent.pos_card = pos_card;

	trigger_game_event(pGame, &stEvent);

	return stEvent.result;

}


RESULT game_cmd_getcard(GameContext* pGame, GameEventContext* pEvent, int num)
{
	int n;
	PosCard  stCard;

	if(pEvent->id == GameEvent_RoundGetCard || pEvent->id == GameEvent_PassiveGetCard)
	{
		// in round get, must get 2 card, 2 can modify by skill

		if(pEvent->get_card->num != num)
		{
			MSG_OUT("get card num error! must be %d\n", pEvent->get_card->num);
			return R_E_PARAM;
		}

		for(n = 0; n < num; n++)
		{
			if(R_CANCEL != trigger_pergetcard_event(pGame, pEvent)
				&& YES != is_player_handfull(CUR_PLAYER(pGame)) 
				&& R_SUCC == game_pop_stack_card(pGame, &stCard.card))
			{
				stCard.card.flag = CardFlag_None;
				MSG_OUT("【%s】摸一张牌。\n", CUR_PLAYER(pGame)->name);
				trigger_postgetcard_event(pGame, pEvent, &stCard);

				// after event , the card  may be empty
				if(stCard.card.id != CardID_None)
				{
					player_add_hand_card(CUR_PLAYER(pGame), &stCard.card);
				}
			}
		}

		pEvent->block = YES;
		pEvent->result = R_SUCC;

		// goto out status
		return R_SUCC;
	}
/*	else if(pEvent->id == GameEvent_PassiveGetCard)
	{
		// in passive get, get specifyed number cards
		if(pEvent->get_card->num != num)
		{
			MSG_OUT("get card num error! must be %d\n", pEvent->get_card->num);
			return R_E_PARAM;
		}
		// passive get without per/postgetcard event
		// get event.num cards
		for(n = 0; n < pEvent->get_card->num; n++)
		{
			if(YES != is_player_handfull(CUR_PLAYER(pGame)) 
				&& R_SUCC == game_pop_stack_card(pGame, &stCard.card))
			{
				stCard.card.flag = CardFlag_None;
				MSG_OUT("【%s】摸一张牌。\n", CUR_PLAYER(pGame)->name);
				player_add_hand_card(CUR_PLAYER(pGame), &stCard.card);
			}
		}

		pEvent->block = YES;
		pEvent->result = R_SUCC;

		return R_SUCC;
	}
*/	else
	{
		MSG_OUT("当前状态不允许进行摸牌!\n");
	}
	
	return R_E_STATUS;
}

RESULT game_round_do_get(GameContext* pGame, GameEventContext* pEvent, int player, int num)
{
	RESULT ret;
	GameEventContext  event;
	//char buffer[128];

	GetCard    stGetCard;

	stGetCard.num = num;
	stGetCard.force = YES;
	snprintf(stGetCard.alter_text, sizeof(stGetCard.alter_text), "请摸[%d]张牌:", stGetCard.num);

	INIT_EVENT(&event, GameEvent_RoundGetCard, player, INVALID_PLAYER, pEvent);
	event.get_card = &stGetCard;


	// if get card force and no more skill can be used. do get directly
	if(YES == stGetCard.force && R_SUCC != check_player_event(pGame, &event, player, 0))
	{
		set_game_cur_player(pGame, player);
		game_cmd_getcard(pGame, &event, stGetCard.num);
		ret = R_SUCC;
	}
	else
	{
		ret = R_SUCC;
		while(ret == R_SUCC)
		{
			set_game_cur_player(pGame, player);
			ret = cmd_loop(pGame, &event, stGetCard.force, stGetCard.alter_text);
			game_flush_discard_cur(pGame);
			CHECK_PLAYER_DEAD_RET(pGame, player, ret);
			RET_CHECK_RET(ret, ret);
			EVENT_CHECK_BREAK(&event);
		}
	}


	return R_SUCC;
}



RESULT game_passive_getcard(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, int num, YESNO force)
{
	RESULT ret;
	GameEventContext  event;
	//char buffer[128];

	GetCard    stGetCard;

// 	if(pGame == NULL || pParentEvent == NULL)
// 	{
// 		luaL_error(GL(L), "game_passive_getcard: invalid null param");
// 		return R_E_PARAM;
// 	}
	
	
	if(!IS_PLAYER_VALID(pGame, player))
	{
		luaL_error(GL(L), "game_passive_getcard: invalid player index - %d", player );
		return R_E_PARAM;
	}

	if(num <= 0)
	{
		luaL_error(GL(L), "game_passive_getcard: invalid card num %d", num);
		return R_E_PARAM;
	}

	
	stGetCard.num = num;
	stGetCard.force = force;
	snprintf(stGetCard.alter_text, sizeof(stGetCard.alter_text), "请摸[%d]张牌:", stGetCard.num);

	INIT_EVENT(&event, GameEvent_PassiveGetCard, player, INVALID_PLAYER, pParentEvent);
	event.get_card = &stGetCard;


	// if get card force and no more skill can be used. do get directly
	if(YES == stGetCard.force && R_SUCC != check_player_event(pGame, &event, player, 0))
	{
		set_game_cur_player(pGame, player);
		game_cmd_getcard(pGame, &event, stGetCard.num);
		ret = R_SUCC;
	}
	else
	{
		ret = R_SUCC;
		while(ret == R_SUCC)
		{
			set_game_cur_player(pGame, player);
			ret = cmd_loop(pGame, &event, stGetCard.force, stGetCard.alter_text);
			CHECK_PLAYER_DEAD_RET(pGame, player, ret);
			RET_CHECK_RET(ret, ret);
			EVENT_CHECK_BREAK(&event);
		}
	}

	return R_SUCC;
}

// 从另一个玩那里得到牌（不能是自己）
RESULT game_player_getcard_from_player(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, int from_player, CardWhere where, int pos)
{
	RESULT  ret;
	Player* pPlayer;
	Player* pFromPlayer;
	VCard   vcard;
	int     n;
	
	if(player == from_player && where == CardWhere_PlayerHand)
		return R_SUCC;

	pPlayer = get_game_player(pGame, player);
	pFromPlayer = get_game_player(pGame, from_player);

	if(pPlayer == NULL || pFromPlayer == NULL)
	{
		luaL_error(GL(L), "gamr_player_getcard_from_player: input invalid %s", pPlayer==NULL? "player" : "from_player");
		return R_E_PARAM;
	}


	ret = get_player_card(pFromPlayer, where, pos, &vcard);

	if(ret != R_SUCC)
	{
		luaL_error(GL(L), "game_player_getcard_from_player: invalid card where/pos.");
		return R_E_PARAM;
	}
	
	if(get_player_card_flag(pPlayer, where, pos) !=  CardFlag_None)
	{
		luaL_error(GL(L), "game_player_getcard_from_player: can not  get this card from player.");
		return R_E_PARAM;
	}

	if(pPlayer->hand_card_num + vcard.rnum > MAX_HAND_CARD)
	{
		luaL_error(GL(L), "game_player_getcard_from_player:  player hand card overflow.");
		return R_E_OVERFLOW;
	}


	if (where == CardWhere_PlayerHand)
	{
		MSG_OUT("【%s】获得【%s】的第[%d]手牌。\n", pPlayer->name, pFromPlayer->name, pos);
	
	}
	else if (where == CardWhere_PlayerEquip)
	{
		MSG_OUT("【%s】获得【%s】的%s: %s.\n", pPlayer->name, pFromPlayer->name, equip_idx_str(pos), get_card_str(&vcard.vcard));
	}
	else
	{
		MSG_OUT("【%s】获得【%s】的判定区牌: %s。\n", pPlayer->name, pFromPlayer->name, get_card_str(&vcard.vcard));
	}



	player_remove_card(pFromPlayer, where, pos, NULL);


	for(n = 0; n < vcard.rnum; n++)
	{
		player_add_hand_card(pPlayer, &vcard.rcards[n]);
	}

	return R_SUCC;
}
