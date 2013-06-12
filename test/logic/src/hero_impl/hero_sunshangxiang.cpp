#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"



static YESNO jieyan_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int jieyan_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}

static YESNO xiaoji_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int xiaoji_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const HeroConfig* get_hero_sunshangxiang()
{
	static HeroConfig  hero = {
		HeroID_SunShangXiang,
		"������",
		3,
		NO,
		HeroSex_Female,
		HeroGroup_Wu,
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


