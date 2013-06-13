#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"

static YESNO card_spearzhangba_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_spearzhangba_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_spearzhangba()
{
	static CardConfig card_spearzhangba = {
		CardID_SpearZhangBa,
		CardType_Weapon,
		"spearzhangba",
		"",
		card_spearzhangba_check,
		card_spearzhangba_out,
	};

	return &card_spearzhangba;

}

