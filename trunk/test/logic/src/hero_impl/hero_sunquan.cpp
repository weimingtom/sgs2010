#include "hero_impl.h"





static YESNO zhiheng_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT zhiheng_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}


static YESNO jiuyuan_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT jiuyuan_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}


const HeroConfig* get_hero_sunquan()
{
	static HeroConfig  hero = {
		HeroID_SunQuan,
		"孙权",
		4,
		YES,
		HeroGroup_Wu,
		HeroSex_Male,
		"",
		2,
		{
			{
				"制衡",
				"出牌阶段,你可以弃掉任意数量的牌,然后摸取等量的牌.每回合限用一次。",
				SkillFlag_None,
				zhiheng_check,
				zhiheng_use,
			},
			{
				"救援",
				"主公技，锁定技，其他吴势力角色在你濒死状态下对你使用【桃】时，你额外回复1点体力。",
				SkillFlag_Master|SkillFlag_Passive,
				jiuyuan_check,
				jiuyuan_use,
			},
		},

	};

	return &hero;
}


