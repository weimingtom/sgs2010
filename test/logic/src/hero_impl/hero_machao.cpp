#include "hero_impl.h"






static YESNO mashu_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int mashu_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}


static YESNO tieqi_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int tieqi_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}


const HeroConfig* get_hero_machao()
{
	static HeroConfig  hero = {
		HeroID_MaChao,
		"��",
		4,
		NO,
		HeroSex_Male,
		HeroGroup_Shu,
		"",
		2,
		{
			{
				"����",
				"�����������������������ɫ�ľ���ʱ��ʼ��-1"
				"\n��������Ч����װ��-1��ʱЧ��һ����������Ȼ����װ��һƥ-1��",
				SkillFlag_Passive,
				mashu_check,
				mashu_use,
			},
			{
				"����",
				"����ʹ�á�ɱ��ָ��һ����ɫΪĿ�������Խ����ж��������Ϊ��ɫ���ˡ�ɱ�����ɱ�����",
				SkillFlag_None,
				tieqi_check,
				tieqi_use,
			},
		},
	};

	return &hero;
}


