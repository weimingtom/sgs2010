#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"



static YESNO paoxiao_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int paoxiao_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
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


