#include "card_impl.h"



static YESNO card_swordqinghong_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_swordqinghong_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
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

