#include "card_impl.h"


static YESNO card_medicine_check(GameContext* pGame, GameEventContext* pEvent, int player)
{

	// in out round , you can use to self
	if(pGame->status == Status_Round_Out && pGame->nRoundPlayer == player)
		return YES;

	// in other way medicine only out in passive

	return NO;
}




static RESULT card_medicine_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	
	

	return R_SUCC;
}





static RESULT card_medicine_calc(GameContext* pGame, GameEventContext* pEvent, int player)
{

	return R_SUCC;
}













const CardConfig* get_card_medicine()
{
	static CardConfig card_medicine = {
		CardID_Medicine,
		CardType_Normal,
		"��",
		"���������������ʹ�ã�"
		"\n1������ĳ��ƽ׶Σ������ʹ�������ظ����1��������"
		"\n2�����н�ɫ���ڱ���״̬ʱ������ԶԸý�ɫʹ�á��ҡ�����ֹ�ý�ɫ��������"
		"\n�����磬һ����ɫ��ʣ������Ϊ2�㣬��ʱ�ܵ������硿��ɵ�3���˺�����ʱ�ý�ɫ���ڱ���״̬���ý�ɫ�������κ��˺ϼ���ʹ��2�š��ҡ����ܾȻأ�2-3+2=1����"
		"\n����ƽ׶Σ�����û����ʧ�������㲻���Զ��Լ�ʹ�á��ҡ���",
		Target_Self,
		card_medicine_check,
		card_medicine_out,
		card_medicine_calc,
		NULL,
	};

	return &card_medicine;

}



