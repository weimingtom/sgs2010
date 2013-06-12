#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"



static YESNO ganglie_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int ganglie_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const HeroConfig* get_hero_xiahoudun()
{
	static HeroConfig  hero = {
		HeroID_XiaHouDun,
		"夏侯惇",
		4,
		NO,
		HeroSex_Male,
		HeroGroup_Wei,
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


