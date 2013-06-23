#include "hero_impl.h"






static YESNO rende_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT rende_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}


static YESNO jijiang_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT jijiang_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}


const HeroConfig* get_hero_liube()
{
	static HeroConfig  hero = {
		HeroID_LiuBei,
		"刘备",
		4,
		YES,
		HeroGroup_Shu,
		HeroSex_Male,
		"",
		2,
		{
			{
				"仁德",
				"出牌阶段，你可以将任意数量的手牌以任意分配方式交给其他角色，若你给出的牌张数不少于两张时，你回复1点体力。"
				"\n★使用仁德技能分出的牌，对方无法拒绝。",
				SkillFlag_None,
				rende_check,
				rende_use,
			},
			{
				"激将",
				"主公技，当你需要使用（或打出）一张【杀】时，你可以发动激将。所有蜀势力角色按行动顺序依次选择是否打出一张【杀】“提供”给你（视为由你使用或打出），直到有一名角色或没有任何角色决定如此作时为止。",
				SkillFlag_Master,
				jijiang_check,
				jijiang_use,
			},
		},

	};

	return &hero;
}

