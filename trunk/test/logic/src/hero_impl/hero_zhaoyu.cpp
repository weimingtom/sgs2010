#include "hero_impl.h"






static YESNO longdan_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int longdan_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}


const HeroConfig* get_hero_zhaoyu()
{
	static HeroConfig  hero = {
		HeroID_ZhaoYun,
		"赵云",
		3,
		NO,
		HeroSex_Male,
		HeroGroup_Shu,
		"",
		1,
		{
			{
				"龙胆",
				"你可以将你手牌的【杀】当【闪】、【闪】当【杀】使用或打出。\n"
				"★使用龙胆时，仅改变牌的类别(名称)和作用，而牌的花色和点数不变。",
				SkillFlag_None,
				longdan_check,
				longdan_use,
			},
		},
	};

	return &hero;
}


