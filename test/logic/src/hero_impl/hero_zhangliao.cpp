#include "hero_impl.h"






static YESNO tuxi_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT tuxi_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const HeroConfig* get_hero_zhanliao()
{
	static HeroConfig  hero = {
		HeroID_ZhangLiao,
		"张辽",
		4,
		NO,
		HeroGroup_Wei,
		HeroSex_Male,
		"",
		1,
		{
			{
				"突袭",
				"摸牌阶段，你可以放弃摸牌，然后从至多两名（至少一名）角色的手牌里各抽取一张牌。"
				"\n★摸牌阶段，你一旦发动突袭，就不能从牌堆获得牌。"
				"\n★只剩一名其他角色时，你就只能选择这一名角色。"
				"\n★若此时其他任何人都没有手牌，你就不能发动突袭。",
				SkillFlag_None,
				tuxi_check,
				tuxi_use,
			},
		},
	};

	return &hero;
}


