#include "card_impl.h"


static YESNO card_genwithair_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_genwithair_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
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


