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
		"��",
		"�����ܵ���ɱ���Ĺ���ʱ�������ʹ��һ�š�������������ɱ����Ч����"
		"\n����ͨ�������ֻ���ڻغ���ʹ�û�����",
		card_defend_check,
		card_defend_out,
	};

	return &card_defend;
}