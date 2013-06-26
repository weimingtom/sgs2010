#include "card_impl.h"

static YESNO card_dawan_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card round , can equip this card
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
	return NO;
}


static RESULT card_dawan_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_dawan()
{
	static CardConfig card_dawan = {
		CardID_DaWan,
		CardType_HorseChase,
		"dawan",
		"",
		Target_Self,
		card_dawan_check,
		card_dawan_out,
	};

	return &card_dawan;

}

