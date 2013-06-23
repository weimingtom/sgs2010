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

static RESULT paoxiao_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	//reset attack count in this round
	//pGame->players[player].params[0] = 0;
	
	//printf("please out a card 'sha' or cancel skill:");
	RESULT ret;

	CardPattern pattern;
	INIT_CARDPATTERN_USE_ID(&pattern, CardID_Attack);

	ret = game_passive_out(pGame, pEvent, player,  PlayerCard_Hand, &pattern, 1, YES, "please out a card 'sha' or cancel the skill:");

	if(ret != R_SUCC)
		return ret;

	//return game_real_outcard(pGame, pEvent, player, )

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


