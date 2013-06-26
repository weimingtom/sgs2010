#include "card_impl.h"

static YESNO card_bagua_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card round , can equip this card
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
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

