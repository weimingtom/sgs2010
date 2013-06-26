#include "card_impl.h"



static YESNO card_swordqinghong_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card round , can equip this card
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
	return NO;
}


static RESULT card_swordqinghong_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_swordqinghong()
{
	static CardConfig card_swordqinghong = {
		CardID_SwordQingHong,
		CardType_Weapon,
		"swordqinghong",
		"",
		Target_Self,
		card_swordqinghong_check,
		card_swordqinghong_out,
	};

	return &card_swordqinghong;

}

