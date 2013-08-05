#include "config.h"
#include "game.h"
#include "event.h"
#include "card.h"
#include "stack.h"
#include "decide.h"
#include "discard.h"

#include "comm.h"


YESNO game_decide_card(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent,int player, const char* szPattern)
{
	GameEventContext  event;
	RESULT ret;
	int       pos;
	PosCard   stCard;
	CardPattern pattern;
	//VCard   stVCard;
	char temp[128];
	char temp2[128];


	if(!IS_PLAYER_VALID(pGame, player))
	{
		if(L) {
			luaL_error(L, "game_decide_card: invalid player index - %d", player );
		} else {
			MSG_OUT("game_decide_card: invalid player index - %d\n", player );
		}
		return NO;
	}

	ST_ZERO(pattern);
	if(R_SUCC != load_card_pattern(&pattern, szPattern, -1))
	{
		if(L) {
			luaL_error(L, "game_decide_card: error card pattern \"%s\"", pattern);
		} else {
			MSG_OUT("game_decide_card: error card pattern \"%s\"\n", pattern);
		}
		return NO;
	}

	ST_ZERO(stCard);

	// GameEvent_PerDecideCard
	INIT_EVENT(&event, GameEvent_PerDecideCard, player, INVALID_PLAYER, pParentEvent);
	event.pos_card = &stCard;
	ret = trigger_game_event(pGame, &event);
	(void)ret;

	// some skill can make the decide success or fail directly.
	if(event.result == R_CANCEL || event.result == R_ABORT)
		return NO;

	if(event.result == R_SKIP)
		return YES;

	// some skill can out a hand card to instead the decide card whill be poped from the stack
	if(!CARD_VALID(&stCard.card))
	{
		ret = game_pop_stack_card(pGame, &stCard.card);
		CHECK_RET(ret, NO);
		//set_vcard_from_card(&stVCard, &stCard.card);
		game_add_discard_cur(pGame, &stCard.card, &pos);
		stCard.where = CardWhere_CurDiscardStack;
		stCard.pos = pos;
		MSG_OUT("从摸牌堆顶得到判定牌 %s\n", card_str(&stCard.card, temp, sizeof(temp)));
	}


	// some skill can out a card instead the current decide card. do it in GameEvent_PerDecideCardCalc event

	// GameEvent_PerDecideCardCalc 
	INIT_EVENT(&event, GameEvent_PerDecideCardCalc, player, INVALID_PLAYER, pParentEvent);
	event.pos_card = &stCard;
	trigger_game_event(pGame, &event);

	ret = card_match(&stCard.card, sizeof(PosCard), 1, &pattern, 1);


	MSG_OUT("判定牌 %s,判定条件 %s,结果：%s\n",card_str(&stCard.card, temp, sizeof(temp)), card_pattern_str(&pattern, temp2, sizeof(temp2)), ret == R_SUCC ? "成功" : "失败");

	// some skill can get the decide card after it be calculated.
	// GameEvent_PostDecideCard
	INIT_EVENT(&event, GameEvent_PostDecideCard, player, INVALID_PLAYER, pParentEvent);
	event.pos_card = &stCard;
	trigger_game_event(pGame, &event);


	return B2YESNO(ret == R_SUCC);
}


