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

RESULT player_cmd_event(GameContext* pGame, GameEventContext* pEvent, int player, YESNO force, const char* alter_text)
{
	RESULT  ret;

	ret = R_SUCC;

	while(ret == R_SUCC)
	{
		set_game_cur_player(pGame, player);
		ret = cmd_loop(pGame, pEvent, force, alter_text);

		// 要检查玩家是否已经阵亡，如果是，则需要立刻结果它的命令输入，返回调用
		CHECK_PLAYER_DEAD_RET(pGame, player, R_DEF);
		// 这里需要检测R_CANCEL，因为角色可能放弃在此事件内的行动
		RET_CHECK_RET(ret, ret);
		EVENT_CHECK_BREAK_RET(pEvent, ret);

	}
	return R_SUCC;
}




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

	ret = R_DEF;

	for(n = 0; n < pGame->player_count; n++)
	{
		if(!IS_PLAYER_DEAD(get_game_player(pGame, m)))
		{
			ret = trigger_player_event(pGame, pEvent, m);
			(void)ret;
			EVENT_CHECK_BREAK(pEvent);
		}
		m = (m + 1) % pGame->player_count;
	}

	return R_DEF;
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
	
	// check player hand card
	for(n = 0; n < pPlayer->hand_card_num; n++)
	{
		pos_card.card = pPlayer->hand_cards[n];
		pos_card.where = CardWhere_PlayerHand;
		pos_card.pos = n;
		yesno = game_card_can_out(pGame, pEvent, player, &pos_card);
		if(yesno == YES)
			may_cards++;
	}


	// check player equip card skills

	for(n = 0; n < EquipIdx_Max; n++)
	{
		if(CARD_VALID(&pPlayer->equip_cards[n]))
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
					CHECK_PLAYER_DEAD_RET(pGame, player, R_DEF);
					EVENT_CHECK_BREAK_RET(pEvent, ret);
				}
			}
			else if(cu == USE_MANUAL)
			{
				may_skills++;
			}
		}
	}

	// check skill can use
	if(pPlayer->hero != HeroID_None)
	{
		skill_num = hero_skill_num(pPlayer->hero);

		for(n = 1; n <=  skill_num; n++)
		{
			cu = call_hero_skill_can_use(pPlayer->hero, n, pGame, pEvent, player);

			if(cu == USE_AUTO || cu == USE_QUIET)
			{
				if(auto_use)
				{
					if(cu == USE_AUTO) 
					{
						MSG_OUT("【%s】的武将技能【%s】被触发。\n", pPlayer->name, get_hero_skill_name(pPlayer->hero, n));
					}
					ret = call_hero_skill_event(pPlayer->hero, n, pGame, pEvent, player);
					use_cnt++;
					CHECK_PLAYER_DEAD_RET(pGame, player, R_DEF);
					EVENT_CHECK_BREAK_RET(pEvent, ret);
				}
			}
			else if(cu == USE_MANUAL)
			{
				may_skills++;
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

	CHECK_PLAYER_DEAD_RET(pGame, player, R_DEF);
	EVENT_CHECK_BREAK_RET(pEvent, ret);

	// auto use card and skills
	ret = check_player_event(pGame, pEvent, player, 1);

	CHECK_PLAYER_DEAD_RET(pGame, player, R_DEF);
	EVENT_CHECK_BREAK_RET(pEvent, ret);

	if(ret == R_DEF)
	{
		ret = check_player_event(pGame, pEvent, player, 0);
	}


	while(ret == R_SUCC)
	{
		set_game_cur_player(pGame, player);
		ret = cmd_loop(pGame, pEvent, NO, "请出一张牌或者发动技能:");

		// 要检查玩家是否已经阵亡，如果是，则需要立刻结果它的命令输入，返回调用
		CHECK_PLAYER_DEAD_RET(pGame, player, R_DEF);
		// 这里需要检测R_CANCEL，因为角色可能放弃在此事件内的行动
		RET_CHECK_RET(ret, ret);
		EVENT_CHECK_BREAK_RET(pEvent, ret);
		ret = check_player_event(pGame, pEvent, player, 0);
	}

	return R_DEF;
}



RESULT call_game_event(GameContext* pGame, GameEventContext* pEvent)
{
	int n;
	int m;
	RESULT  ret;
	
	m = pEvent->trigger;

	ret = R_DEF;

	for(n = 0; n < pGame->player_count; n++)
	{
		if(!IS_PLAYER_DEAD(get_game_player(pGame, m)))
		{
			ret = call_player_event(pGame, pEvent, m);
			EVENT_CHECK_BREAK(pEvent);
			(void)ret;
		}
		m = (m + 1) % pGame->player_count;
	}

	return R_DEF;
}

RESULT call_player_event(GameContext* pGame, GameEventContext* pEvent, int player)
{
	RESULT  ret;
	Player* pPlayer;
	int skill_num;
	int n;
	PosCard   pos_card;


	pPlayer = get_game_player(pGame, player);

	for(n = 0; n < EquipIdx_Max; n++)
	{
		if(CARD_VALID(&pPlayer->equip_cards[n]))
		{
			pos_card.card = pPlayer->equip_cards[n];
			pos_card.where = CardWhere_PlayerEquip;
			pos_card.pos = n;

			ret = call_card_event(pos_card.card.id, pGame, pEvent, player);
			CHECK_PLAYER_DEAD_RET(pGame, player, R_DEF);
			EVENT_CHECK_BREAK_RET(pEvent, ret);
		}
	}

	if(pPlayer->hero != HeroID_None)
	{
		skill_num = hero_skill_num(pPlayer->hero);

		for(n = 1; n <=  skill_num; n++)
		{
			ret = call_hero_skill_event(pPlayer->hero, n, pGame, pEvent, player);
			CHECK_PLAYER_DEAD_RET(pGame, player, R_DEF);
			EVENT_CHECK_BREAK_RET(pEvent, ret);
		}
	}

	return R_DEF;
}



////////////////////////////////////////////////////////////////////////////////
// lua api exports

const NewGameConfig* game_event_get_new_game_config(lua_State* L, tolua_notnull GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_NewGame:
		if(pEvent->new_game_config == NULL && L != NULL)
			luaL_error(L, "error unexpected a null point 'new_game_config' is got in event object!");
		return pEvent->new_game_config;
	default:
		if(L != NULL)
			luaL_error(L, "error unexpected event id for get 'new_game_config' in event object!");
		return NULL;
	}
}

const char* game_event_get_file_name(lua_State* L, tolua_notnull GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_LoadGame:
		if(pEvent->file_name == NULL && L != NULL)
			luaL_error(L, "error unexpected a null point 'file_name' is got in event object!");
		return pEvent->file_name;
	default:
		if(L != NULL)
			luaL_error(L, "error unexpected event id for get 'file_name' in event object!");
		return NULL;
	}
}


GetCard* game_event_get_get_card(lua_State* L, tolua_notnull GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_PerRoundGet:
	case GameEvent_RoundGetCard:
	case GameEvent_PostRoundGet:
	case GameEvent_PassiveGetCard:
		if(pEvent->get_card == NULL && L != NULL)
			luaL_error(L, "error unexpected a null point 'get_card' is got in event object!");
		return pEvent->get_card;
	default:
		if(L != NULL)
			luaL_error(L, "error unexpected event id for get 'get_card' in event object!");
		return NULL;
	}
}

AttackDis* game_event_get_attack_dis(lua_State* L, tolua_notnull GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_GetBaseAttackDis:
	case GameEvent_CalcAttackDis:
		if(pEvent->attack_dis == NULL && L != NULL)
			luaL_error(L, "error unexpected a null point 'attack_dis' is got in event object!");
		return pEvent->attack_dis;
	default:
		if(L != NULL)
			luaL_error(L, "error unexpected event id for get 'attack_dis' in event object!");
		return NULL;
	}
}

BeforePassiveOut* game_event_get_before_pout(lua_State* L, tolua_notnull GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_BeforePassiveOut:
		if(pEvent->before_pout == NULL && L != NULL)
			luaL_error(L, "error unexpected a null point 'before_pout' is got in event object!");
		return pEvent->before_pout;
	default:
		if(L != NULL)
			luaL_error(L, "error unexpected event id for get 'before_pout' in event object!");
		return NULL;
	}
}

OutCardPattern* game_event_get_card_pattern(lua_State* L, tolua_notnull GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_AfterPassiveOut:
		if(pEvent->card_pattern == NULL && L != NULL)
			luaL_error(L, "error unexpected a null point 'card_pattern' is got in event object!");
		return pEvent->card_pattern;
	default:
		if(L != NULL)
			luaL_error(L, "error unexpected event id for get 'card_pattern' in event object!");
		return NULL;
	}
}

PatternOut* game_event_get_pattern_out(lua_State* L, tolua_notnull GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_PerPassiveOutCard:
	case GameEvent_PassiveOutCard:
	case GameEvent_PostPassiveOutCard:
	case GameEvent_PerSupplyCard:
	case GameEvent_SupplyCard:
	case GameEvent_PostSupplyCard:
		if(pEvent->pattern_out == NULL && L != NULL)
			luaL_error(L, "error unexpected a null point 'pattern_out' is got in event object!");
		return pEvent->pattern_out;
	default:
		if(L != NULL)
			luaL_error(L, "error unexpected event id for get 'pattern_out' in event object!");
		return NULL;
	}
}

OutCard* game_event_get_out_card(lua_State* L, tolua_notnull GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_PerOutCardPrepare:
	case GameEvent_OutCardPrepare:
	case GameEvent_PerOutCard:
	case GameEvent_BeforeOutCardEffect:
	case GameEvent_OutCard:
	case GameEvent_PerOutCardCalc:
	case GameEvent_OutCardCalc:
	case GameEvent_PostOutCardCalc:
	case GameEvent_PostOutCard:
		if(pEvent->out_card == NULL && L != NULL)
			luaL_error(L, "error unexpected a null point 'out_card' is got in event object!");
		return pEvent->out_card;
	default:
		if(L != NULL)
			luaL_error(L, "error unexpected event id for get 'out_card' in event object!");
		return NULL;
	}
}

DiscardPattern* game_event_get_discard_pattern(lua_State* L, tolua_notnull GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_RoundDiscardCard:
	case GameEvent_PassiveDiscardCard:
		if(pEvent->discard_pattern == NULL && L != NULL)
			luaL_error(L, "error unexpected a null point 'discard_pattern' is got in event object!");
		return pEvent->discard_pattern;
	default:
		if(L != NULL)
			luaL_error(L, "error unexpected event id for get 'discard_pattern' in event object!");
		return NULL;
	}
}

PosCard* game_event_get_pos_card(lua_State* L, tolua_notnull GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_PostGetCard:
	case GameEvent_PerLostCard:
	case GameEvent_PostLostCard:
	case GameEvent_CheckCardCanOut:
		if(pEvent->pos_card == NULL && L != NULL)
			luaL_error(L, "error unexpected a null point 'pos_card' is got in event object!");
		return pEvent->pos_card;
	default:
		if(L != NULL)
			luaL_error(L, "error unexpected event id for get 'pos_card' in event object!");
		return NULL;
	}
}

PosVCard* game_event_get_pos_vcard(lua_State* L, tolua_notnull GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_PerDiscardCard:
	case GameEvent_PostDiscardCard:
		if(pEvent->pos_vcard == NULL && L != NULL)
			luaL_error(L, "error unexpected a null point 'pos_vcard' is got in event object!");
		return pEvent->pos_vcard;
	default:
		if(L != NULL)
			luaL_error(L, "error unexpected event id for get 'pos_vcard' in event object!");
		return NULL;
	}
}

JudgeCard* game_event_get_judge_card(lua_State* L, tolua_notnull GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_PerCardCalc:
	case GameEvent_CardCalc:
	case GameEvent_PostCardCalc:
	case GameEvent_FiniCardCalc:
		if(pEvent->judge_card == NULL && L != NULL)
			luaL_error(L, "error unexpected a null point 'pos_vcard' is got in event object!");
		return pEvent->judge_card;
	default:
		if(L != NULL)
			luaL_error(L, "error unexpected event id for get 'pos_vcard' in event object!");
		return NULL;
	}
}

DecideCard* game_event_get_decide_card(lua_State* L, tolua_notnull GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_PerDecideCard:
	case GameEvent_PerDecideCardCalc:
	case GameEvent_PostDecideCard:
		if(pEvent->decide_card == NULL && L != NULL)
			luaL_error(L, "error unexpected a null point 'decide_card' is got in event object!");
		return pEvent->decide_card;
	default:
		if(L != NULL)
			luaL_error(L, "error unexpected event id for get 'decide_card' in event object!");
		return NULL;
	}
}

EquipCard* game_event_get_equip_card(lua_State* L, tolua_notnull GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_PerEquipCard:
	case GameEvent_PostEquipCard:
		if(pEvent->equip_card == NULL && L != NULL)
			luaL_error(L, "error unexpected a null point 'equip_card' is got in event object!");
		return pEvent->equip_card;
	default:
		if(L != NULL)
			luaL_error(L, "error unexpected event id for get 'equip_card' in event object!");
		return NULL;
	}
}

ChangeLife* game_event_get_change_life(lua_State* L, tolua_notnull GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_PerChangeLife:
	case GameEvent_ChangeLife:
	case GameEvent_PostChangeLife:
		if(pEvent->change_life == NULL && L != NULL)
			luaL_error(L, "error unexpected a null point 'change_life' is got in event object!");
		return pEvent->change_life;
	default:
		if(L != NULL)
			luaL_error(L, "error unexpected event id for get 'change_life' in event object!");
		return NULL;
	}
}

SelectTarget* game_event_get_select_target(lua_State* L, tolua_notnull GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_SelectTarget:
		if(pEvent->select_target == NULL && L != NULL)
			luaL_error(L, "error unexpected a null point 'select_target' is got in event object!");
		return pEvent->select_target;
	default:
		if(L != NULL)
			luaL_error(L, "error unexpected event id for get 'select_target' in event object!");
		return NULL;
	}
}

SkillCanUse* game_event_get_skill_canuse(lua_State* L, tolua_notnull GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_CheckSkillCanUse:
		if(pEvent->skill_canuse == NULL && L != NULL)
			luaL_error(L, "error unexpected a null point 'skill_canuse' is got in event object!");
		return pEvent->skill_canuse;
	default:
		if(L != NULL)
			luaL_error(L, "error unexpected event id for get 'skill_canuse' in event object!");
		return NULL;
	}
}

CardCanUse* game_event_get_card_canuse(lua_State* L, tolua_notnull GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_CheckCardCanUse:
		if(pEvent->card_canuse == NULL && L != NULL)
			luaL_error(L, "error unexpected a null point 'card_canuse' is got in event object!");
		return pEvent->card_canuse;
	default:
		if(L != NULL)
			luaL_error(L, "error unexpected event id for get 'card_canuse' in event object!");
		return NULL;
	}
}








/////////////////////////////////////////////////////////////////////////////////