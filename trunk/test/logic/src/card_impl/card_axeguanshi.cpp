#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"

static YESNO card_axeguanshi_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_axeguanshi_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_axeguanshi()
{
	static CardConfig card_axeguanshi = {
		CardID_AxeGuanShi,
		CardType_Weapon,
		"axeguanshi",
		"",
		card_axeguanshi_check,
		card_axeguanshi_out,
	};

	return &card_axeguanshi;

}

