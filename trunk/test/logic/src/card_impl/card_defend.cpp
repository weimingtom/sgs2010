#include "card_impl.h"




const CardConfig* get_card_defend()
{
	static CardConfig card_defend = {
		CardID_Defend,
		CardType_Normal,
		"闪",
		"当你受到【杀】的攻击时，你可以使用一张【闪】来抵消【杀】的效果。"
		"\n★闪通常情况下只能在回合外使用或打出。",
		Target_None,
		NULL,
		NULL,
		NULL,
		NULL,
	};

	return &card_defend;
}