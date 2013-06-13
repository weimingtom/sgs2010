#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"

static YESNO card_bagua_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_bagua_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_bagua()
{
	static CardConfig card_bagua = {
		CardID_BaGua,
		CardType_Armor,
		"bagua",
		"",
		card_bagua_check,
		card_bagua_out,
	};

	return &card_bagua;

}

