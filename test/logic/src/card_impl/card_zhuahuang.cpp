#include "card_impl.h"


static YESNO card_zhuahuang_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_zhuahuang_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
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

