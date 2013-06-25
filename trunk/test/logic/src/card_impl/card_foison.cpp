#include "card_impl.h"



static YESNO card_foison_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_foison_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_foison()
{
	static CardConfig card_foison = {
		CardID_Foison,
		CardType_Strategy,
		"foison",
		"",
		Target_All,
		card_foison_check,
		card_foison_out,
	};

	return &card_foison;

}

