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
		"�ĺ",
		4,
		NO,
		HeroGroup_Wei,
		HeroSex_Male,
		"",
		1,
		{
			{
				"����",
				"��ÿ�ܵ�һ���˺����ɽ���һ���ж����������Ϊ���ң���Ŀ����Դ������ж�ѡһ�����������ƻ��ܵ��������ɵ�1���˺���",
				SkillFlag_None,
				ganglie_check,
				ganglie_use,
			},
		},
	};

	return &hero;
}


