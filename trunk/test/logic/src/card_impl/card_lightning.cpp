#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"



static YESNO card_lightning_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_lightning_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_lightning()
{
	static CardConfig card_lightning = {
		CardID_Lightning,
		CardType_Strategy,
		"lightning",
		"",
		card_lightning_check,
		card_lightning_out,
	};

	return &card_lightning;

}

