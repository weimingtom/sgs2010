#include "hero_impl.h"






static YESNO rende_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT rende_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}


static YESNO jijiang_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static RESULT jijiang_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return R_SUCC;
}


const HeroConfig* get_hero_liube()
{
	static HeroConfig  hero = {
		HeroID_LiuBei,
		"����",
		4,
		YES,
		HeroGroup_Shu,
		HeroSex_Male,
		"",
		2,
		{
			{
				"�ʵ�",
				"���ƽ׶Σ�����Խ�����������������������䷽ʽ����������ɫ���������������������������ʱ����ظ�1��������"
				"\n��ʹ���ʵ¼��ֳܷ����ƣ��Է��޷��ܾ���",
				SkillFlag_None,
				rende_check,
				rende_use,
			},
			{
				"����",
				"��������������Ҫʹ�ã�������һ�š�ɱ��ʱ������Է���������������������ɫ���ж�˳������ѡ���Ƿ���һ�š�ɱ�����ṩ�����㣨��Ϊ����ʹ�û�������ֱ����һ����ɫ��û���κν�ɫ���������ʱΪֹ��",
				SkillFlag_Master,
				jijiang_check,
				jijiang_use,
			},
		},

	};

	return &hero;
}

