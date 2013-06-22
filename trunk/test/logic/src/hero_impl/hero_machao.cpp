#include "hero_impl.h"






static YESNO mashu_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int mashu_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}


static YESNO tieqi_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int tieqi_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}


const HeroConfig* get_hero_machao()
{
	static HeroConfig  hero = {
		HeroID_MaChao,
		"马超",
		4,
		NO,
		HeroSex_Male,
		HeroGroup_Shu,
		"",
		2,
		{
			{
				"马术",
				"锁定技，当你计算与其他角色的距离时，始终-1"
				"\n★马术的效果与装备-1马时效果一样，但你仍然可以装备一匹-1马",
				SkillFlag_Passive,
				mashu_check,
				mashu_use,
			},
			{
				"铁骑",
				"当你使用【杀】指定一名角色为目标后，你可以进行判定，若结果为红色，此【杀】不可被闪避",
				SkillFlag_None,
				tieqi_check,
				tieqi_use,
			},
		},
	};

	return &hero;
}


