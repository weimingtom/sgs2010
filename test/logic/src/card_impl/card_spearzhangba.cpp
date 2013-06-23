#include "card_impl.h"

static YESNO card_spearzhangba_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_spearzhangba_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_spearzhangba()
{
	static CardConfig card_spearzhangba = {
		CardID_SpearZhangBa,
		CardType_Weapon,
		"spearzhangba",
		"",
		card_spearzhangba_check,
		card_spearzhangba_out,
	};

	return &card_spearzhangba;

}

