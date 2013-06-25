#include "card_impl.h"


static YESNO card_makebrother_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_makebrother_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_makebrother()
{
	static CardConfig card_makebrother = {
		CardID_MakeBrother,
		CardType_Strategy,
		"makebrother",
		"",
		Target_All,
		card_makebrother_check,
		card_makebrother_out,
	};

	return &card_makebrother;

}

