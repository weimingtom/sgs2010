#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"


static YESNO card_defend_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


int card_defend_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}


const CardConfig* get_card_defend()
{
	static CardConfig card_defend = {
		CardID_Defend,
		CardType_Normal,
		"闪",
		"当你受到【杀】的攻击时，你可以使用一张【闪】来抵消【杀】的效果。"
		"\n★闪通常情况下只能在回合外使用或打出。",
		card_defend_check,
		card_defend_out,
	};

	return &card_defend;
}