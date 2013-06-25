// event.cpp 



#include "config.h"
#include "event.h"
#include "game.h"
#include "player.h"
#include "card.h"
#include "hero.h"






RESULT trigger_game_event(GameContext* pGame, GameEventContext* pEvent)
{
	int n;
	int m = pEvent->trigger;

	for(n = 0; n < pGame->nPlayerCount; n++)
	{
		if(!IS_PLAYER_DEAD(GAME_PLAYER(m)))
		{
			trigger_player_event(pGame, pEvent, m);
			if(pEvent->block == YES)
				break;
		}
		m = (m + 1) % pGame->nPlayerCount;
	}

	return R_SUCC;
}



RESULT trigger_player_event(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// check player skill
	Player* pPlayer;
	const HeroConfig* pHero;
	const HeroSkill* pSkill;
	const CardConfig* pCardConfig;
	int  n;
	YESNO ret;
	int   may_skills = 0;
	int   may_cards = 0;
	
	pPlayer = GAME_PLAYER(pGame, player);
	
	pHero = get_hero_config(pPlayer->hero);

	if(pHero != NULL)
	{
		for(n = 0; n < pHero->skillNum; n++)
		{
			pSkill = &pHero->skills[n];

			if(pSkill->check)
			{
				ret = (*pSkill->check)(pGame, pEvent, player);

				if(pEvent->block == YES)
				{
					return R_SUCC;
				}

				if(ret == YES)
					may_skills++;
			}
		}
	}
	
	
	// check player hand card
	for(n = 0; n < pPlayer->nHandCardNum; n++)
	{
		pCardConfig = get_card_config(pPlayer->stHandCards[n].id);
		if(pCardConfig && pCardConfig->check)
		{
			ret = (*pCardConfig->check)(pGame, pEvent, player, PlayerCard_Hand, &pPlayer->stHandCards[n]);

			if(ret == YES)
				may_cards++;
		}
	}


	// check player equip card
	// ...


	if(may_skills > 0 || may_cards > 0)
	{
		cmd_loop(pGame, pEvent, "please use skill or out card:");
	}

	return R_SUCC;
}

