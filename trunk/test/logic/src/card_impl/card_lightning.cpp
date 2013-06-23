#include "card_impl.h"



static YESNO card_lightning_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_lightning_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_lightning()
{
	static CardConfig card_lightning = {
		CardID_Lightning,
		CardType_Strategy,
		"lightning",
		"",
		card_lightning_check,
		card_lightning_out,
	};

	return &card_lightning;

}

