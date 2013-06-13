#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"

static YESNO card_dawan_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_dawan_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_dawan()
{
	static CardConfig card_dawan = {
		CardID_DaWan,
		CardType_HorseChase,
		"dawan",
		"",
		card_dawan_check,
		card_dawan_out,
	};

	return &card_dawan;

}

