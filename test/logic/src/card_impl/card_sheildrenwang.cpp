#include "card_impl.h"

static YESNO card_sheildrenwang_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card round , can equip this card
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
	return NO;
}


static RESULT card_sheildrenwang_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}




const CardConfig* get_card_sheildrenwang()
{
	static CardConfig card_sheildrenwang = {
		CardID_SheildRenWang,
		CardType_Armor,
		"sheildrenwang",
		"",
		Target_Self,
		card_sheildrenwang_check,
		card_sheildrenwang_out,
	};

	return &card_sheildrenwang;

}

