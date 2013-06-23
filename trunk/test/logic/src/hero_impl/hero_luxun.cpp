#include "hero_impl.h"





static YESNO qianxun_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT qianxun_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}

static YESNO lianying_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT lianying_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const HeroConfig* get_hero_luxun()
{
	static HeroConfig  hero = {
		HeroID_LuXun,
		"陆逊",
		3,
		NO,
		HeroGroup_Wu,
		HeroSex_Male,
		"",
		2,
		{
			{
				"谦逊",
				"锁定技，你不能成为【顺手牵羊】和【乐不思蜀】的目标。",
				SkillFlag_Passive,
				qianxun_check,
				qianxun_use,
			},
			{
				"连营",
				"每当你失去最后一张手牌时，可立即摸一张牌。",
				SkillFlag_None,
				lianying_check,
				lianying_use,
			},
		},
	};

	return &hero;
}


