#include "card_impl.h"


static YESNO card_otherattack_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_otherattack_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
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

