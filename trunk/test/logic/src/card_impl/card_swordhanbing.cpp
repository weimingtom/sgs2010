#include "card_impl.h"

static YESNO card_swordhanbing_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card round , can equip this card
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
	return NO;
}


static RESULT card_swordhanbing_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_swordhanbing()
{
	static CardConfig card_swordhanbing = {
		CardID_SwordHanBing,
		CardType_Weapon,
		"swordhanbing",
		"",
		Target_Self,
		card_swordhanbing_check,
		card_swordhanbing_out,
	};

	return &card_swordhanbing;

}

