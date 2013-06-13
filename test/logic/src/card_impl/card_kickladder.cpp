#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"


static YESNO card_kickladder_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_kickladder_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_kickladder()
{
	static CardConfig card_kickladder = {
		CardID_KickLadder,
		CardType_Strategy,
		"kickladder",
		"",
		card_kickladder_check,
		card_kickladder_out,
	};

	return &card_kickladder;

}

