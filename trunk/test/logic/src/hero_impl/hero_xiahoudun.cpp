#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"



static YESNO ganglie_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int ganglie_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const HeroConfig* get_hero_xiahoudun()
{
	static HeroConfig  hero = {
		HeroID_XiaHouDun,
		"�ĺ",
		4,
		NO,
		HeroSex_Male,
		HeroGroup_Wei,
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


