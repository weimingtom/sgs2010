#include "hero_impl.h"






static YESNO tuxi_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return NO;
}

static int tuxi_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}



const HeroConfig* get_hero_zhanliao()
{
	static HeroConfig  hero = {
		HeroID_ZhangLiao,
		"����",
		4,
		NO,
		HeroSex_Male,
		HeroGroup_Wei,
		"",
		1,
		{
			{
				"ͻϮ",
				"���ƽ׶Σ�����Է������ƣ�Ȼ�����������������һ������ɫ�����������ȡһ���ơ�"
				"\n�����ƽ׶Σ���һ������ͻϮ���Ͳ��ܴ��ƶѻ���ơ�"
				"\n��ֻʣһ��������ɫʱ�����ֻ��ѡ����һ����ɫ��"
				"\n������ʱ�����κ��˶�û�����ƣ���Ͳ��ܷ���ͻϮ��",
				SkillFlag_None,
				tuxi_check,
				tuxi_use,
			},
		},
	};

	return &hero;
}


