#include "hero_impl.h"





static YESNO zhiheng_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT zhiheng_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}


static YESNO jiuyuan_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT jiuyuan_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}


const HeroConfig* get_hero_sunquan()
{
	static HeroConfig  hero = {
		HeroID_SunQuan,
		"��Ȩ",
		4,
		YES,
		HeroGroup_Wu,
		HeroSex_Male,
		"",
		2,
		{
			{
				"�ƺ�",
				"���ƽ׶�,���������������������,Ȼ����ȡ��������.ÿ�غ�����һ�Ρ�",
				SkillFlag_None,
				zhiheng_check,
				zhiheng_use,
			},
			{
				"��Ԯ",
				"����������������������������ɫ�������״̬�¶���ʹ�á��ҡ�ʱ�������ظ�1��������",
				SkillFlag_Master|SkillFlag_Passive,
				jiuyuan_check,
				jiuyuan_use,
			},
		},

	};

	return &hero;
}


