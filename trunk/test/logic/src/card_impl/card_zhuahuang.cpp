#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"


static YESNO card_zhuahuang_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_zhuahuang_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_zhuahuang()
{
	static CardConfig card_zhuahuang = {
		CardID_ZhuaHuang,
		CardType_HorseRun,
		"zhuahuang",
		"",
		card_zhuahuang_check,
		card_zhuahuang_out,
	};

	return &card_zhuahuang;

}

