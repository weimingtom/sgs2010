#include "hero_impl.h"






static YESNO jianxiong_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// be damaged and target is self and the source of damage is card. 
	if(pEvent->id == GameEvent_PostLostLife
		&& pEvent->target == player
		&& pEvent->card.id != CardID_None
		&& YES != is_player_handfull(&pGame->players[player]))
		return YES;
	return NO;
}

static RESULT jianxiong_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// damage and
	

	return R_SUCC;
}


static YESNO hujia_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	if(pEvent && pEvent->id == GameEvent_PassiveOutCard && pEvent->card.id == CardID_Defend)
		return YES;
	return NO;
}

static RESULT hujia_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	int n;
	int nextplayer;
	const HeroConfig* pHero;
	CardPattern   pattern;
	OutCard   out;

	for(n = 1; n < pGame->nPlayerCount; n++)
	{
		nextplayer = (player + 1) % pGame->nPlayerCount;

		pHero = get_hero_config(pGame->players[nextplayer].hero);

		if(pHero && pHero->group == HeroGroup_Wei)
		{
			INIT_CARDPATTERN_USE_ID(&pattern, CardID_Defend);
			if(R_SUCC == game_supply_card(pGame, pEvent, player, nextplayer, &pattern, &out) )
			{
				// out card instead mine 
				return game_real_outcard(pGame, pEvent, player, pEvent ? pEvent->target : 0, nextplayer, &out);
				// break;
			}
		}
	}

	return R_SUCC;
}


const HeroConfig* get_hero_cacao()
{
	static HeroConfig  hero = {
		HeroID_CaoCao,
		"曹操",
		4,
		YES,
		HeroGroup_Wei,
		HeroSex_Male,
		"",
		2,
		{
			{
				"奸雄",
				"你可以立即获得对你造成伤害的牌。",
				SkillFlag_None,
				jianxiong_check,
				jianxiong_use,
			},
			{
				"护驾",
				"主公技，当你需要使用（或打出）一张【闪】时，你可以发动护驾。所有魏势力角色按行动顺序依次选择是否打出一张【闪】“提供”给你（视为由你使用或打出），直到有一名角色或没有任何角色决定如此做时为止。",
				SkillFlag_Master,
				hujia_check,
				hujia_use,
			},
		},

	};

	return &hero;
}


