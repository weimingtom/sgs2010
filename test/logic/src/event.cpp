// event.cpp 



#include "config.h"
#include "event.h"
#include "game.h"
#include "player.h"
#include "card.h"
#include "hero.h"
#include "cmd.h"


// begin lua interface implements


// end lua interface implements



RESULT trigger_game_event(GameContext* pGame, GameEventContext* pEvent)
{
	int n;
	int m = pEvent->trigger;

	for(n = 0; n < pGame->player_count; n++)
	{
		if(!IS_PLAYER_DEAD(GAME_PLAYER(pGame, m)))
		{
			trigger_player_event(pGame, pEvent, m);
			if(pEvent->block == YES)
				break;
		}
		m = (m + 1) % pGame->player_count;
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
	PosCard    pos_card;
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
					if(pEvent->block == YES)
					{
						return R_SUCC;
					}
				}
				else if(ret == YES)
				{
					may_skills++;
				}
			}
		}
	}
	
	
	// check player hand card
	for(n = 0; n < pPlayer->hand_card_num; n++)
	{
		//pCardConfig = get_card_config(pPlayer->hand_cards[n].id);
		//if(pCardConfig && pCardConfig->check)
		//{
		//	ret = (*pCardConfig->check)(pGame, pEvent, player);
		//	if(ret == YES)
		//		may_cards++;
		//}
		//if(R_SUCC == get_player_card(pPlayer, CardWhere_PlayerHand, n, &pos_card.card))
		{
			pos_card.card = pPlayer->hand_cards[n];
			pos_card.where = CardWhere_PlayerHand;
			pos_card.pos = n;
			ret = call_card_can_out(pPlayer->hand_cards[n].id, pGame, pEvent, player, &pos_card);
			if(ret == YES)
				may_cards++;
		}
	}


	// check player equip card skills

	for(n = 0; n < EquipIdx_Max; n++)
	{
		if(CARD_VALID(&pPlayer->equip_cards[n]))
		{
			//pCardConfig = get_card_config(pPlayer->equip_cards[n].id);
			//if(pCardConfig && pCardConfig->out)
			//{
			//	// for this event to calc the equip effect
			//	(*pCardConfig->out)(pGame, pEvent, player);
			//}
			//if(R_SUCC == get_player_card(pPlayer, CardWhere_PlayerEquip, n, &pos_card.card))
			{
				pos_card.card = pPlayer->equip_cards[n];
				pos_card.where = CardWhere_PlayerEquip;
				pos_card.pos = n;
				ret = call_card_can_use(pos_card.card.id, pGame, pEvent, player, &pos_card);
				if(ret == YES)
					may_skills++;
			}
		}
	}

	if(may_skills > 0 || may_cards > 0)
	{
		set_game_cur_player(pGame, player);
		cmd_loop(pGame, pEvent, "please use skill or out card:");
	}

	return R_SUCC;
}

