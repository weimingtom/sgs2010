#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"

static YESNO card_sheildrenwang_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_sheildrenwang_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}




const CardConfig* get_card_sheildrenwang()
{
	static CardConfig card_sheildrenwang = {
		CardID_SheildRenWang,
		CardType_Armor,
		"sheildrenwang",
		"",
		card_sheildrenwang_check,
		card_sheildrenwang_out,
	};

	return &card_sheildrenwang;

}

