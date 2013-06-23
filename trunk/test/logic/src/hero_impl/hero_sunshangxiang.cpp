#include "hero_impl.h"





static YESNO jieyan_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT jieyan_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}

static YESNO xiaoji_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT xiaoji_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}



const HeroConfig* get_hero_sunshangxiang()
{
	static HeroConfig  hero = {
		HeroID_SunShangXiang,
		"������",
		3,
		NO,
		HeroGroup_Wu,
		HeroSex_Female,
		"",
		2,
		{
			{
				"����",
				"���ƽ׶Σ���������������Ʋ�ѡ��һ�����˵����Խ�ɫ�����Ŀ���ɫ���ظ�1��������ÿ�غ�����һ�Ρ�"
				"\n��ʹ�ý����������ǡ������˵����Խ�ɫ���������Ƿ������޹ء�",
				SkillFlag_None,
				jieyan_check,
				jieyan_use,
			},
			{
				"�ɼ�",
				"����ʧȥһ��װ���������ʱ������������������ơ�",
				SkillFlag_None,
				xiaoji_check,
				xiaoji_use,
			},
		},
	};

	return &hero;
}


