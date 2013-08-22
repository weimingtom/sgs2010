// event.cpp 



#include "config.h"
#include "comm.h"
#include "event.h"
#include "game.h"
#include "player.h"
#include "card.h"
#include "hero.h"
#include "cmd.h"
#include "script.h"
#include "skill.h"
#include "out.h"


// begin lua interface implements


// end lua interface implements

static RESULT call_player_global_event(GameContext* pGame, GameEventContext* pEvent, int player)
{
	RESULT ret = R_DEF;
	lua_State* L = get_game_script();
	lua_getglobal(L, "call_player_event");	
	tolua_pushusertype(L, pGame, "GameContext");
	tolua_pushusertype(L, pEvent, "GameEventContext");
	tolua_pushnumber(L, player);
	//tolua_pushusertype(L, pos_card, "PosCard");
	lua_call(L, 3, 1);
	if(lua_isnumber(L, -1))
	{
		ret = (RESULT)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return ret;
}


RESULT trigger_game_event(GameContext* pGame, GameEventContext* pEvent)
{
	RESULT  ret;
	int n;
	int m = pEvent->trigger;

	for(n = 0; n < pGame->player_count; n++)
	{
		if(!IS_PLAYER_DEAD(get_game_player(pGame, m)))
		{
			ret = trigger_player_event(pGame, pEvent, m);
			RET_CHECK_BACK(ret);
		}
		m = (m + 1) % pGame->player_count;
	}

	return R_SUCC;
}


RESULT check_player_event(GameContext* pGame, GameEventContext* pEvent, int player, int auto_use)
{
	// check player skills
	Player* pPlayer;
	int     skill_num;
	//int     skill_flag;
	PosCard pos_card;
	int     n;
	YESNO   yesno;
	CANUSE  cu;
	RESULT  ret;
	int     may_skills = 0;
	int     may_cards = 0;
	int     use_cnt = 0;
	
	pPlayer = get_game_player(pGame, player);

	//pHero = get_hero_config(pPlayer->hero);
	if(pPlayer->hero != HeroID_None)
	{

		skill_num = hero_skill_num(pPlayer->hero);

		//if(pHero != NULL)
		if(skill_num > 0)
		{
			for(n = 1; n <=  skill_num; n++)
			{
				//pSkill = &pHero->skills[n];

				//if(pSkill->check)
				{
					//ret = (*pSkill->check)(pGame, pEvent, player);
					cu = call_hero_skill_can_use(pPlayer->hero, n, pGame, pEvent, player);
					//skill_flag = hero_skill_flag(pPlayer->hero, n);

					if(cu == USE_AUTO || cu == USE_QUIET)
					{
						if(auto_use)
						{
							if(cu == USE_AUTO) 
							{
								MSG_OUT("【%s】的武将技能【%s】被触发。\n", pPlayer->name, get_hero_skill_name(pPlayer->hero, n));
							}
							//(*pSkill->use)(pGame, pEvent, player);
							ret = call_hero_skill_event(pPlayer->hero, n, pGame, pEvent, player);
							use_cnt++;
							RET_CHECK_BACK(ret);
						}
					}
					else if(cu == USE_MANUAL)
					{
						may_skills++;
					}
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
			yesno = game_card_can_out(pGame, pEvent, player, &pos_card);
			if(yesno == YES)
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
				cu = game_card_can_use(pGame, pEvent, player, &pos_card);
				if(cu == USE_AUTO || cu == USE_QUIET)
				{
					if(auto_use)
					{
						if(cu == USE_AUTO)
						{
							MSG_OUT("【%s】的【%s】%s效果被触发。\n", pPlayer->name, get_card_name(pos_card.card.id), equip_idx_str(n));
						}
						set_player_card_flag(pPlayer, pos_card.where, pos_card.pos, CardFlag_InUse);
						ret = call_card_event(pos_card.card.id, pGame, pEvent, player);
						set_player_card_flag(pPlayer, pos_card.where, pos_card.pos, CardFlag_None);
						use_cnt++;
						RET_CHECK_BACK(ret);
					}
				}
				else if(cu == USE_MANUAL)
				{
					may_skills++;
				}
			}
		}
	}

	if(auto_use)
	{
		if(use_cnt > 0)
			return R_DEF;
	}

	if(may_cards > 0 || may_skills > 0)
	{
		return R_SUCC;
	}
	else
	{
		return R_BACK;
	}
}


RESULT trigger_player_event(GameContext* pGame, GameEventContext* pEvent, int player)
{
	RESULT ret;

	ret = call_player_global_event(pGame, pEvent, player);

	RET_CHECK_BACK(ret);

	// auto use card and skills
	ret = check_player_event(pGame, pEvent, player, 1);

	if(ret == R_BACK)
		return R_BACK;

	if(ret == R_DEF)
	{
		ret = check_player_event(pGame, pEvent, player, 0);
	}


	while(ret == R_SUCC)
	{
		set_game_cur_player(pGame, player);
		ret = cmd_loop(pGame, pEvent, NO, "请出一张牌或者发动技能:");
		RET_CHECK_BACK(ret);
		ret = check_player_event(pGame, pEvent, player, 0);
	}

	return R_SUCC;
}



RESULT call_game_event(GameContext* pGame, GameEventContext* pEvent)
{
	int n;
	int m;
	RESULT  ret;
	
	m = pEvent->trigger;

	for(n = 0; n < pGame->player_count; n++)
	{
		if(!IS_PLAYER_DEAD(get_game_player(pGame, m)))
		{
			ret = call_player_event(pGame, pEvent, m);
			RET_CHECK_BACK(ret);
		}
		m = (m + 1) % pGame->player_count;
	}

	return R_SUCC;
}

RESULT call_player_event(GameContext* pGame, GameEventContext* pEvent, int player)
{
	RESULT  ret;
	Player* pPlayer;
	int skill_num;
	int n;
	PosCard   pos_card;


	pPlayer = get_game_player(pGame, player);
	
	if(pPlayer->hero != HeroID_None)
	{
		skill_num = hero_skill_num(pPlayer->hero);

		for(n = 1; n <=  skill_num; n++)
		{
			ret = call_hero_skill_event(pPlayer->hero, n, pGame, pEvent, player);
			RET_CHECK_BACK(ret);
		}
	}


	for(n = 0; n < EquipIdx_Max; n++)
	{
		if(CARD_VALID(&pPlayer->equip_cards[n]))
		{
			pos_card.card = pPlayer->equip_cards[n];
			pos_card.where = CardWhere_PlayerEquip;
			pos_card.pos = n;

			ret = call_card_event(pos_card.card.id, pGame, pEvent, player);
			RET_CHECK_BACK(ret);
		}
	}
	return R_SUCC;
}

