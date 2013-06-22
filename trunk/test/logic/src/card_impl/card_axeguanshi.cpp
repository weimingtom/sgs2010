#include "card_impl.h"

static YESNO card_axeguanshi_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_axeguanshi_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_axeguanshi()
{
	static CardConfig card_axeguanshi = {
		CardID_AxeGuanShi,
		CardType_Weapon,
		"axeguanshi",
		"",
		card_axeguanshi_check,
		card_axeguanshi_out,
	};

	return &card_axeguanshi;

}

