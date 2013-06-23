#include "card_impl.h"




static YESNO card_attack_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// reset attack count in round begin
	if(pEvent->id == GameEvent_RoundBegin)
	{
		pGame->players[player].params[0] = 0; 
	}


	// use in round out 
	if(pEvent->id == GameEvent_RoundOutCard)
	{
		if(pGame->players[player].params[0] == 0)
			return YES;
	}

	// other ways : NO
	return  NO;
}

static RESULT card_attack_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	RESULT ret;
	int target;
	// select target
	while(1)
	{
		ret = game_select_target(pGame, pEvent, player, 1, &target);
				
	}
	
	return R_SUCC;
}





static RESULT card_attack_calc(GameContext* pGame, GameEventContext* pEvent, int player)
{

	return R_SUCC;
}









const CardConfig* get_card_attack()
{
	static CardConfig card_attack = {
		CardID_Attack,
		CardType_Normal,
		"ɱ",
		"��ĳ��ƽ׶Σ��Գ����⣬�㹥����Χ�ڵ�һ����ɫʹ�ã�Ч���ǶԸý�ɫ���1���˺���"
		"\n����Ϸ��ʼʱ��Ĺ�����Χ��1	��"
		"\n��ÿ�����ƽ׶���ֻ��ʹ��һ�š�ɱ����",
		card_attack_check,
		card_attack_out,
		card_attack_calc,
		NULL,  
	};

	return &card_attack;
}



