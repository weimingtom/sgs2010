#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"


static YESNO card_unassailable_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_unassailable_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_unassailable()
{
	static CardConfig card_unassailable = {
		CardID_Unassailable,
		CardType_Strategy,
		"unassailable",
		"",
		card_unassailable_check,
		card_unassailable_out,
	};

	return &card_unassailable;

}

