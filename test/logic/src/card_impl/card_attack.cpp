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
		"杀",
		"你的出牌阶段，对除你外，你攻击范围内的一名角色使用，效果是对该角色造成1点伤害。"
		"\n★游戏开始时你的攻击范围是1	。"
		"\n★每个出牌阶段你只能使用一张【杀】。",
		card_attack_check,
		card_attack_out,
		card_attack_calc,
		NULL,  
	};

	return &card_attack;
}



