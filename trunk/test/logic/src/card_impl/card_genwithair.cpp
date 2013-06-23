#include "card_impl.h"


static YESNO card_genwithair_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_genwithair_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_genwithair()
{
	static CardConfig card_genwithair = {
		CardID_GenWithAir,
		CardType_Strategy,
		"genwithair",
		"",
		card_genwithair_check,
		card_genwithair_out,
	};

	return &card_genwithair;

}


