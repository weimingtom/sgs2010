#include "hero_impl.h"





static YESNO paoxiao_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	if(pEvent->id == GameEvent_RoundOutCard)
	{
		return YES;
	}
	return NO;
}

static RESULT paoxiao_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	//reset attack count in this round
	//pGame->players[player].params[0] = 0;
	
	//MSG_OUT("please out a card 'sha' or cancel skill:");
	RESULT ret;

	//OutCardPattern pattern;
	OutCard        out_card;

	//pattern.num = 1;
	//INIT_CARDPATTERN_USE_ID(&pattern.patterns[0], CardID_Attack);
	//pattern.where = PlayerCard_Hand;

	ret = game_supply_card(NULL, pGame, pEvent, player, player, "h:{attack}", "please out a card 'attack' or cancel the skill:", &out_card);
	CHECK_RET(ret, ret);

	return game_real_out_card(pGame, pEvent, player, &out_card);

	return R_SUCC;
}



















const HeroConfig* get_hero_zhangfei()
{
	static HeroConfig  hero = {
		HeroID_ZhangFei,
		"张飞",
		4,
		NO,
		HeroGroup_Shu,
		HeroSex_Male,
		"",
		1,
		{
			{
				"咆哮",
				"出牌阶段，你可以使用任意数量的【杀】。",
				SkillFlag_Passive,
				paoxiao_check,
				paoxiao_use,
			},
		},
	};

	return &hero;
}


