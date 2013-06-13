#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"


static YESNO card_allattack_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}


static int card_allattack_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const CardConfig* get_card_allattack()
{
	static CardConfig card_allattack = {
		CardID_AllAttack,
		CardType_Strategy,
		"����뷢",
		"���ƽ׶Σ��Գ�����������н�ɫʹ�á����ж�˳����㡫����Ŀ���ɫ���һ�š�����������ý�ɫ�ܵ�������뷢��������ɵ�1���˺���"
		"\n�������ָ������������н�ɫΪĿ�꣬Ȼ�����ǣ�������¼ҿ�ʼ������ѡ���Ƿ�����������",
		card_allattack_check,
		card_allattack_out,
	};

	return &card_allattack;

}



