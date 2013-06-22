#include "card_impl.h"



static YESNO card_halberdfangtian_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_halberdfangtian_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_halberdfangtian()
{
	static CardConfig card_halberdfangtian = {
		CardID_HalberdFangTian,
		CardType_Weapon,
		"halberdfangtian",
		"",
		card_halberdfangtian_check,
		card_halberdfangtian_out,
	};

	return &card_halberdfangtian;

}

