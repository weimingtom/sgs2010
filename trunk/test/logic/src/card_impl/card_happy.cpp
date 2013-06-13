#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"


static YESNO card_happy_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_happy_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_happy()
{
	static CardConfig card_happy = {
		CardID_Happy,
		CardType_Strategy,
		"happy",
		"",
		card_happy_check,
		card_happy_out,
	};

	return &card_happy;

}

