#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"



static YESNO jianxiong_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int jianxiong_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}


static YESNO hujia_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int hujia_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}


const HeroConfig* get_hero_cacao()
{
	static HeroConfig  hero = {
		HeroID_CaoCao,
		"曹操",
		4,
		YES,
		HeroSex_Male,
		HeroGroup_Wei,
		"",
		2,
		{
			{
				"奸雄",
				"你可以立即获得对你造成伤害的牌。",
				SkillFlag_None,
				jianxiong_check,
				jianxiong_use,
			},
			{
				"护驾",
				"主公技，当你需要使用（或打出）一张【闪】时，你可以发动护驾。所有魏势力角色按行动顺序依次选择是否打出一张【闪】“提供”给你（视为由你使用或打出），直到有一名角色或没有任何角色决定如此做时为止。",
				SkillFlag_Master,
				hujia_check,
				hujia_use,
			},
		},

	};

	return &hero;
}


