#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"


static YESNO card_aggression_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_aggression_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
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
		card_aggression_out,
	};

	return &card_aggression;

}



