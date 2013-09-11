#include "config.h"
#include "game.h"
#include "event.h"
#include "card.h"
#include "stack.h"
#include "decide.h"
#include "discard.h"
#include "script.h"
#include "comm.h"
#include "cmd.h"


YESNO game_decide_card(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent,int player, const char* szPattern)
{
	GameEventContext  event;
	int       pos;

	DecideCard decide_card;

	RESULT     ret;
	//VCard   stVCard;


	if(!IS_PLAYER_VALID(pGame, player))
	{
		luaL_error(GL(L), "game_decide_card: invalid player index - %d", player );
		return NO;
	}

	ST_ZERO(decide_card);


	if(R_SUCC != load_card_pattern(&decide_card.pattern, szPattern, -1))
	{
		luaL_error(GL(L), "game_decide_card: error card pattern \"%s\"", szPattern);
		return NO;
	}


	// GameEvent_PerDecideCard
	INIT_EVENT(&event, GameEvent_PerDecideCard, player, INVALID_PLAYER, pParentEvent);
	event.decide_card = &decide_card;
	trigger_game_event(pGame, &event);

	// some skill can make the decide success or fail directly.
	RET_CHECK_CANCEL_RET(event.result, NO);
	RET_CHECK_SUCC_RET(event.result, YES);

	// some skill can out a hand card to instead the decide card will be popped from the stack
	if(!CARD_VALID(&decide_card.pos_card.card))
	{
		ret = game_pop_stack_card(pGame, &decide_card.pos_card.card);
		RET_CHECK_RET(ret, NO);

		//set_vcard_from_card(&stVCard, &stCard.card);
		game_add_discard_cur(pGame, &decide_card.pos_card.card, &pos);
		decide_card.pos_card.where = CardWhere_CurDiscardStack;
		decide_card.pos_card.pos = pos;

		MSG_OUT("从摸牌堆顶得到判定牌 %s\n", get_card_str(&decide_card.pos_card.card));
	}


	// some skill can out a card instead the current decide card. do it in GameEvent_PerDecideCardCalc event

	// GameEvent_PerDecideCardCalc 
	INIT_EVENT(&event, GameEvent_PerDecideCardCalc, player, INVALID_PLAYER, pParentEvent);
	event.decide_card = &decide_card;

	// ignored event return. cannot cancel in this step
	trigger_game_event(pGame, &event);

	ret = card_match(&decide_card.pos_card.card, sizeof(PosCard), 1, &decide_card.pattern, 1);
	decide_card.result = B2YESNO(ret == R_SUCC);

	MSG_OUT("判定牌 %s,判定条件 %s,结果：%s\n",get_card_str(&decide_card.pos_card.card), 
			get_pattern_str(&decide_card.pattern), ret == R_SUCC ? "成功" : "失败");

	// some skill can get the decide card after it be calculated.
	// GameEvent_PostDecideCard
	INIT_EVENT(&event, GameEvent_PostDecideCard, player, INVALID_PLAYER, pParentEvent);
	event.decide_card = &decide_card;

	// ignore return result
	trigger_game_event(pGame, &event);

	// decide_card.result can be modified in GameEvent_PostDecideCard event
	return decide_card.result;
}


