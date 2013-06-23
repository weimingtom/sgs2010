#include "hero_impl.h"





static YESNO qianxun_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT qianxun_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}

static YESNO lianying_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT lianying_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const HeroConfig* get_hero_luxun()
{
	static HeroConfig  hero = {
		HeroID_LuXun,
		"½ѷ",
		3,
		NO,
		HeroGroup_Wu,
		HeroSex_Male,
		"",
		2,
		{
			{
				"ǫѷ",
				"���������㲻�ܳ�Ϊ��˳��ǣ�򡿺͡��ֲ�˼�񡿵�Ŀ�ꡣ",
				SkillFlag_Passive,
				qianxun_check,
				qianxun_use,
			},
			{
				"��Ӫ",
				"ÿ����ʧȥ���һ������ʱ����������һ���ơ�",
				SkillFlag_None,
				lianying_check,
				lianying_use,
			},
		},
	};

	return &hero;
}


