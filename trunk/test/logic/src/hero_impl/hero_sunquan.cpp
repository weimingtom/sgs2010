#include "hero_impl.h"





static YESNO zhiheng_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int zhiheng_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}


static YESNO jiuyuan_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int jiuyuan_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}


const HeroConfig* get_hero_sunquan()
{
	static HeroConfig  hero = {
		HeroID_SunQuan,
		"��Ȩ",
		4,
		YES,
		HeroSex_Male,
		HeroGroup_Wu,
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


