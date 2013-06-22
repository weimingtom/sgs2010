#include "hero_impl.h"





static YESNO paoxiao_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	if(pEvent == NULL)
	{
		if(pGame->nRoundPlayer == player && pGame->status == Status_Round_Out)
			return YES;
	}
	return NO;
}

static int paoxiao_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	//reset attack count in this round
	//pGame->players[player].params[0] = 0;
	
	//printf("please out a card 'sha' or cancel skill:");

	CardPattern pattern;
	INIT_CARDPATTERN_USE_ID(&pattern, CardID_Attack);


	return game_appoint_out(pGame, player,  PlayerCard_Hand, &pattern, 1, 1, "please out a card 'sha' or cancel the skill:");

	return 0;
}



















const HeroConfig* get_hero_zhangfei()
{
	static HeroConfig  hero = {
		HeroID_ZhangFei,
		"张飞",
		4,
		NO,
		HeroSex_Male,
		HeroGroup_Shu,
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


