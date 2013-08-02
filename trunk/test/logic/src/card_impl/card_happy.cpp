#include "card_impl.h"


static YESNO card_happy_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_happy_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_happy()
{
	static CardConfig card_happy = {
		CardID_Happy,
		CardType_Strategy,
		"happy",
		"",
		Target_OtherOne,
		card_happy_check,
		card_happy_out,
	};

	return &card_happy;

}

