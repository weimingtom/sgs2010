#include "../config.h"
#include "../hero.h"
#include "../comm.h"
#include "../game.h"



static int supply_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int player, int id, OutCard* pOut)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_SupplyCard, trigger, player, pParentEvent);
	event.card.id = id;
	event.result = Result_None;

	trigger_game_event(pGame, &event);

	if(event.result == Result_Yes)
	{
		*pOut = event.out;
	}

	return event.result;
}





static int per_out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, OutCard* pOut)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PerOutCard, trigger, target, pParentEvent);
	event.out = *pOut;

	trigger_game_event(pGame, &event);

	return event.result;
}

static int post_out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, OutCard* pOut)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PostOutCard, trigger, target, pParentEvent);
	event.out = *pOut;

	trigger_game_event(pGame, &event);

	*pOut = event.out;

	return event.result;	
}

static int remove_out_card(GameContext* pGame, int supply, OutCard* pOut)
{
	int n;
	char buf[128];
	if(pOut->nrcard == 0)
	{
		if(0 != player_remove_card(&pGame->players[supply], pOut->vcard.pos))
		{
			printf("remove out card [%s]  from player [%d] pos [%d] failed ", card_str(&pOut->vcard.card, buf, sizeof(buf)), supply, pOut->vcard.pos);
			return -1;
		}
	}
	else
	{
		for(n = 0; n < pOut->nrcard; n++)
		{
			player_remove_card(&pGame->players[supply], pOut->rcards[n].pos);
			printf("remove out card [%s] from player [%d] failed ", card_str(&pOut->rcards[n].card, buf, sizeof(buf)), supply, pOut->rcards[n].pos);
			return -1;
		}
	}
	return 0;
}

static int add_out_stack(GameContext* pGame, OutCard* pOut)
{
	int n;
	char buf[128];
	if(pOut->nrcard > 0)
	{
		for(n = 0; n < pOut->nrcard; n++)
		{
			if(0 != card_stack_push(&pGame->cardOut, &pOut->rcards[n].card))
			{
				printf("add out card [%s] failed ", card_str(&pOut->rcards[n].card, buf, sizeof(buf)));
				return -1;
			}
		}
	}
	else if(pOut->vcard.card.id != CardID_None)
	{
		if(0 != card_stack_push(&pGame->cardOut, &pOut->vcard.card))
		{
			printf("add out card [%s] failed ", card_str(&pOut->vcard.card, buf, sizeof(buf)));
			return -1;
		}
	}
	return 0;
}

static int out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, int supply, OutCard* pOut)
{
	if(Result_Cancel == per_out_card(pGame, pParentEvent, trigger, target, pOut))
		return -1;

	remove_out_card(pGame, supply, pOut);

	post_out_card(pGame, pParentEvent, trigger, target, pOut);

	add_out_stack(pGame, pOut);

	return 0;
}


static YESNO jianxiong_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	if(pEvent && pEvent->id == GameEvent_PostLostLife)
		return YES;
	return NO;
}

static int jianxiong_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	return 0;
}


static YESNO hujia_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	if(pEvent && pEvent->id == GameEvent_PassiveOutCard && pEvent->card.id == CardID_Defend)
		return YES;
	return NO;
}

static int hujia_use(GameContext* pGame, GameEventContext* pEvent, int player)
{
	int n;
	int nextplayer;
	const HeroConfig* pHero;
	OutCard   out;

	for(n = 1; n < pGame->nPlayerCount; n++)
	{
		nextplayer = (player + 1) % pGame->nPlayerCount;

		pHero = get_hero_config(pGame->players[nextplayer].id);

		if(pHero && pHero->group == HeroGroup_Wei)
		{
			if(supply_card(pGame, pEvent, player, nextplayer, CardID_Defend, &out) == Result_Yes)
			{
				// out card instead mine 
				return out_card(pGame, pEvent, player, pEvent ? pEvent->target : 0, nextplayer, &out);
				// break;
			}
		}
	}

	return -1;
}


const HeroConfig* get_hero_cacao()
{
	static HeroConfig  hero = {
		HeroID_CaoCao,
		"曹操",
		4,
		YES,
		HeroSex_Male,
		HeroGroup_Wei,
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


