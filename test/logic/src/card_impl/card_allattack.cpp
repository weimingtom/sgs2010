#include "card_impl.h"


static YESNO card_allattack_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static RESULT card_allattack_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const CardConfig* get_card_allattack()
{
	static CardConfig card_allattack = {
		CardID_AllAttack,
		CardType_Strategy,
		"����뷢",
		"���ƽ׶Σ��Գ�����������н�ɫʹ�á����ж�˳����㡫����Ŀ���ɫ���һ�š�����������ý�ɫ�ܵ�������뷢��������ɵ�1���˺���"
		"\n�������ָ������������н�ɫΪĿ�꣬Ȼ�����ǣ�������¼ҿ�ʼ������ѡ���Ƿ�����������",
		Target_OtherAll,
		card_allattack_check,
		card_allattack_use,
	};

	return &card_allattack;

}



