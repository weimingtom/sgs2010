#include "hero_impl.h"






static YESNO jianxiong_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// be damaged and target is self and the source of damage is card. 
	if(pEvent->id == GameEvent_PostChangeLife
		&& pEvent->trigger == player
		&& pEvent->change_life->delta < 0
		&& pEvent->change_life->src_cards.list.num > 0 // ��Ҫ�ж�ָ������Ƿ����
		&& YES != is_player_handfull(&pGame->players[player]))
		return YES;
	return NO;
}

static RESULT jianxiong_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// damage and
	if(pEvent->id == GameEvent_PostChangeLife)
	{
		//add_player_handle_card(GAME_PLAYER(pGame, player, &pEvent->change_life->src_cards.list))
		return R_SUCC;
	}

	return R_E_FAIL;
}


static YESNO hujia_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	int n;
	if(pEvent->id == GameEvent_PassiveOutCard && pEvent->trigger == player)
	{
		n = 0;
		for(n = 0; n < pEvent->pattern_out->pattern.num; n++)
		{
			if(pEvent->pattern_out->pattern.patterns[n].id == card_sid2id("shan"))
			{
				return YES;
			}
		}
	}
	return NO;
}

static RESULT hujia_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	int n;
	int nextplayer;
	const HeroConfig* pHero;
	//OutCardPattern   pattern;
	// OutCard          out;

	// the event must be GameEvent_PerPassiveOutCard

	nextplayer = player;

	for(n = 1; n < pGame->player_count; n++)
	{
		nextplayer = (nextplayer + 1) % pGame->player_count;

		pHero = get_hero_config(pGame->players[nextplayer].hero);

		if(pHero && pHero->group == HeroGroup_Wei)
		{
			//pattern.num = 1;
			//INIT_CARDPATTERN_USE_ID(&pattern.patterns[0], CardID_Defend);
			//pattern.where = PlayerCard_Hand;
			if(R_SUCC == game_supply_card(NULL, pGame, pEvent, player, nextplayer, "h:{defend}", NULL, &pEvent->pattern_out->out) )
			{
				// out card instead mine 
				return R_SUCC;
				// break;
			}
		}
	}

	return R_E_FAIL;
}


const HeroConfig* get_hero_cacao()
{
	static HeroConfig  hero = {
		HeroID_CaoCao,
		"�ܲ�",
		4,
		YES,
		HeroGroup_Wei,
		HeroSex_Male,
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


