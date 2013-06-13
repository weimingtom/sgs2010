#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"

static YESNO card_bowqiling_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_bowqiling_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_bowqiling()
{
	static CardConfig card_bowqiling = {
		CardID_BowQiLing,
		CardType_Weapon,
		"bowqiling",
		"",
		card_bowqiling_check,
		card_bowqiling_out,
	};

	return &card_bowqiling;

}

