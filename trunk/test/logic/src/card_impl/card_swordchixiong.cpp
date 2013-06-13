#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"

static YESNO card_swordchixiong_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_swordchixiong_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_swordchixiong()
{
	static CardConfig card_swordchixiong = {
		CardID_SwordChiXiong,
		CardType_Weapon,
		"swordchixiong",
		"",
		card_swordchixiong_check,
		card_swordchixiong_out,
	};

	return &card_swordchixiong;

}

