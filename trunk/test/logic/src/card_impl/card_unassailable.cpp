#include "card_impl.h"


static YESNO card_unassailable_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_unassailable_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_unassailable()
{
	static CardConfig card_unassailable = {
		CardID_Unassailable,
		CardType_Strategy,
		"unassailable",
		"",
		Target_None,
		card_unassailable_check,
		card_unassailable_out,
	};

	return &card_unassailable;

}

