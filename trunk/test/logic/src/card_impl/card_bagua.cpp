#include "card_impl.h"

static YESNO card_bagua_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_bagua_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_bagua()
{
	static CardConfig card_bagua = {
		CardID_BaGua,
		CardType_Armor,
		"bagua",
		"",
		Target_Self,
		card_bagua_check,
		card_bagua_out,
	};

	return &card_bagua;

}

