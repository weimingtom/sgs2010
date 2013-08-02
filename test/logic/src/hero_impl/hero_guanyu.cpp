#include "hero_impl.h"






static YESNO wushen_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT wushen_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}


const HeroConfig* get_hero_guanyu()
{
	static HeroConfig  hero = {
		HeroID_GuanYu,
		"关羽",
		4,
		NO,
		HeroGroup_Shu,
		HeroSex_Male,
		"",
		1,
		{
			{
				"武圣",
				"你可以将你的任意一张红色牌当【杀】使用或打出。\n"
				"★若同时用到当前装备的红色装备效果时，不可把这张装备牌当【杀】来使用或打出。\n"
				"★使用武圣时，仅改变牌的类别(名称)和作用，而牌的花色和点数不变。",
				SkillFlag_None,
				wushen_check,
				wushen_use,
			},
		},
	};

	return &hero;
}


