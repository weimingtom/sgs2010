#include "card_impl.h"


static YESNO card_snitch_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_snitch_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_snitch()
{
	static CardConfig card_snitch = {
		CardID_Snitch,
		CardType_Strategy,
		"snitch",
		"",
		Target_OtherOneIn1,
		card_snitch_check,
		card_snitch_out,
	};

	return &card_snitch;

}

