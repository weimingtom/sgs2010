#include "card_impl.h"



static RESULT out_card_check_event(GameContext* pGame, GameEventContext* pParentEvent, int player, int target)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_OutCardCheck, player, target, pParentEvent);
	event.card.id = CardID_Attack;
	event.result = R_DEF;

	trigger_game_event(pGame, &event);

	if(event.result ==R_SUCC )
		return R_SUCC;
	else if(event.result ==R_E_FAIL)
		return R_E_FAIL;
	// default
	return pGame->players[player].params[0] == 0 ? R_SUCC : R_E_FAIL;
}


static YESNO card_attack_check(GameContext* pGame, GameEventContext* pEvent, int player)
{

	if(pEvent == NULL)
	{
		// my out round 
		if(pGame->nRoundPlayer == player && out_card_check_event(pGame, pEvent, player, 0))
			return YES;
	}
	else
	{

		// reset attack count in round begin
		if(pEvent->id == GameEvent_RoundBegin)
		{
			pGame->players[player].params[0] = 0; 
		}


		// use in force out 
		if(pEvent->id == GameEvent_PassiveOutCard && pEvent->card.id == CardID_Attack)
			return YES;
	}

	// other ways : NO
	return  NO;
}

static RESULT card_attack_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// select target
	while(1)
	{
		printf("select target:\n");
		
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



