#include "card_impl.h"

static YESNO card_swordqinglong_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_swordqinglong_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_swordqinglong()
{
	static CardConfig card_swordqinglong = {
		CardID_SwordQingLong,
		CardType_Weapon,
		"swordqinglong",
		"",
		card_swordqinglong_check,
		card_swordqinglong_out,
	};

	return &card_swordqinglong;

}

