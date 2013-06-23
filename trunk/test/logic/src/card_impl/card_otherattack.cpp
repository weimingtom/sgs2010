#include "card_impl.h"


static YESNO card_otherattack_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_otherattack_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_otherattack()
{
	static CardConfig card_otherattack = {
		CardID_OtherAttack,
		CardType_Strategy,
		"otherattack",
		"",
		card_otherattack_check,
		card_otherattack_out,
	};

	return &card_otherattack;

}

