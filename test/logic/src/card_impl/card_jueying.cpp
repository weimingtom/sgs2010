#include "card_impl.h"


static YESNO card_jueying_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card round , can equip this card
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
	return NO;
}


static RESULT card_jueying_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_jueying()
{
	static CardConfig card_jueying = {
		CardID_JueYing,
		CardType_HorseRun,
		"jueying",
		"",
		Target_Self,
		card_jueying_check,
		card_jueying_out,
	};

	return &card_jueying;

}

