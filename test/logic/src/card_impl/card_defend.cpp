#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"




const CardConfig* get_card_defend()
{
	static CardConfig card_defend = {
		CardID_Defend,
		CardType_Normal,
		"��",
		"�����ܵ���ɱ���Ĺ���ʱ�������ʹ��һ�š�������������ɱ����Ч����"
		"\n����ͨ�������ֻ���ڻغ���ʹ�û�����",
		NULL,
		NULL,
		NULL,
		NULL,
	};

	return &card_defend;
}