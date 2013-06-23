#include "hero_impl.h"






static YESNO ganglie_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT ganglie_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const HeroConfig* get_hero_xiahoudun()
{
	static HeroConfig  hero = {
		HeroID_XiaHouDun,
		"夏侯惇",
		4,
		NO,
		HeroGroup_Wei,
		HeroSex_Male,
		"",
		1,
		{
			{
				"刚烈",
				"你每受到一次伤害，可进行一次判定：若结果不为红桃，则目标来源必须进行二选一：弃两张手牌或受到你对其造成的1点伤害。",
				SkillFlag_None,
				ganglie_check,
				ganglie_use,
			},
		},
	};

	return &hero;
}


