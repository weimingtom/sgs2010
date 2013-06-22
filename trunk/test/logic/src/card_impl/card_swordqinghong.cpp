#include "card_impl.h"



static YESNO card_swordqinghong_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_swordqinghong_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_swordqinghong()
{
	static CardConfig card_swordqinghong = {
		CardID_SwordQingHong,
		CardType_Weapon,
		"swordqinghong",
		"",
		card_swordqinghong_check,
		card_swordqinghong_out,
	};

	return &card_swordqinghong;

}

