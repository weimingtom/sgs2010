#include "hero_impl.h"






static YESNO wushen_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int wushen_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}


const HeroConfig* get_hero_guanyu()
{
	static HeroConfig  hero = {
		HeroID_GuanYu,
		"����",
		4,
		NO,
		HeroSex_Male,
		HeroGroup_Shu,
		"",
		1,
		{
			{
				"��ʥ",
				"����Խ��������һ�ź�ɫ�Ƶ���ɱ��ʹ�û�����\n"
				"����ͬʱ�õ���ǰװ���ĺ�ɫװ��Ч��ʱ�����ɰ�����װ���Ƶ���ɱ����ʹ�û�����\n"
				"��ʹ����ʥʱ�����ı��Ƶ����(����)�����ã����ƵĻ�ɫ�͵������䡣",
				SkillFlag_None,
				wushen_check,
				wushen_use,
			},
		},
	};

	return &hero;
}


