#include "card_impl.h"


static YESNO card_chitu_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_chitu_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_chitu()
{
	static CardConfig card_chitu = {
		CardID_ChiTu,
		CardType_HorseChase,
		"chitu",
		"",
		card_chitu_check,
		card_chitu_out,
	};

	return &card_chitu;

}

