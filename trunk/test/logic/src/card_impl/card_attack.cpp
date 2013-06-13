#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"


static int out_card_check_event(GameContext* pGame, GameEventContext* pParentEvent, int player, int target)
{
	GameEventContext  event;
	event.id = GameEvent_OutCardCheck;
	event.trigger = player;
	event.target = target;
	event.parent_event = pParentEvent;
	event.out_check.card.id = CardID_Attack;
	event.out_check.result = Check_Unknown;

	trigger_game_event(pGame, &event);

	if(event.out_check.result == Check_Yes )
		return 1;
	else if(event.out_check.result == Check_No)
		return 0;
	// default
	return pGame->players[player].params[0] == 0;

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
		if(pEvent->id == GameEvent_PassiveOutCard && pEvent->passive_out.id == CardID_Attack)
			return YES;
	}

	// other ways : NO
	return  NO;
}

static int card_attack_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// select target

	
	return 0;
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
	};

	return &card_attack;
}



