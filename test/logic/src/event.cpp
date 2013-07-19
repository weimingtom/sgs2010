// event.cpp 



#include "config.h"
#include "event.h"
#include "game.h"
#include "player.h"
#include "card.h"
#include "hero.h"
#include "cmd.h"


// begin lua interface implements

GameEvent get_event_id(GameEventContext* pEvent)
{
	return pEvent->id;
}

int get_event_trigger(GameEventContext* pEvent)
{
	return pEvent->trigger;
}

int get_event_target(GameEventContext* pEvent)
{
	return pEvent->target;
}

GameEventContext*  get_parent_event(GameEventContext* pEvent)
{
	return pEvent->parent_event;
}

RESULT get_event_result(GameEventContext* pEvent)
{
	return pEvent->result;
}


void set_event_result(GameEventContext* pEvent, RESULT result)
{
	pEvent->result = result;
}

YESNO  get_event_block(GameEventContext* pEvent)
{
	return pEvent->block;
}

void  set_event_block(GameEventContext* pEvent, YESNO  yesno)
{
	pEvent->block = yesno;
}

/*
Card* get_event_card(GameEventContext* pEvent)
{
	return pEvent->pCard;
}
*/

OutCard* get_event_out(GameEventContext* pEvent)
{
	return pEvent->pOut;
}
/*
void  set_event_out(GameEventContext* pEvent, OutCard* pOut)
{
	pEvent->pOut = pOut;
}
*/

PosCard* get_event_poscard(GameEventContext* pEvent)
{
	return pEvent->pPosCard;
}

EquipCard* get_event_equipcard(GameEventContext* pEvent)
{
	return pEvent->pEquipCard;
}

ChangeLife* get_event_changelife(GameEventContext* pEvent)
{
	return pEvent->pChangeLife;
}

AttackDis* get_event_attackdis(GameEventContext* pEvent)
{
	return pEvent->pAttackDis;
}


// end lua interface implements



RESULT trigger_game_event(GameContext* pGame, GameEventContext* pEvent)
{
	int n;
	int m = pEvent->trigger;

	for(n = 0; n < pGame->nPlayerCount; n++)
	{
		if(!IS_PLAYER_DEAD(GAME_PLAYER(pGame, m)))
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
	// const CardConfig* pCardConfig;
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

				if(ret == YES && pSkill->flag & SkillFlag_Passive)
				{
					(*pSkill->use)(pGame, pEvent, player);
				}
				if(ret == YES)
					may_skills++;

				if(pEvent->block == YES)
				{
					return R_SUCC;
				}
			}
		}
	}
	
	
	// check player hand card
	for(n = 0; n < pPlayer->nHandCardNum; n++)
	{
		//pCardConfig = get_card_config(pPlayer->stHandCards[n].id);
		//if(pCardConfig && pCardConfig->check)
		//{
		//	ret = (*pCardConfig->check)(pGame, pEvent, player);
		//	if(ret == YES)
		//		may_cards++;
		//}
		ret = card_check_call(pPlayer->stHandCards[n].id, pGame, pEvent, player);
		if(ret == YES)
			may_cards++;
	}


	// check player equip card skills

	for(n = 0; n < EquipIdx_Max; n++)
	{
		if(CARD_VALID(&pPlayer->stEquipCard[n]))
		{
			//pCardConfig = get_card_config(pPlayer->stEquipCard[n].id);
			//if(pCardConfig && pCardConfig->out)
			//{
			//	// for this event to calc the equip effect
			//	(*pCardConfig->out)(pGame, pEvent, player);
			//}
			if(R_SUCC == card_out_call(pPlayer->stEquipCard[n].id, pGame, pEvent, player))
				may_skills++;
		}
	}

	if(may_skills > 0 || may_cards > 0)
	{
		set_game_cur_player(pGame, player);
		cmd_loop(pGame, pEvent, "please use skill or out card:");
	}

	return R_SUCC;
}

