#include "hero_impl.h"






static YESNO longdan_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT longdan_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}


const HeroConfig* get_hero_zhaoyu()
{
	static HeroConfig  hero = {
		HeroID_ZhaoYun,
		"����",
		3,
		NO,
		HeroGroup_Shu,
		HeroSex_Male,
		"",
		1,
		{
			{
				"����",
				"����Խ������Ƶġ�ɱ����������������������ɱ��ʹ�û�����\n"
				"��ʹ������ʱ�����ı��Ƶ����(����)�����ã����ƵĻ�ɫ�͵������䡣",
				SkillFlag_None,
				longdan_check,
				longdan_use,
			},
		},
	};

	return &hero;
}


