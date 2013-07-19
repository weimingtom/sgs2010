// get.cpp 


#include "config.h"
#include "comm.h"
#include "game.h"
#include "event.h"
#include "card.h"
#include "get.h"
#include "cmd.h"
#include "stack.h"


// the new game first step , all player get 4 cards
RESULT game_first_getcard(GameContext* pGame)
{
	int n, k, player;
	RESULT  ret;
	Card stCard;

	MSG_OUT("the first time each player get 4 cards.\n");

	player = get_game_master_player(pGame);

	for(n = 0; n < pGame->player_count; n++)
	{
		// get 4 card each player
		for(k = 0; k < 4; k++)
		{
			CHECK_RET(ret = game_pop_stack_card(pGame, &stCard), ret);
			CHECK_RET(ret = player_add_hand_card(GAME_PLAYER(pGame, player), &stCard), ret);
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
	INIT_EVENT(&stEvent, GameEvent_PerGetCard, pGame->cur_player, 0, pEvent);
	trigger_game_event(pGame, &stEvent);

	return stEvent.result;
}

static RESULT  trigger_postgetcard_event(GameContext* pGame, GameEventContext* pEvent, PosCard* pos_card)
{
	GameEventContext  stEvent;
	INIT_EVENT(&stEvent, GameEvent_PostGetCard, pGame->cur_player, 0, pEvent);
	stEvent.pos_card = pos_card;

	trigger_game_event(pGame, &stEvent);

	return stEvent.result;

}


RESULT game_cmd_getcard(GameContext* pGame, GameEventContext* pEvent, int num)
{
	int n;
	PosCard  stCard;

	if(pEvent->id == GameEvent_RoundGetCard)
	{
		// in round get, must get 2 card

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
				trigger_postgetcard_event(pGame, pEvent, &stCard);

				// after event , the card  may be empty
				if(stCard.card.id != CardID_None)
				{
					player_add_hand_card(CUR_PLAYER(pGame), &stCard.card);
				}
			}
		}

		// goto out status
		return R_BACK;
	}
	else if(pEvent->id == GameEvent_PassiveGetCard)
	{
		// passive get without per/postgetcard event
		// get event.num cards
		for(n = 0; n < pEvent->get_card->num; n++)
		{
			if(YES != is_player_handfull(CUR_PLAYER(pGame)) 
				&& R_SUCC == game_pop_stack_card(pGame, &stCard.card))
			{
				player_add_hand_card(CUR_PLAYER(pGame), &stCard.card);
			}
		}

		return R_BACK;
	}
	
	return R_E_STATUS;
}

RESULT game_round_do_get(GameContext* pGame, GameEventContext* pEvent, int player, int num)
{
	RESULT ret;
	GameEventContext  event;
	char buffer[128];

	GetCard    stGetCard;

	stGetCard.num = num;

	
	INIT_EVENT(&event, GameEvent_RoundGetCard, player, 0, pEvent);
	event.get_card = &stGetCard;

	set_game_cur_player(pGame, player);

	snprintf(buffer, sizeof(buffer), "please get %d card:", stGetCard.num);

	ret = cmd_loop(pGame, &event, buffer);

	(void)ret;

	return R_SUCC;
}



RESULT game_passive_getcard(GameContext* pGame, GameEventContext* pEvent, int player, int num)
{
	RESULT ret;
	GameEventContext  event;
	char buffer[128];

	GetCard    stGetCard;

	stGetCard.num = num;


	INIT_EVENT(&event, GameEvent_PassiveGetCard, player, 0, pEvent);
	event.get_card = &stGetCard;

	set_game_cur_player(pGame, player);

	snprintf(buffer, sizeof(buffer), "please get %d card:", stGetCard.num);

	ret = cmd_loop(pGame, &event, buffer);

	(void)ret;

	return R_SUCC;
}
