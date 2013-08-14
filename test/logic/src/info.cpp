#include "config.h"
#include "game.h"
#include "info.h"
#include "comm.h"
#include "event.h"
#include "get.h"
#include "out.h"
#include "discard.h"
#include "select.h"
#include "life.h"
#include "equip.h"
#include "skill.h"


RESULT game_cur_info(GameContext* pGame, GameEventContext* pEvent)
{
	int n;
	char buf[128];
	int player;
	Player* p;
	//const HeroConfig*  pHero;
	int  idx = 1;
	int skill_num;
	int skill_flag;
	PosCard  pos_card;
	int cu;

	if(get_game_status(pGame) == Status_None)
	{
		MSG_OUT("not in game!\n");
		return R_E_STATUS;
	}

	p = CUR_PLAYER(pGame);
	player = pGame->cur_player;
	// round info
	MSG_OUT("回合[%d],【%s】,阶段【%s】\n", pGame->round_num, ROUND_PLAYER(pGame)->name, game_status_str(get_game_status(pGame)));
	// base info
	MSG_OUT("当前玩家: [%d]【%s】, %s, 体力: %d/%d\n", pGame->cur_player, p->name, player_id_str(p->id), p->cur_life, p->max_life);
	// hand cards
	MSG_OUT("手牌 (共%d张):\n",  p->hand_card_num);
	for(n = 0; n < p->hand_card_num; n++)
	{
		pos_card.card = p->hand_cards[n];
		pos_card.where = CardWhere_PlayerHand;
		pos_card.pos = n;

		if(pEvent->id == GameEvent_PassiveOutCard || pEvent->id == GameEvent_SupplyCard)
		{
			// 是否匹配
			if(CHECK_WHERE_PATTERN(pos_card.where, pEvent->pattern_out->pattern.where ) 
				&& R_SUCC == card_match(&pos_card.card, sizeof(PosCard), 1, pEvent->pattern_out->pattern.patterns, pEvent->pattern_out->pattern.num))
			{
				cu = 1;
			}
			else
			{
				cu = 0;
			}
		}
		else
		{
			cu = (YES == game_card_can_out(pGame, pEvent, player, &pos_card));
		}

		//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
		MSG_OUT("%s [%d] %s;\n", cu ? "*":" ", idx++, card_str(&p->hand_cards[n], buf, sizeof(buf)));
	}

	// equiped cards
	MSG_OUT("装备区:\n");
	for(n = 0; n < EquipIdx_Max; n++)
	{
		if(CARD_VALID(&p->equip_cards[n]))
		{
			pos_card.card = p->equip_cards[n];
			pos_card.where = CardWhere_PlayerEquip;
			pos_card.pos = n;

			if(pEvent->id == GameEvent_PassiveOutCard || pEvent->id == GameEvent_SupplyCard)
			{
				// 是否匹配
				if(CHECK_WHERE_PATTERN(pos_card.where, pEvent->pattern_out->pattern.where ) 
					&& card_match(&pos_card.card, sizeof(PosCard), 1, pEvent->pattern_out->pattern.patterns, pEvent->pattern_out->pattern.num))
				{
					cu = 1;
				}
				else
				{
					cu = 0;
				}
			}
			else
			{
			//	cu = call_card_can_out(pos_card.card.id, pGame, pEvent, pGame->cur_player, &pos_card);
				cu = 0;
			}
			MSG_OUT("%s [%d] %s: %s\n", cu ? "*":" ", idx++, equip_idx_str(n), card_str(&p->equip_cards[n], buf, sizeof(buf)) );
		}
	}

	// judgment cards
	if(p->judgment_card_num > 0)
	{
		MSG_OUT("判定区(共%d张):\n",  p->judgment_card_num);
		for(n = 0; n < p->judgment_card_num; n++)
		{
			//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
			MSG_OUT("  [%d] %s;\n", idx++, vcard_str(&p->judgment_cards[n], buf, sizeof(buf)));
		}
	}

	// skills

	skill_num = hero_skill_num(p->hero);


	for(n = 1; n <= skill_num; n++)
	{
		cu = (USE_MANUAL == call_hero_skill_can_use(p->hero, n, pGame, pEvent, player));
		skill_flag = hero_skill_flag(p->hero, n);
		MSG_OUT("%s 技能[%d]: 【%s】%s%s\n", cu ? "*":" ", n,  hero_skill_name(p->hero, n, buf, sizeof(buf)), 
			(skill_flag & SkillFlag_Master) ? ",主公技":"",  (skill_flag & SkillFlag_Passive) ? ",锁定技":"");
	
	}

	// equip skills
	for(n = 0l; n < EquipIdx_Max; n++)
	{
		if(CARD_VALID(&p->equip_cards[n]))
		{
			pos_card.card = p->equip_cards[n];
			pos_card.where = CardWhere_PlayerEquip;
			pos_card.pos = n;

			cu = (USE_MANUAL == game_card_can_use(pGame, pEvent, player, &pos_card));

			if(cu)
			{
				MSG_OUT("%s 技能[%c]: 【%s】,%s效果\n", cu ? "*":" ", "waid"[n],  card_name(pos_card.card.id, buf, sizeof(buf)), equip_idx_str(n));
			}

		}
	}

	return R_SUCC;
}

RESULT game_other_player_info(GameContext* pGame, GameEventContext* pEvent, int player)
{
	int n;
	char buf[128];
	Player* pPlayer;
	int f;

	if(get_game_status(pGame) == Status_None)
	{
		MSG_OUT("not in game!\n");
		return R_E_STATUS;
	}

	pPlayer = &pGame->players[player];

	MSG_OUT("  (%d) %s%s +%d 【%s】, %s, 体力: %d/%d, 手牌: %d\n", player + 1,
		pGame->round_player == player ? "R":"-", pGame->cur_player == player  ? "C":"-",
		game_player_dis(pGame, pGame->cur_player, player),
		pPlayer->name, player_id_str( (player == pGame->cur_player || IS_PLAYER_SHOW(pPlayer) ) ? pPlayer->id : PlayerID_Unknown),
		pPlayer->cur_life, pPlayer->max_life, pPlayer->hand_card_num);

	f = 0;
	// equiped cards
	for(n = 0; n < EquipIdx_Max; n++)
	{
		if(CARD_VALID(&pPlayer->equip_cards[n]))
		{
			if(f == 0)
			{
				MSG_OUT("装备区:\n");
				f = 1;
			}
			MSG_OUT("    %s: %s\n",  equip_idx_str(n), card_str(&pPlayer->equip_cards[n], buf, sizeof(buf)) );
		}
	}

	// judgment cards
	if(pPlayer->judgment_card_num > 0)
	{
		MSG_OUT("判定区 (%d):\n",  pPlayer->judgment_card_num);
		for(n = 0; n < pPlayer->judgment_card_num; n++)
		{
			//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
			MSG_OUT("    [%d] %s;\n", n+1, vcard_str(&pPlayer->judgment_cards[n], buf, sizeof(buf)));
		}

	}
	return R_SUCC;
}


RESULT game_global_info(GameContext* pGame, GameEventContext* pEvent)
{
	int n, k;
	char buf[128];
	Player* p;
	int f;

	if(get_game_status(pGame) == Status_None)
	{
		MSG_OUT("not in game!\n");
		return R_E_STATUS;
	}


	// round info
	MSG_OUT("回合[%d],【%s】,阶段【%s】\n", pGame->round_num, ROUND_PLAYER(pGame)->name, game_status_str(get_game_status(pGame)));

	// all players
	MSG_OUT("(*) %d players (%d+%d+%d+%d): \n", pGame->player_count, 1, pGame->minister_count, pGame->spy_count, pGame->mutineer_count);

	for(k = 0; k < pGame->player_count; k++)
	{
		p = &pGame->players[k];

		if(IS_PLAYER_DEAD(p) )
			continue;

		MSG_OUT("  (%d) %s%s +%d 【%s】, %s, 体力: %d/%d, 手牌: %d\n", k + 1,
			pGame->round_player == k ? "R":"-", pGame->cur_player == k ? "C":"-",
			game_player_dis(pGame, pGame->cur_player, k),
			p->name, player_id_str((k == pGame->cur_player || IS_PLAYER_SHOW(p) ) ? p->id : PlayerID_Unknown ),
			p->cur_life, p->max_life, p->hand_card_num);

		// equiped cards
		f = 0;
		for(n = 0; n < EquipIdx_Max; n++)
		{
			if(CARD_VALID(&p->equip_cards[n]))
			{
				if(f == 0)
				{
					MSG_OUT("    装备区:\n");
					f = 1;
				}
				MSG_OUT("      %s: %s\n",  equip_idx_str(n), card_str(&p->equip_cards[n], buf, sizeof(buf)) );
			}
		}

		// judgment cards
		if( p->judgment_card_num > 0)
		{
			MSG_OUT("    判定区 (%d):\n",  p->judgment_card_num);
			for(n = 0; n < p->judgment_card_num; n++)
			{
				//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
				MSG_OUT("      [%d] %s;\n", n+1, vcard_str(&p->judgment_cards[n], buf, sizeof(buf)));
			}
		}
	}
	MSG_OUT("(*) 摸牌牌堆: %d\n", pGame->get_card_stack.count);
	MSG_OUT("(*) 弃牌牌堆: %d\n", pGame->discard_card_stack.count);
	MSG_OUT("(*) 当前弃牌: %d\n", pGame->cur_discard_card_num);

	// [RoleName] Out Cards: [(card str) (card str)] from [RoleName] As (card str)
	// [RoleName] Passive Out: [(card str) (card str)] from [RoleName] As (card str) 
	return R_SUCC;
}


static void game_event_param__new_game_config(GameContext* pGame, GameEventContext* pEvent)
{
	if(NULL == pEvent->new_game_config)
	{
		MSG_OUT("    new_game_config=NULL;\n");
	}
	else
	{
		MSG_OUT("    new_game_config.players=%d;\n", pEvent->new_game_config->players);
		MSG_OUT("    new_game_config.masters=%d;\n", pEvent->new_game_config->masters);
		MSG_OUT("    new_game_config.ministers=%d;\n", pEvent->new_game_config->ministers);
		MSG_OUT("    new_game_config.spies=%d;\n", pEvent->new_game_config->spies);
		MSG_OUT("    new_game_config.mutineers=%d;\n", pEvent->new_game_config->mutineers);
	}
}

static void game_event_param__file_name(GameContext* pGame, GameEventContext* pEvent)
{
	if(NULL == pEvent->file_name)
	{
		MSG_OUT("    file_name=NULL;\n");
	}
	else
	{
		MSG_OUT("    file_name=\"%s\";\n", pEvent->file_name);
	}
}

// from game.cpp
extern const char* card_value_id_str(CardValue value);
extern const char* card_color_id_str(CardColor color);


static const char* card_where_id_str(CardWhere where)
{
	switch(where)
	{
	case CardWhere_None: return "CardWhere_None";
	case CardWhere_PlayerHand: return "CardWhere_PlayerHand";
	case CardWhere_PlayerEquip:return "CardWhere_PlayerEquip";
	case CardWhere_PlayerJudgment:return "CardWhere_PlayerJudgment";
	case CardWhere_GetStack:return "CardWhere_GetStack";
	case CardWhere_CurDiscardStack:return "CardWhere_CurDiscardStack";
	case CardWhere_DiscardStack:return "CardWhere_DiscardStack";
	default: return "-1 {CardWhere_Invalid}";
	}
}

static char* card_pattern_where_str(int where, char* buf, int buflen)
{
	int n = 0;
	if(where == PatternWhere_None) {
		strncpy(buf, "PatternWhere_None", buflen);
	} else {
		if(where  & PatternWhere_Hand) {
			n += snprintf(buf+n, buflen-n, "%s", "PatternWhere_Hand");
			where &= ~PatternWhere_Hand;
		}
		if(where  & PatternWhere_Equip) {
			n += snprintf(buf+n, buflen-n, "%s%s", n > 0 ? "|":"", "PatternWhere_Equip");
			where &= ~PatternWhere_Equip;
		}
		if(where  & PatternWhere_Judgment) {
			n += snprintf(buf+n, buflen-n, "%s%s",  n > 0 ? "|":"", "PatternWhere_Judgment");
			where &= ~PatternWhere_Judgment;
		}
		if(where != 0)
		{
			n += snprintf(buf+n, buflen-n, "%s0x%x", n > 0 ? "|":"", where);
		}
	}
	return buf;
}

static void p_out_card_pattern(const char* perffix, OutCardPattern* p)
{
	int n;
	char  buf[512];

	MSG_OUT("    %s.where=%s;\n", perffix, card_pattern_where_str(p->where, buf, sizeof(buf)));
	MSG_OUT("    %s.fixed=%s;\n", perffix, YESNO2STR(p->fixed));
	MSG_OUT("    %s.num=%d;\n", perffix, p->num);
	for(n = 0; n < p->num; n++)
	{
		MSG_OUT("    %s.patterns[%d].id=%d {%s};\n", perffix, n, p->patterns[n].id, card_sid(p->patterns[n].id, buf, sizeof(buf)));
		MSG_OUT("    %s.patterns[%d].color=%s;\n", perffix, n, card_color_id_str(p->patterns[n].color));
		MSG_OUT("    %s.patterns[%d].value_min=%s;\n", perffix, n, card_value_id_str(p->patterns[n].value_min));
		MSG_OUT("    %s.patterns[%d].value_max=%s;\n", perffix, n, card_value_id_str(p->patterns[n].value_max));
	}
	MSG_OUT("    %s.ud=\"%s\";\n", perffix, p->ud);

}

static void p_card(const char* perffix, Card* p)
{
	char  buf[512];
	MSG_OUT("    %s.id=%d {%s};\n", perffix, p->id, card_sid(p->id, buf, sizeof(buf)));
	MSG_OUT("    %s.color=%s;\n", perffix, card_color_id_str(p->color));
	MSG_OUT("    %s.value=%s;\n", perffix, card_value_id_str(p->value));
	MSG_OUT("    %s.flag=%s;\n", perffix, card_flag_str(p->flag));
}


static void p_pos_card(const char* perffix, PosCard* p)
{
	char s_per[128];

	snprintf(s_per, sizeof(s_per), "%s.card", perffix);
	p_card(s_per, &p->card);
	MSG_OUT("    %s.where=%s;\n", perffix, card_where_id_str(p->where));
	MSG_OUT("    %s.pos=%d;\n", perffix, p->pos);
}

#if 0
static void p_vcard(const char* perffix, VCard* p)
{
	int  n;
	char s_per[128];
	snprintf(s_per, sizeof(s_per), "%s.vcard", perffix);
	p_card(s_per, &p->vcard);
	MSG_OUT("    %s.rnum=%d;\n", perffix, p->rnum);
	for(n = 0; n < p->rnum; n++)
	{
		snprintf(s_per, sizeof(s_per), "%s.rcards[%d]", perffix, n);
		p_card(s_per, &p->rcards[n]);
	}
}

#endif


static void p_pos_vcard(const char* perffix, PosVCard* p)
{
	int n;
	char s_per[128];
	snprintf(s_per, sizeof(s_per), "%s.vcard", perffix);
	p_card(s_per, &p->vcard);

	MSG_OUT("    %s.list.num=%d;\n", perffix, p->list.num);
	for(n = 0; n < p->list.num; n++)
	{
		snprintf(s_per, sizeof(s_per), "%s.list.pcards[%d]", perffix, n);
		p_pos_card(s_per, &p->list.pcards[n]);
	}
}



static void p_out_card(const char* perffix, OutCard* p)
{
	int    n;
	char s_per[128];

	MSG_OUT("    %s.trigger=%d;\n", perffix, p->trigger);
	MSG_OUT("    %s.supply=%d;\n", perffix, p->supply);
	MSG_OUT("    %s.target_num=%d;\n", perffix, p->target_num);
	for(n = 0; n < p->target_num; n++)
	{
		MSG_OUT("    %s.targets[%d]=%d;\n", perffix, n, p->target_num);

	}

	snprintf(s_per, sizeof(s_per), "%s.vcard", perffix);
	p_card(s_per, &p->vcard);

	MSG_OUT("    %s.list.num=%d;\n", perffix, p->list.num);
	for(n = 0; n < p->list.num; n++)
	{
		snprintf(s_per, sizeof(s_per), "%s.list.pcards[%d]", perffix, n);
		p_pos_card(s_per, &p->list.pcards[n]);
	}

}


static void game_event_param__get_card(GameContext* pGame, GameEventContext* pEvent)
{
	if(NULL == pEvent->get_card)
	{
		MSG_OUT("    get_card=NULL;\n");
	}
	else
	{
		MSG_OUT("    get_card.num=%d;\n", pEvent->get_card->num);
		MSG_OUT("    get_card.force=%s;\n", YESNO2STR(pEvent->get_card->force));
		MSG_OUT("    get_card.alter_text=\"%s\";\n", pEvent->get_card->alter_text);
	}
}

static void game_event_param__attack_dis(GameContext* pGame, GameEventContext* pEvent)
{
	if(NULL == pEvent->attack_dis)
	{
		MSG_OUT("    attack_dis=NULL;\n");
	}
	else
	{
		p_card("attack_dis", &pEvent->attack_dis->card);
		MSG_OUT("    attack_dis.base=%d;\n", pEvent->attack_dis->base);
		MSG_OUT("    attack_dis.inc=%d;\n", pEvent->attack_dis->inc);
		MSG_OUT("    attack_dis.dis=%d;\n", pEvent->attack_dis->dis);
		MSG_OUT("    attack_dis.flag=%d;\n", pEvent->attack_dis->flag);
	}

}


static void game_event_param__before_pout(GameContext* pGame, GameEventContext* pEvent)
{
	if(NULL == pEvent->before_pout)
	{
		MSG_OUT("    before_pout=NULL;\n");
	}
	else
	{
		p_out_card_pattern("before_pout.pattern", &pEvent->before_pout->pattern);
		MSG_OUT("    before_pout.alter_text=\"%s\";\n", pEvent->before_pout->alter_text);
	}
}

static void game_event_param__card_pattern(GameContext* pGame, GameEventContext* pEvent)
{

	if(NULL == pEvent->card_pattern)
	{
		MSG_OUT("    card_pattern=NULL;\n");
	}
	else
	{
		p_out_card_pattern("card_pattern", pEvent->card_pattern);
	}
}

static void game_event_param__pattern_out(GameContext* pGame, GameEventContext* pEvent)
{

	if(NULL == pEvent->pattern_out)
	{
		MSG_OUT("    pattern_out=NULL;\n");
	}
	else
	{
		p_out_card_pattern("pattern_out.pattern", &pEvent->pattern_out->pattern);
		p_out_card("pattern_out.out", &pEvent->pattern_out->out);
		MSG_OUT("    pattern_out.alter_text=\"%s\";\n", pEvent->pattern_out->alter_text);
	}
	
}

static void game_event_param__out_card(GameContext* pGame, GameEventContext* pEvent)
{
	if(NULL == pEvent->out_card)
	{
		MSG_OUT("    out_card=NULL;\n");
	}
	else
	{
		p_out_card("out_card", pEvent->out_card);
	}
}

static void game_event_param__pos_card(GameContext* pGame, GameEventContext* pEvent)
{
	if(NULL == pEvent->pos_card)
	{
		MSG_OUT("    pos_card=NULL;\n");
	}
	else
	{
		p_pos_card("pos_card", pEvent->pos_card);
	}
}

static void game_event_param__pos_vcard(GameContext* pGame, GameEventContext* pEvent)
{
	if(NULL == pEvent->pos_vcard)
	{
		MSG_OUT("    pos_vcard=NULL;\n");
	}
	else
	{
		p_pos_vcard("pos_vcard", pEvent->pos_vcard);
	}
}


static void game_event_param__equip_card(GameContext* pGame, GameEventContext* pEvent)
{
	if(NULL == pEvent->equip_card)
	{
		MSG_OUT("    equip_card=NULL;\n");
	}
	else
	{
		MSG_OUT("    equip_card.equip_pos=%s;\n", equip_idx_str(pEvent->equip_card->equip_pos));
		MSG_OUT("    equip_card.supply=%d;\n", pEvent->equip_card->supply);
		p_pos_card("equip_card.card", &pEvent->equip_card->card);
	}
}

static void game_event_param__change_life(GameContext* pGame, GameEventContext* pEvent)
{
	if(NULL == pEvent->change_life)
	{
		MSG_OUT("    change_life=NULL;\n");
	}
	else
	{
		MSG_OUT("    change_life.delta=%d;\n", pEvent->change_life->delta);
		MSG_OUT("    change_life.after_life=%d;\n", pEvent->change_life->after_life);
		MSG_OUT("    change_life.src_player=%d;\n", pEvent->change_life->src_player);
		p_out_card("change_life.src_cards", &pEvent->change_life->src_cards);
		MSG_OUT("    change_life.src_skill=%d;\n", pEvent->change_life->src_skill);
	}

}

static void game_event_param__discard_pattern(GameContext* pGame, GameEventContext* pEvent)
{
	char  buf[512];
	if(NULL == pEvent->discard_pattern)
	{
		MSG_OUT("    discard_pattern=NULL;\n");
	}
	else
	{
		MSG_OUT("    discard_pattern.num=%d;\n", pEvent->discard_pattern->num);
		MSG_OUT("    discard_pattern.where=%s;\n", card_pattern_where_str(pEvent->discard_pattern->where, buf, sizeof(buf)));
		MSG_OUT("    discard_pattern.force=%s;\n", YESNO2STR(pEvent->discard_pattern->force));
		MSG_OUT("    discard_pattern.alter_text=\"%s\";\n", pEvent->discard_pattern->alter_text);
	}
}

static void game_event_param__select_target(GameContext* pGame, GameEventContext* pEvent)
{
	if(NULL == pEvent->select_target)
	{
		MSG_OUT("    select_target=NULL;\n");
	}
	else
	{
		p_card("select_target", &pEvent->select_target->card);
		MSG_OUT("    select_target.message=\"%s\";\n", pEvent->select_target->message);
	}
}

static const char* can_use_str(CANUSE can_use)
{
	static char temp[16];
	switch(can_use)
	{
	case USE_CANNOT: return "USE_CANNOT";
	case USE_MANUAL: return "USE_MANUAL";
	case USE_AUTO:   return "USE_AUTO";
	case USE_QUIET:  return "USE_QUIET";
	default: 
		snprintf(temp, sizeof(temp), "\? (%d)", can_use);
		return temp;
	}
}

static void game_event_param__card_canuse(GameContext* pGame, GameEventContext* pEvent)
{
	if(NULL == pEvent->card_canuse)
	{
		MSG_OUT("    card_canuse=NULL;\n");
	}
	else
	{
		p_pos_card("card_canuse", &pEvent->card_canuse->pos_card);
		MSG_OUT("    card_canuse.can_use=%s;\n", can_use_str(pEvent->card_canuse->can_use));
	}
}


static void game_event_param__skill_canuse(GameContext* pGame, GameEventContext* pEvent)
{
	char buf[128];
	if(NULL == pEvent->skill_canuse)
	{
		MSG_OUT("    skill_canuse=NULL;\n");
	}
	else
	{
		MSG_OUT("    skill_canuse.hero_id=%d {%s};\n", pEvent->skill_canuse->hero_id, hero_sid(pEvent->skill_canuse->hero_id, buf, sizeof(buf)));
		MSG_OUT("    skill_canuse.skill_index=%d;\n", pEvent->skill_canuse->skill_index);
		MSG_OUT("    skill_canuse.can_use=%s;\n", can_use_str(pEvent->skill_canuse->can_use));
	}
}


static void game_event_param(GameContext* pGame, GameEventContext* pEvent)
{
	switch(pEvent->id)
	{
	case GameEvent_NewGame:
		game_event_param__new_game_config(pGame, pEvent);
		break;
	case GameEvent_LoadGame:
		game_event_param__file_name(pGame, pEvent);
		break;
	case GameEvent_PerRoundGet:
	case GameEvent_RoundGetCard:
	case GameEvent_PostRoundGet:
	case GameEvent_PassiveGetCard:
		game_event_param__get_card(pGame, pEvent);
		break;
	case GameEvent_CalcAttackDis:
		game_event_param__attack_dis(pGame, pEvent);
		break;
	case GameEvent_BeforePassiveOut:
		game_event_param__before_pout(pGame, pEvent);
		break;
	case GameEvent_AfterPassiveOut:
		game_event_param__card_pattern(pGame, pEvent);
		break;
	case GameEvent_PerPassiveOutCard:
	case GameEvent_PassiveOutCard:
	case GameEvent_PostPassiveOutCard:
	case GameEvent_PerSupplyCard:
	case GameEvent_SupplyCard:
	case GameEvent_PostSupplyCard:
		game_event_param__pattern_out(pGame, pEvent);
		break;
	case GameEvent_PerOutCardPrepare:
	case GameEvent_OutCardPrepare:
	case GameEvent_PerOutCard:
	case GameEvent_BeforeOutCardEffect:
	case GameEvent_OutCard:
	case GameEvent_PerOutCardCalc:
	case GameEvent_OutCardCalc:
	case GameEvent_PostOutCardCalc:
	case GameEvent_PostOutCard:
		game_event_param__out_card(pGame, pEvent);
		break;
	case GameEvent_PerDecideCard:
	case GameEvent_PerDecideCardCalc:
	case GameEvent_PostDecideCard:
	case GameEvent_PostGetCard:
	case GameEvent_PerLostCard:
	case GameEvent_PostLostCard:
	case GameEvent_CheckCardCanOut:
		game_event_param__pos_card(pGame, pEvent);
		break;
	case GameEvent_RoundDiscardCard:
	case GameEvent_PassiveDiscardCard:
		game_event_param__discard_pattern(pGame, pEvent);
		break;
	case GameEvent_PerDiscardCard:
	case GameEvent_PostDiscardCard:
	case GameEvent_PerCardCalc:
	case GameEvent_CardCalc:
	case GameEvent_PostCardCalc:
	case GameEvent_FiniCardCalc:
		game_event_param__pos_vcard(pGame, pEvent);
		break;
	case GameEvent_PerEquipCard:
	case GameEvent_PostEquipCard:
		game_event_param__equip_card(pGame, pEvent);
		break;
	case GameEvent_PerChangeLife:
	case GameEvent_ChangeLife:
	case GameEvent_PostChangeLife:
		game_event_param__change_life(pGame, pEvent);
		break;
	case GameEvent_SelectTarget:
		game_event_param__select_target(pGame, pEvent);
		break;
	case GameEvent_CheckCardCanUse:
		game_event_param__card_canuse(pGame, pEvent);
		break;
	case GameEvent_CheckSkillCanUse:
		game_event_param__skill_canuse(pGame, pEvent);
		break;
	default:
		/* do nothing */
		break;
	}

}

RESULT game_event_info(GameContext* pGame, GameEventContext* pEvent, int detail)
{
	int deep;
	GameEventContext* pe;
	char   buf[128];

	pe = pEvent;

	for(deep = 0; pe != NULL; deep++)
	{
		get_event_str(pe->id, buf, sizeof(buf));
		Player* ptr = get_game_player(pGame, pe->trigger);
		Player* pta = get_game_player(pGame, pe->target);

		MSG_OUT("[%d] 事件【%s】, 触发者【%s】,目标【%s】;ud=[\"%s\"]\n", deep,buf, ptr ? ptr->name :"无",pta ? pta->name : "无",  pe->ud);
		if(detail)
		{
			game_event_param(pGame, pe);
		}
		
		pe = pe->parent_event;
	}
	return R_SUCC;

}


