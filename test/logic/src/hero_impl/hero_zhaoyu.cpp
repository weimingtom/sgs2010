#include "hero_impl.h"






static YESNO longdan_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int longdan_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}


const HeroConfig* get_hero_zhaoyu()
{
	static HeroConfig  hero = {
		HeroID_ZhaoYun,
		"����",
		3,
		NO,
		HeroSex_Male,
		HeroGroup_Shu,
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


