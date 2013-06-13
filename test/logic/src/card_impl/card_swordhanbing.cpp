#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"

static YESNO card_swordhanbing_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_swordhanbing_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_swordhanbing()
{
	static CardConfig card_swordhanbing = {
		CardID_SwordHanBing,
		CardType_Weapon,
		"swordhanbing",
		"",
		card_swordhanbing_check,
		card_swordhanbing_out,
	};

	return &card_swordhanbing;

}

