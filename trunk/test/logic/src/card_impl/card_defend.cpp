#include "card_impl.h"




const CardConfig* get_card_defend()
{
	static CardConfig card_defend = {
		CardID_Defend,
		CardType_Normal,
		"��",
		"�����ܵ���ɱ���Ĺ���ʱ�������ʹ��һ�š�������������ɱ����Ч����"
		"\n����ͨ�������ֻ���ڻغ���ʹ�û�����",
		Target_None,
		NULL,
		NULL,
		NULL,
		NULL,
	};

	return &card_defend;
}