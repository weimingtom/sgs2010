#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"


static YESNO card_chitu_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_chitu_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_chitu()
{
	static CardConfig card_chitu = {
		CardID_ChiTu,
		CardType_HorseChase,
		"chitu",
		"",
		card_chitu_check,
		card_chitu_out,
	};

	return &card_chitu;

}

