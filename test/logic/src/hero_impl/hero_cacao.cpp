#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"



static YESNO jianxiong_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int jianxiong_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}


static YESNO hujia_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int hujia_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}


const HeroConfig* get_hero_cacao()
{
	static HeroConfig  hero = {
		HeroID_CaoCao,
		"�ܲ�",
		4,
		YES,
		HeroSex_Male,
		HeroGroup_Wei,
		"",
		2,
		{
			{
				"����",
				"�����������ö�������˺����ơ�",
				SkillFlag_None,
				jianxiong_check,
				jianxiong_use,
			},
			{
				"����",
				"��������������Ҫʹ�ã�������һ�š�����ʱ������Է������ݡ�����κ������ɫ���ж�˳������ѡ���Ƿ���һ�š��������ṩ�����㣨��Ϊ����ʹ�û�������ֱ����һ����ɫ��û���κν�ɫ���������ʱΪֹ��",
				SkillFlag_Master,
				hujia_check,
				hujia_use,
			},
		},

	};

	return &hero;
}


