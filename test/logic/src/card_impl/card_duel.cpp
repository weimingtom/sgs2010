#include "card_impl.h"

static YESNO card_duel_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_duel_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_duel()
{
	static CardConfig card_duel = {
		CardID_Duel,
		CardType_Strategy,
		"Duel",
		"",
		card_duel_check,
		card_duel_out,
	};

	return &card_duel;

}


