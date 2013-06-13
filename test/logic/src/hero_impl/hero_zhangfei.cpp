#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"



static YESNO paoxiao_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	if(pEvent == NULL)
	{
		if(pGame->nRoundPlayer == player && pGame->status == Status_Round_Out)
			return YES;
	}
	return NO;
}

static int paoxiao_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	//reset attack count in this round
	pGame->players[player].params[0] = 0;
	return 0;
}


const HeroConfig* get_hero_zhangfei()
{
	static HeroConfig  hero = {
		HeroID_ZhangFei,
		"�ŷ�",
		4,
		NO,
		HeroSex_Male,
		HeroGroup_Shu,
		"",
		1,
		{
			{
				"����",
				"���ƽ׶Σ������ʹ�����������ġ�ɱ����",
				SkillFlag_Passive,
				paoxiao_check,
				paoxiao_use,
			},
		},
	};

	return &hero;
}


