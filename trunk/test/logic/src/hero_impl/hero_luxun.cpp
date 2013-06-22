#include "hero_impl.h"





static YESNO qianxun_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int qianxun_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}

static YESNO lianying_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int lianying_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const HeroConfig* get_hero_luxun()
{
	static HeroConfig  hero = {
		HeroID_LuXun,
		"½ѷ",
		3,
		NO,
		HeroSex_Male,
		HeroGroup_Wu,
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


