#include "card_impl.h"


static YESNO card_aggression_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_aggression_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_aggression()
{
	static CardConfig card_aggression = {
		CardID_Aggression,
		CardType_Strategy,
		"��������",
		"���ƽ׶Σ��Գ�����������н�ɫʹ�á����ж�˳����㡫����Ŀ���ɫ���һ�š�ɱ��������ý�ɫ�ܵ����������֡�������ɵ�1���˺���"
		"\n�������ָ������������н�ɫΪĿ�꣬Ȼ���������Σ�������¼ҿ�ʼ��ѡ���Ƿ�����ɱ����",
		card_aggression_check,
		card_aggression_use,
	};

	return &card_aggression;

}



