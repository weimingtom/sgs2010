#include "card_impl.h"



static YESNO card_halberdfangtian_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_halberdfangtian_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_halberdfangtian()
{
	static CardConfig card_halberdfangtian = {
		CardID_HalberdFangTian,
		CardType_Weapon,
		"halberdfangtian",
		"",
		Target_Self,
		card_halberdfangtian_check,
		card_halberdfangtian_out,
	};

	return &card_halberdfangtian;

}

