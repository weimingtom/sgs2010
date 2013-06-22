#include "card_impl.h"


static YESNO card_dilu_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_dilu_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_dilu()
{
	static CardConfig card_dilu = {
		CardID_DiLu,
		CardType_HorseRun,
		"dilu",
		"",
		card_dilu_check,
		card_dilu_out,
	};

	return &card_dilu;

}

