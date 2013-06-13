#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"


static YESNO card_makebrother_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_makebrother_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_makebrother()
{
	static CardConfig card_makebrother = {
		CardID_MakeBrother,
		CardType_Strategy,
		"makebrother",
		"",
		card_makebrother_check,
		card_makebrother_out,
	};

	return &card_makebrother;

}

