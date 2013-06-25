#include "card_impl.h"

static YESNO card_liannu_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_liannu_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_liannu()
{
	static CardConfig card_liannu = {
		CardID_LianNu,
		CardType_Weapon,
		"liannu",
		"",
		Target_Self,
		card_liannu_check,
		card_liannu_out,
	};

	return &card_liannu;

}

