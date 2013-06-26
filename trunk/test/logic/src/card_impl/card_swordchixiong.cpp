#include "card_impl.h"

static YESNO card_swordchixiong_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card round , can equip this card
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
	return NO;
}


static RESULT card_swordchixiong_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_swordchixiong()
{
	static CardConfig card_swordchixiong = {
		CardID_SwordChiXiong,
		CardType_Weapon,
		"swordchixiong",
		"",
		Target_Self,
		card_swordchixiong_check,
		card_swordchixiong_out,
	};

	return &card_swordchixiong;

}

