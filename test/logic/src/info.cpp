#include "config.h"
#include "game.h"
#include "info.h"
#include "cmd.h"
#include "event.h"
#include "get.h"
#include "out.h"
#include "discard.h"
#include "select.h"
#include "life.h"
#include "judge.h"
#include "decide.h"
#include "equip.h"
#include "skill.h"
#include "script.h"


RESULT game_cur_info(GameContext* pGame, GameEventContext* pEvent)
{
	int n;
	//char buf[128];
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
		MSG_OUT("%s [%d] %s;\n", cu ? "*":" ", idx++, get_card_str(&p->hand_cards[n]));
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
			MSG_OUT("%s [%d] %s: %s\n", cu ? "*":" ", idx++, equip_idx_str(n), get_card_str(&p->equip_cards[n]) );
		}
	}

	// judgment cards
	if(p->judgment_card_num > 0)
	{
		MSG_OUT("判定区(共%d张):\n",  p->judgment_card_num);
		for(n = 0; n < p->judgment_card_num; n++)
		{
			//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
			MSG_OUT("  [%d] %s;\n", idx++, get_vcard_str(&p->judgment_cards[n]));
		}
	}

	// skills

	if(p->hero != HeroID_None)
	{
		skill_num = hero_skill_num(p->hero);

		for(n = 1; n <= skill_num; n++)
		{
			cu = (USE_MANUAL == call_hero_skill_can_use(p->hero, n, pGame, pEvent, player));
			skill_flag = hero_skill_flag(p->hero, n);
			MSG_OUT("%s 技能[%d]: 【%s】%s%s\n", cu ? "*":" ", n,  get_hero_skill_name(p->hero, n), 
				(skill_flag & SkillFlag_Master) ? ",主公技":"",  (skill_flag & SkillFlag_Passive) ? ",锁定技":"");

		}
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
				MSG_OUT("%s 技能[%c]: 【%s】,%s效果\n", cu ? "*":" ", "waid"[n],  get_card_name(pos_card.card.id), equip_idx_str(n));
			}
		}
	}

	return R_SUCC;
}

RESULT game_other_player_info(GameContext* pGame, GameEventContext* pEvent, int player)
{
	int n;
	//char buf[128];
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
			MSG_OUT("    %s: %s\n",  equip_idx_str(n), get_card_str(&pPlayer->equip_cards[n]) );
		}
	}

	// judgment cards
	if(pPlayer->judgment_card_num > 0)
	{
		MSG_OUT("判定区 (%d):\n",  pPlayer->judgment_card_num);
		for(n = 0; n < pPlayer->judgment_card_num; n++)
		{
			//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
			MSG_OUT("    [%d] %s;\n", n+1, get_vcard_str(&pPlayer->judgment_cards[n]));
		}

	}
	return R_SUCC;
}


RESULT game_global_info(GameContext* pGame, GameEventContext* pEvent)
{
	int n, k;
	//char buf[128];
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
				MSG_OUT("      %s: %s\n",  equip_idx_str(n), get_card_str(&p->equip_cards[n]) );
			}
		}

		// judgment cards
		if( p->judgment_card_num > 0)
		{
			MSG_OUT("    判定区 (%d):\n",  p->judgment_card_num);
			for(n = 0; n < p->judgment_card_num; n++)
			{
				//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
				MSG_OUT("      [%d] %s;\n", n+1, get_vcard_str(&p->judgment_cards[n]));
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


RESULT game_stack_info(GameContext* pGame, GameEventContext* pEvent)
{
	int n;
	char  buffer[128];
	Card* pCard;
	MSG_OUT("当前弃牌: 共[%d]张\n", pGame->cur_discard_card_num);
	for(n = 0; n < pGame->cur_discard_card_num; n++)
	{
		pCard = &pGame->cur_discard_cards[n];
		MSG_OUT(" [%d] %s\n", n, card_str(pCard, buffer, sizeof(buffer)));
	}

	MSG_OUT("弃牌牌堆: 共[%d]张\n", pGame->discard_card_stack.count);		
	card_stack_dump(&pGame->discard_card_stack);

	MSG_OUT("摸牌牌堆: 共[%d]张\n", pGame->get_card_stack.count);		
	card_stack_dump(&pGame->get_card_stack);

	return R_SUCC;
}

RESULT game_card_info(GameContext* pGame, GameEventContext* pEvent, const char* sid)
{
	CardID maxid;
	CardID id;

	//char  sid[128];
	//char  name[128];
	//char  desc[1024];
	if(sid == NULL)
	{
		maxid = card_maxid();
		for(id = CardID_Begin; id <= maxid; id = (CardID)(id + 1))
		{
			//pCardCfg = get_card_config((CardID)id);

			//if(pCardCfg)
			//{
			//	MSG_OUT("(%d) %s, %s\n", pCardCfg->id, pCardCfg->name, card_type_str(pCardCfg->type));
			//}

			if(card_id_valid(id))
			{
				MSG_OUT("(%d) {%s}: %s, %s\n", id, get_card_sid(id),
					get_card_name(id), card_type_str(card_type(id)));
			}
		}
	}
	else
	{
		if(!card_sid_valid(sid))
		{
			MSG_OUT("没找到sid为\'%s\'的卡牌!\n", sid);
			return R_E_PARAM;
		}
		else
		{
			id = card_sid2id(sid);

			MSG_OUT("(%d) {%s}, %s, %s\n%s\n", id, get_card_sid(id),
				get_card_name(id), card_type_str(card_type(id)), 
				get_card_desc(id));
		}
	}
	return R_SUCC;
}

RESULT game_hero_info(tolua_notnull GameContext* pGame,tolua_notnull  GameEventContext* pEvent, const char* sid)
{
	HeroID maxid;
	HeroID id;
	int n;
	//char  sid[128];
	//char  name[128];
	//char  desc[1024];
	int  skill_num;
	int  skill_flag;

	if(sid == NULL)
	{
		maxid = hero_maxid();
		for(id = HeroID_Begin; id <= maxid; id = (HeroID)(id + 1))
		{
			if(hero_id_valid(id))
			{
				MSG_OUT("(%d) {%s}: 【%s】, %s, %s, life %d%s\n", id, get_hero_sid(id), 
					get_hero_name(id), hero_group_str(hero_group(id)), 
					hero_sex_str(hero_sex(id)), hero_life(id), (hero_master(id) == YES) ? ", 主公":"");
			}
		}
	}
	else
	{
		if(!hero_sid_valid(sid))
		{
			MSG_OUT("没找到sid为'%s'的武将!\n", sid);
			return R_E_PARAM;
		}
		else
		{
			id = hero_sid2id(sid);
			MSG_OUT("(%d) {%s}: 【%s】, %s, %s, life %d%s\n%s\n", id, get_hero_sid(id), 
				get_hero_name(id), hero_group_str(hero_group(id)), 
				hero_sex_str(hero_sex(id)), hero_life(id), (hero_master(id) == YES) ? ", 主公":"", 
				get_hero_desc(id));
			skill_num = hero_skill_num(id);
			for(n = 1; n <= skill_num; n++)
			{
				skill_flag = hero_skill_flag(id, n);
				MSG_OUT(" 技能[%d]： 【%s】%s%s\n", n,  get_hero_skill_name(id, n), 
					(skill_flag & SkillFlag_Master) ? ",主公技":"",  (skill_flag & SkillFlag_Passive) ? ",锁定技":"");
			}
		}
	}
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

/*
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
*/


static void p_card_pattern(const char* perffix, CardPattern* p)
{
	MSG_OUT("    %s.id=%d {%s};\n", perffix, p->id, get_card_sid(p->id));
	MSG_OUT("    %s.color=%s;\n", perffix, ENUM2STR(CardColor, p->color));
	MSG_OUT("    %s.value_min=%s;\n", perffix, ENUM2STR(CardValue, p->value_min));
	MSG_OUT("    %s.value_max=%s;\n", perffix, ENUM2STR(CardValue, p->value_max));
}


static void p_out_card_pattern(const char* perffix, OutCardPattern* p)
{
	int n;
	char  s_per[512];

	MSG_OUT("    %s.where=%s;\n", perffix, BITOR2STR(PatternWhere, p->where));
	MSG_OUT("    %s.fixed=%s;\n", perffix, YESNO2STR(p->fixed));
	MSG_OUT("    %s.num=%d;\n", perffix, p->num);
	for(n = 0; n < p->num; n++)
	{
		snprintf(s_per, sizeof(s_per), "%s.patterns[%d]", perffix, n);
		p_card_pattern(s_per, &p->patterns[n]);
	}
	MSG_OUT("    %s.ud=\"%s\";\n", perffix, p->ud);

}

static void p_card(const char* perffix, Card* p)
{
	//char  buf[512];
	MSG_OUT("    %s.id=%d {%s};\n", perffix, p->id, get_card_sid(p->id));
	MSG_OUT("    %s.color=%s;\n", perffix, ENUM2STR(CardColor, p->color));
	MSG_OUT("    %s.value=%s;\n", perffix, ENUM2STR(CardValue, p->value));
	MSG_OUT("    %s.flag=%s;\n", perffix, ENUM2STR(CardFlag, p->flag));
}


static void p_pos_card(const char* perffix, PosCard* p)
{
	char s_per[128];

	snprintf(s_per, sizeof(s_per), "%s.card", perffix);
	p_card(s_per, &p->card);
	MSG_OUT("    %s.where=%s;\n", perffix, ENUM2STR(CardWhere, p->where));
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

static const char*  player_name(GameContext* pGame, int player)
{
	Player* p = get_game_player(pGame, player);
	if(!p) return "";

	return p->name;
}

static void p_out_card(GameContext* pGame, const char* perffix, OutCard* p)
{
	int    n;
	char s_per[128];

	MSG_OUT("    %s.trigger=%d \"%s\";\n", perffix, p->trigger, player_name(pGame, p->trigger));
	MSG_OUT("    %s.supply=%d \"%s\";\n", perffix, p->supply, player_name(pGame, p->supply));
	MSG_OUT("    %s.target_num=%d;\n", perffix, p->target_num);
	for(n = 0; n < p->target_num; n++)
	{
		MSG_OUT("    %s.targets[%d]=%d \"%s\";\n", perffix, n, p->targets[n], player_name(pGame, p->targets[n]));

	}
	MSG_OUT("    %s.flag=%s;\n", perffix, BITOR2STR(OutCardFlag, p->flag));

	snprintf(s_per, sizeof(s_per), "%s.vcard", perffix);
	p_card(s_per, &p->vcard);

	MSG_OUT("    %s.list.num=%d;\n", perffix, p->list.num);
	for(n = 0; n < p->list.num; n++)
	{
		snprintf(s_per, sizeof(s_per), "%s.list.pcards[%d]", perffix, n);
		p_pos_card(s_per, &p->list.pcards[n]);
	}
	MSG_OUT("    %s.ud=\"%s\";\n", perffix, p->ud);

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
		p_out_card(pGame, "pattern_out.out", &pEvent->pattern_out->out);
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
		p_out_card(pGame, "out_card", pEvent->out_card);
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

static void game_event_param__decide_card(GameContext* pGame, GameEventContext* pEvent)
{
	if(NULL == pEvent->decide_card)
	{
		MSG_OUT("    decide_card=NULL;\n");
	}
	else
	{
		p_card_pattern("decide_card.pattern", &pEvent->decide_card->pattern);
		p_pos_card("decide_card.pos_card", &pEvent->decide_card->pos_card);
		MSG_OUT("    decide_card.result=%s", YESNO2STR(pEvent->decide_card->result));
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

static void game_event_param__judge_card(GameContext* pGame, GameEventContext* pEvent)
{
	if(NULL == pEvent->judge_card)
	{
		MSG_OUT("    judge_card=NULL;\n");
	}
	else
	{
		p_pos_vcard("judge_card.pos_vcard", pEvent->pos_vcard);
		MSG_OUT("    judge_card.judge_result=%d;\n", pEvent->judge_card->judge_result);
		MSG_OUT("    judge_card.ud=\"%s\";\n", pEvent->judge_card->ud);
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
		MSG_OUT("    equip_card.supply=%d \"%s\";\n", pEvent->equip_card->supply, player_name(pGame, pEvent->equip_card->supply));
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
		MSG_OUT("    change_life.src_player=%d \"%s\";\n", pEvent->change_life->src_player, player_name(pGame, pEvent->change_life->src_player));
		p_out_card(pGame, "change_life.src_cards", &pEvent->change_life->src_cards);
		MSG_OUT("    change_life.src_skill=%d;\n", pEvent->change_life->src_skill);
	}

}

static void game_event_param__discard_pattern(GameContext* pGame, GameEventContext* pEvent)
{
	//char  buf[512];
	if(NULL == pEvent->discard_pattern)
	{
		MSG_OUT("    discard_pattern=NULL;\n");
	}
	else
	{
		MSG_OUT("    discard_pattern.num=%d;\n", pEvent->discard_pattern->num);
		MSG_OUT("    discard_pattern.where=%s;\n", BITOR2STR(PatternWhere, pEvent->discard_pattern->where));
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

/*
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
*/


static void game_event_param__card_canuse(GameContext* pGame, GameEventContext* pEvent)
{
	if(NULL == pEvent->card_canuse)
	{
		MSG_OUT("    card_canuse=NULL;\n");
	}
	else
	{
		p_pos_card("card_canuse", &pEvent->card_canuse->pos_card);
		MSG_OUT("    card_canuse.can_use=%s;\n", ENUM2STR(USE, pEvent->card_canuse->can_use));
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
		MSG_OUT("    skill_canuse.can_use=%s;\n", ENUM2STR(USE, pEvent->skill_canuse->can_use));
	}
}


static void game_event_param(GameContext* pGame, GameEventContext* pEvent)
{
	if(game_event_get_new_game_config(NULL, pEvent))
		game_event_param__new_game_config(pGame, pEvent);
	if(game_event_get_file_name(NULL, pEvent))
		game_event_param__file_name(pGame, pEvent);
	if(game_event_get_get_card(NULL, pEvent))
		game_event_param__get_card(pGame, pEvent);
	if(game_event_get_attack_dis(NULL, pEvent))
		game_event_param__attack_dis(pGame, pEvent);
	if(game_event_get_before_pout(NULL, pEvent))
		game_event_param__before_pout(pGame, pEvent);
	if(game_event_get_card_pattern(NULL, pEvent))
		game_event_param__card_pattern(pGame, pEvent);
	if(game_event_get_pattern_out(NULL, pEvent))
		game_event_param__pattern_out(pGame, pEvent);
	if(game_event_get_out_card(NULL, pEvent))
		game_event_param__out_card(pGame, pEvent);
	if(game_event_get_decide_card(NULL, pEvent))
		game_event_param__decide_card(pGame, pEvent);
	if(game_event_get_pos_card(NULL, pEvent))
		game_event_param__pos_card(pGame, pEvent);
	if(game_event_get_discard_pattern(NULL, pEvent))
		game_event_param__discard_pattern(pGame, pEvent);
	if(game_event_get_pos_vcard(NULL, pEvent))
		game_event_param__pos_vcard(pGame, pEvent);
	if(game_event_get_judge_card(NULL, pEvent))
		game_event_param__judge_card(pGame, pEvent);
	if(game_event_get_equip_card(NULL, pEvent))
		game_event_param__equip_card(pGame, pEvent);
	if(game_event_get_change_life(NULL, pEvent))
		game_event_param__change_life(pGame, pEvent);
	if(game_event_get_select_target(NULL, pEvent))
		game_event_param__select_target(pGame, pEvent);
	if(game_event_get_card_canuse(NULL, pEvent))
		game_event_param__card_canuse(pGame, pEvent);
	if(game_event_get_skill_canuse(NULL, pEvent))
		game_event_param__skill_canuse(pGame, pEvent);

}

RESULT game_event_info(GameContext* pGame, GameEventContext* pEvent, int detail)
{
	int deep;
	GameEventContext* pe;
	char   buf[128];

	pe = pEvent;

	for(deep = 0; pe != NULL; deep++)
	{
		get_enumt_str("GameEvent", pe->id, 0,  buf, sizeof(buf));
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


