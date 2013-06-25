#include "card_impl.h"





static YESNO card_bowqiling_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_bowqiling_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_bowqiling()
{
	static CardConfig card_bowqiling = {
		CardID_BowQiLing,
		CardType_Weapon,
		"bowqiling",
		"",
		Target_Self,
		card_bowqiling_check,
		card_bowqiling_out,
	};

	return &card_bowqiling;

}

