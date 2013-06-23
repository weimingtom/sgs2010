#include "hero_impl.h"





static YESNO jieyan_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT jieyan_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}

static YESNO xiaoji_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT xiaoji_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const HeroConfig* get_hero_sunshangxiang()
{
	static HeroConfig  hero = {
		HeroID_SunShangXiang,
		"孙尚香",
		3,
		NO,
		HeroGroup_Wu,
		HeroSex_Female,
		"",
		2,
		{
			{
				"结姻",
				"出牌阶段，你可以弃两张手牌并选择一名受伤的男性角色：你和目标角色各回复1点体力。每回合限用一次。"
				"\n★使用结姻的条件是“有受伤的男性角色”，与你是否受伤无关。",
				SkillFlag_None,
				jieyan_check,
				jieyan_use,
			},
			{
				"枭姬",
				"当你失去一张装备区里的牌时，你可以立即摸两张牌。",
				SkillFlag_None,
				xiaoji_check,
				xiaoji_use,
			},
		},
	};

	return &hero;
}


