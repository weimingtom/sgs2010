#include "card_impl.h"


static YESNO card_kickladder_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_kickladder_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_kickladder()
{
	static CardConfig card_kickladder = {
		CardID_KickLadder,
		CardType_Strategy,
		"kickladder",
		"",
		Target_OtherOne,
		card_kickladder_check,
		card_kickladder_out,
	};

	return &card_kickladder;

}

