#include "card_impl.h"





static YESNO card_bowqiling_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card round , can equip this card
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
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

