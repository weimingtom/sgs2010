#include "card_impl.h"



static YESNO card_zixing_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_zixing_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_zixing()
{
	static CardConfig card_zixing = {
		CardID_ZiXing,
		CardType_HorseChase,
		"zixing",
		"",
		card_zixing_check,
		card_zixing_out,
	};

	return &card_zixing;

}

