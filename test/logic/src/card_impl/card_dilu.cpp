#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"


static YESNO card_dilu_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_dilu_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_dilu()
{
	static CardConfig card_dilu = {
		CardID_DiLu,
		CardType_HorseRun,
		"dilu",
		"",
		card_dilu_check,
		card_dilu_out,
	};

	return &card_dilu;

}

