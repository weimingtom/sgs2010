#include "card_impl.h"

static YESNO card_sheildrenwang_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
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
		card_sheildrenwang_check,
		card_sheildrenwang_out,
	};

	return &card_sheildrenwang;

}

