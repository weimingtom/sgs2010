#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"


static YESNO card_jueying_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_jueying_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_jueying()
{
	static CardConfig card_jueying = {
		CardID_JueYing,
		CardType_HorseRun,
		"jueying",
		"",
		card_jueying_check,
		card_jueying_out,
	};

	return &card_jueying;

}

