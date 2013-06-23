#include "card_impl.h"

static YESNO card_swordhanbing_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_swordhanbing_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_swordhanbing()
{
	static CardConfig card_swordhanbing = {
		CardID_SwordHanBing,
		CardType_Weapon,
		"swordhanbing",
		"",
		card_swordhanbing_check,
		card_swordhanbing_out,
	};

	return &card_swordhanbing;

}

