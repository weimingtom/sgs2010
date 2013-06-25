#include "card_impl.h"



static YESNO card_zixing_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_zixing_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_zixing()
{
	static CardConfig card_zixing = {
		CardID_ZiXing,
		CardType_HorseChase,
		"zixing",
		"",
		Target_Self,
		card_zixing_check,
		card_zixing_out,
	};

	return &card_zixing;

}

