#include "config.h"
#include "game.h"
#include "info.h"
#include "comm.h"
#include "event.h"



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
	MSG_OUT("�غ�[%d],��ҡ�%s��,�׶Ρ�%s��\n", pGame->round_num, ROUND_PLAYER(pGame)->name, game_status_str(get_game_status(pGame)));
	// base info
	MSG_OUT("��ǰ���: [%d]��%s��, %s, ����: %d/%d\n", pGame->cur_player, p->name, player_id_str(p->id), p->cur_life, p->max_life);
	// hand cards
	MSG_OUT("���� (��%d��):\n",  p->hand_card_num);
	for(n = 0; n < p->hand_card_num; n++)
	{
		pos_card.card = p->hand_cards[n];
		pos_card.where = CardWhere_PlayerHand;
		pos_card.pos = n;

		if(pEvent->id == GameEvent_PassiveOutCard || pEvent->id == GameEvent_SupplyCard)
		{
			// �Ƿ�ƥ��
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
			cu = (YES == call_card_can_out(pos_card.card.id, pGame, pEvent, player, &pos_card));
		}

		//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
		MSG_OUT("%s [%d] %s;\n", cu ? "*":" ", idx++, card_str(&p->hand_cards[n], buf, sizeof(buf)));
	}

	// equiped cards
	for(n = 0; n < EquipIdx_Max; n++)
	{
		if(CARD_VALID(&p->equip_cards[n]))
		{
			pos_card.card = p->equip_cards[n];
			pos_card.where = CardWhere_PlayerEquip;
			pos_card.pos = n;

			if(pEvent->id == GameEvent_PassiveOutCard || pEvent->id == GameEvent_SupplyCard)
			{
				// �Ƿ�ƥ��
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
			MSG_OUT("%s :%s [%d] %s\n", equip_idx_str(n), cu ? "*":" ", idx++, card_str(&p->equip_cards[n], buf, sizeof(buf)) );
		}
	}

	// judgment cards
	if(p->judgment_card_num > 0)
	{
		MSG_OUT("�ж���(��%d��):\n",  p->judgment_card_num);
		for(n = 0; n < p->judgment_card_num; n++)
		{
			//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
			MSG_OUT("  [%d] %s;\n", idx++, card_str(&p->judgment_cards[n], buf, sizeof(buf)));
		}
	}

	// skills

	skill_num = hero_skill_num(p->hero);


	for(n = 1; n <= skill_num; n++)
	{
		cu = (USE_MANUAL == call_hero_skill_can_use(p->hero, n, pGame, pEvent, player));
		skill_flag = hero_skill_flag(p->hero, n);
		MSG_OUT("%s ����[%d]�� ��%s��%s%s\n", cu ? "*":" ", n,  hero_skill_name(p->hero, n, buf, sizeof(buf)), 
			(skill_flag & SkillFlag_Master) ? ",������":"",  (skill_flag & SkillFlag_Passive) ? ",������":"");
	
	}

	// equip skills
	for(n = 0l; n < EquipIdx_Max; n++)
	{
		if(CARD_VALID(&p->equip_cards[n]))
		{
			pos_card.card = p->equip_cards[n];
			pos_card.where = CardWhere_PlayerEquip;
			pos_card.pos = n;

			cu = (USE_MANUAL == call_card_can_use(pos_card.card.id, pGame, pEvent, player, &pos_card));

			if(cu)
			{
				MSG_OUT("%s ����[%c]�� ��%s��,%s����\n", cu ? "*":" ", "waid"[n],  card_name(pos_card.card.id, buf, sizeof(buf)), equip_idx_str(n));
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

	if(get_game_status(pGame) == Status_None)
	{
		MSG_OUT("not in game!\n");
		return R_E_STATUS;
	}

	pPlayer = &pGame->players[player];

	MSG_OUT("  (%d) %s%s +%d ��%s��, %s, ����: %d/%d, ����: %d\n", player + 1,
		pGame->round_player == player ? "R":"-", pGame->cur_player == player  ? "C":"-",
		game_player_dis(pGame, pGame->cur_player, player),
		pPlayer->name, player_id_str( (player == pGame->cur_player || IS_PLAYER_SHOW(pPlayer) ) ? pPlayer->id : PlayerID_Unknown),
		pPlayer->cur_life, pPlayer->max_life, pPlayer->hand_card_num);

	// equiped cards
	for(n = 0; n < EquipIdx_Max; n++)
	{
		if(CARD_VALID(&pPlayer->equip_cards[n]))
		{
			MSG_OUT("    %s : %s\n",  equip_idx_str(n), card_str(&pPlayer->equip_cards[n], buf, sizeof(buf)) );
		}
	}

	// judgment cards
	if(pPlayer->judgment_card_num > 0)
	{
		MSG_OUT("�ж��� (%d):\n",  pPlayer->judgment_card_num);
		for(n = 0; n < pPlayer->judgment_card_num; n++)
		{
			//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
			MSG_OUT("    [%d] %s;\n", n+1, card_str(&pPlayer->judgment_cards[n], buf, sizeof(buf)));
		}

	}
	return R_SUCC;
}


RESULT game_global_info(GameContext* pGame, GameEventContext* pEvent)
{
	int n, k;
	char buf[128];
	Player* p;


	if(get_game_status(pGame) == Status_None)
	{
		MSG_OUT("not in game!\n");
		return R_E_STATUS;
	}


	// round info
	MSG_OUT("�غϡ�%d��,��ҡ�%s��,�׶Ρ�%s��\n", pGame->round_num, ROUND_PLAYER(pGame)->name, game_status_str(get_game_status(pGame)));

	// all players
	MSG_OUT("(*) %d players (%d+%d+%d+%d): \n", pGame->player_count, 1, pGame->minster_count, pGame->spy_count, pGame->mutineer_count);

	for(k = 0; k < pGame->player_count; k++)
	{
		p = &pGame->players[k];

		if(IS_PLAYER_DEAD(p) )
			continue;

		MSG_OUT("  (%d) %s%s +%d ��%s��, %s, ����: %d/%d, ����: %d\n", k + 1,
			pGame->round_player == k ? "R":"-", pGame->cur_player == k ? "C":"-",
			game_player_dis(pGame, pGame->cur_player, k),
			p->name, player_id_str((k == pGame->cur_player || IS_PLAYER_SHOW(p) ) ? p->id : PlayerID_Unknown ),
			p->cur_life, p->max_life, p->hand_card_num);

		// equiped cards
		for(n = 0; n < EquipIdx_Max; n++)
		{
			if(CARD_VALID(&p->equip_cards[n]))
				MSG_OUT("    %s : %s\n",  equip_idx_str(n), card_str(&p->equip_cards[n], buf, sizeof(buf)) );
		}

		// judgment cards
		if( p->judgment_card_num > 0)
		{
			MSG_OUT("    �ж��� (%d):\n",  p->judgment_card_num);
			for(n = 0; n < p->judgment_card_num; n++)
			{
				//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
				MSG_OUT("      [%d] %s;\n", n+1, card_str(&p->judgment_cards[n], buf, sizeof(buf)));
			}
		}
	}
	MSG_OUT("(*) �����ƶ�: %d\n", pGame->get_card_stack.count);
	MSG_OUT("(*) �����ƶ�: %d\n", pGame->discard_card_stack.count);
	MSG_OUT("(*) ��ǰ����: %d\n", pGame->cur_discard_card_num);

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
		MSG_OUT("    new_game_config.minsters=%d;\n", pEvent->new_game_config->minsters);
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

static void game_event_param__get_card(GameContext* pGame, GameEventContext* pEvent)
{
	if(NULL == pEvent->get_card)
	{
		MSG_OUT("    get_card=NULL;\n");
	}
	else
	{
		MSG_OUT("    get_card.num=%d;\n", pEvent->get_card->num);
		MSG_OUT("    get_card.can_cancel=%d;\n", YESNO2STR(pEvent->get_card->can_cancel));
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
		MSG_OUT("    attack_dis.base=%d;\n", pEvent->attack_dis->base);
		MSG_OUT("    attack_dis.inc=%d;\n", pEvent->attack_dis->inc);
		MSG_OUT("    attack_dis.dis=%d;\n", pEvent->attack_dis->dis);
		MSG_OUT("    attack_dis.flag=%d;\n", pEvent->attack_dis->flag);
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


static void game_event_param__before_pout(GameContext* pGame, GameEventContext* pEvent)
{
	int   n;
	char  buf[512];
	if(NULL == pEvent->before_pout)
	{
		MSG_OUT("    before_pout=NULL;\n");
	}
	else
	{
		MSG_OUT("    before_pout.pattern.where=%s;\n", card_pattern_where_str(pEvent->before_pout->pattern.where, buf, sizeof(buf)));
		MSG_OUT("    before_pout.pattern.fixed=%s;\n", YESNO2STR(pEvent->before_pout->pattern.fixed));
		MSG_OUT("    before_pout.pattern.num=%d;\n", pEvent->before_pout->pattern.num);
		for(n = 0; n < pEvent->before_pout->pattern.num; n++)
		{
			MSG_OUT("    before_pout.pattern.patterns[%d].id=%d {%s};\n", n, pEvent->before_pout->pattern.patterns[n].id, 
				card_sid(pEvent->before_pout->pattern.patterns[n].id, buf, sizeof(buf)));
			MSG_OUT("    before_pout.pattern.patterns[%d].color=%s;\n", n, card_color_id_str(pEvent->before_pout->pattern.patterns[n].color));
			MSG_OUT("    before_pout.pattern.patterns[%d].value_min=%s;\n", n, card_value_id_str(pEvent->before_pout->pattern.patterns[n].value_min));
			MSG_OUT("    before_pout.pattern.patterns[%d].value_max=%s;\n", n, card_value_id_str(pEvent->before_pout->pattern.patterns[n].value_max));
		}
		MSG_OUT("    before_pout.alter_text=\"%s\";\n", pEvent->before_pout->alter_text);
	}
}

static void game_event_param__card_pattern(GameContext* pGame, GameEventContext* pEvent)
{
	int n;
	char  buf[512];
	if(NULL == pEvent->card_pattern)
	{
		MSG_OUT("    card_pattern=NULL;\n");
	}
	else
	{
		MSG_OUT("    card_pattern.where=%s;\n", card_pattern_where_str(pEvent->card_pattern->where, buf, sizeof(buf)));
		MSG_OUT("    card_pattern.fixed=%s;\n", YESNO2STR(pEvent->card_pattern->fixed));
		MSG_OUT("    card_pattern.num=%d;\n", pEvent->card_pattern->num);
		for(n = 0; n < pEvent->card_pattern->num; n++)
		{
			MSG_OUT("    card_pattern.patterns[%d].id=%d {%s};\n", n, pEvent->card_pattern->patterns[n].id, 
				card_sid(pEvent->card_pattern->patterns[n].id, buf, sizeof(buf)));
			MSG_OUT("    card_pattern.patterns[%d].color=%s;\n", n, card_color_id_str(pEvent->card_pattern->patterns[n].color));
			MSG_OUT("    card_pattern.patterns[%d].value_min=%s;\n", n, card_value_id_str(pEvent->card_pattern->patterns[n].value_min));
			MSG_OUT("    card_pattern.patterns[%d].value_max=%s;\n", n, card_value_id_str(pEvent->card_pattern->patterns[n].value_max));
		}
	}
}

static void game_event_param__pattern_out(GameContext* pGame, GameEventContext* pEvent)
{
	int   n;
	char  buf[512];
	if(NULL == pEvent->pattern_out)
	{
		MSG_OUT("    pattern_out=NULL;\n");
	}
	else
	{
		MSG_OUT("    pattern_out.pattern.where=%s;\n", card_pattern_where_str(pEvent->pattern_out->pattern.where, buf, sizeof(buf)));
		MSG_OUT("    pattern_out.pattern.fixed=%s;\n", YESNO2STR(pEvent->pattern_out->pattern.fixed));
		MSG_OUT("    pattern_out.pattern.num=%d;\n", pEvent->pattern_out->pattern.num);
		for(n = 0; n < pEvent->pattern_out->pattern.num; n++)
		{
			MSG_OUT("    pattern_out.pattern.patterns[%d].id=%d {%s};\n", n, pEvent->pattern_out->pattern.patterns[n].id, 
				card_sid(pEvent->pattern_out->pattern.patterns[n].id, buf, sizeof(buf)));
			MSG_OUT("    pattern_out.pattern.patterns[%d].color=%s;\n", n, card_color_id_str(pEvent->pattern_out->pattern.patterns[n].color));
			MSG_OUT("    pattern_out.pattern.patterns[%d].value_min=%s;\n", n, card_value_id_str(pEvent->pattern_out->pattern.patterns[n].value_min));
			MSG_OUT("    pattern_out.pattern.patterns[%d].value_max=%s;\n", n, card_value_id_str(pEvent->pattern_out->pattern.patterns[n].value_max));
		}
		MSG_OUT("    pattern_out.out.trigger=%d;\n", pEvent->pattern_out->out.trigger);
		MSG_OUT("    pattern_out.out.supply=%d;\n", pEvent->pattern_out->out.supply);
		MSG_OUT("    pattern_out.out.target=%d;\n", pEvent->pattern_out->out.target);
		MSG_OUT("    pattern_out.out.vcard.id=%d {%s};\n", pEvent->pattern_out->out.vcard.id, card_sid(pEvent->pattern_out->out.vcard.id, buf, sizeof(buf)));
		MSG_OUT("    pattern_out.out.vcard.color=%s;\n", card_color_id_str(pEvent->pattern_out->out.vcard.color));
		MSG_OUT("    pattern_out.out.vcard.value=%s;\n", card_value_id_str(pEvent->pattern_out->out.vcard.value));
		MSG_OUT("    pattern_out.out.vcard.flag=0x%x;\n", pEvent->pattern_out->out.vcard.flag);
		MSG_OUT("    pattern_out.out.list.num=%d;\n", pEvent->pattern_out->out.list.num);
		for(n = 0; n < pEvent->pattern_out->out.list.num; n++)
		{
			MSG_OUT("    pattern_out.out.list.pcards[%d].card.id=%d {%s};\n", n, pEvent->pattern_out->out.list.pcards[n].card.id, card_sid(pEvent->pattern_out->out.list.pcards[n].card.id, buf, sizeof(buf)));
			MSG_OUT("    pattern_out.out.list.pcards[%d].card.color=%s;\n", n, card_color_id_str(pEvent->pattern_out->out.list.pcards[n].card.color));
			MSG_OUT("    pattern_out.out.list.pcards[%d].card.value=%s;\n", n, card_value_id_str(pEvent->pattern_out->out.list.pcards[n].card.value));
			MSG_OUT("    pattern_out.out.list.pcards[%d].card.flag=0x%x;\n", n, pEvent->pattern_out->out.list.pcards[n].card.flag);
			MSG_OUT("    pattern_out.out.list.pcards[%d].where=%s;\n", n, card_where_id_str(pEvent->pattern_out->out.list.pcards[n].where));
			MSG_OUT("    pattern_out.out.list.pcards[%d].pos=%d;\n", n, pEvent->pattern_out->out.list.pcards[n].pos);
		}
	}
	
}

static void game_event_param__out_card(GameContext* pGame, GameEventContext* pEvent)
{
	int   n;
	char  buf[512];
	if(NULL == pEvent->out_card)
	{
		MSG_OUT("    out_card=NULL;\n");
	}
	else
	{
		MSG_OUT("    out_card.trigger=%d;\n", pEvent->out_card->trigger);
		MSG_OUT("    out_card.supply=%d;\n", pEvent->out_card->supply);
		MSG_OUT("    out_card.target=%d;\n", pEvent->out_card->target);
		MSG_OUT("    out_card.vcard.id=%d {%s};\n", pEvent->out_card->vcard.id, card_sid(pEvent->out_card->vcard.id, buf, sizeof(buf)));
		MSG_OUT("    out_card.vcard.color=%s;\n", card_color_id_str(pEvent->out_card->vcard.color));
		MSG_OUT("    out_card.vcard.value=%s;\n", card_value_id_str(pEvent->out_card->vcard.value));
		MSG_OUT("    out_card.vcard.flag=0x%x;\n", pEvent->out_card->vcard.flag);
		MSG_OUT("    out_card.list.num=%d;\n", pEvent->out_card->list.num);
		for(n = 0; n < pEvent->out_card->list.num; n++)
		{
			MSG_OUT("    out_card.list.pcards[%d].card.id=%d {%s};\n", n, pEvent->out_card->list.pcards[n].card.id, card_sid(pEvent->out_card->list.pcards[n].card.id, buf, sizeof(buf)));
			MSG_OUT("    out_card.list.pcards[%d].card.color=%s;\n", n, card_color_id_str(pEvent->out_card->list.pcards[n].card.color));
			MSG_OUT("    out_card.list.pcards[%d].card.value=%s;\n", n, card_value_id_str(pEvent->out_card->list.pcards[n].card.value));
			MSG_OUT("    out_card.list.pcards[%d].card.flag=0x%x;\n", n, pEvent->out_card->list.pcards[n].card.flag);
			MSG_OUT("    out_card.list.pcards[%d].where=%s;\n", n, card_where_id_str(pEvent->out_card->list.pcards[n].where));
			MSG_OUT("    out_card.list.pcards[%d].pos=%d;\n", n, pEvent->out_card->list.pcards[n].pos);
		}

	}
}

static void game_event_param__pos_card(GameContext* pGame, GameEventContext* pEvent)
{
	char  buf[512];
	if(NULL == pEvent->pos_card)
	{
		MSG_OUT("    pos_card=NULL;\n");
	}
	else
	{
		MSG_OUT("    pos_card.card.id=%d {%s};\n", pEvent->pos_card->card.id, card_sid(pEvent->pos_card->card.id, buf, sizeof(buf)));
		MSG_OUT("    pos_card.card.color=%s;\n", card_color_id_str(pEvent->pos_card->card.color));
		MSG_OUT("    pos_card.card.value=%s;\n", card_value_id_str(pEvent->pos_card->card.value));
		MSG_OUT("    pos_card.card.flag=0x%x;\n", pEvent->pos_card->card.flag);
		MSG_OUT("    pos_card.where=%s;\n", card_where_id_str(pEvent->pos_card->where));
		MSG_OUT("    pos_card.pos=%d;\n", pEvent->pos_card->pos);
	}
}

static void game_event_param__equip_card(GameContext* pGame, GameEventContext* pEvent)
{
	char  buf[512];
	if(NULL == pEvent->equip_card)
	{
		MSG_OUT("    equip_card=NULL;\n");
	}
	else
	{
		MSG_OUT("    equip_card.equip_pos=%s;\n", equip_idx_str(pEvent->equip_card->equip_pos));
		MSG_OUT("    equip_card.supply=%d;\n", pEvent->equip_card->supply);
		MSG_OUT("    equip_card.card.card.id=%d {%s};\n", pEvent->equip_card->card.card.id, card_sid(pEvent->equip_card->card.card.id, buf, sizeof(buf)));
		MSG_OUT("    equip_card.card.card.color=%s;\n", card_color_id_str(pEvent->equip_card->card.card.color));
		MSG_OUT("    equip_card.card.card.value=%s;\n", card_value_id_str(pEvent->equip_card->card.card.value));
		MSG_OUT("    equip_card.card.card.flag=0x%x;\n", pEvent->equip_card->card.card.flag);
		MSG_OUT("    equip_card.card.where=%s;\n", card_where_id_str(pEvent->equip_card->card.where));
		MSG_OUT("    equip_card.card.pos=%d;\n", pEvent->equip_card->card.pos);
	}
}

static void game_event_param__change_life(GameContext* pGame, GameEventContext* pEvent)
{
	int   n;
	char  buf[512];
	if(NULL == pEvent->change_life)
	{
		MSG_OUT("    change_life=NULL;\n");
	}
	else
	{
		MSG_OUT("    change_life.delta=%d;\n", pEvent->change_life->delta);
		MSG_OUT("    change_life.after_life=%d;\n", pEvent->change_life->after_life);
		MSG_OUT("    change_life.src_player=%d;\n", pEvent->change_life->src_player);
		MSG_OUT("    change_life.src_cards.trigger=%d;\n", pEvent->change_life->src_cards.trigger);
		MSG_OUT("    change_life.src_cards.supply=%d;\n", pEvent->change_life->src_cards.supply);
		MSG_OUT("    change_life.src_cards.target=%d;\n", pEvent->change_life->src_cards.target);
		MSG_OUT("    change_life.src_cards.vcard.id=%d {%s};\n", pEvent->change_life->src_cards.vcard.id, card_sid(pEvent->change_life->src_cards.vcard.id, buf, sizeof(buf)));
		MSG_OUT("    change_life.src_cards.vcard.color=%s;\n", card_color_id_str(pEvent->change_life->src_cards.vcard.color));
		MSG_OUT("    change_life.src_cards.vcard.value=%s;\n", card_value_id_str(pEvent->change_life->src_cards.vcard.value));
		MSG_OUT("    change_life.src_cards.vcard.flag=0x%x;\n", pEvent->change_life->src_cards.vcard.flag);
		MSG_OUT("    change_life.src_cards.list.num=%d;\n", pEvent->change_life->src_cards.list.num);
		for(n = 0; n < pEvent->change_life->src_cards.list.num; n++)
		{
			MSG_OUT("    change_life.src_cards.list.pcards[%d].card.id=%d {%s};\n", n, pEvent->change_life->src_cards.list.pcards[n].card.id, card_sid(pEvent->change_life->src_cards.list.pcards[n].card.id, buf, sizeof(buf)));
			MSG_OUT("    change_life.src_cards.list.pcards[%d].card.color=%s;\n", n, card_color_id_str(pEvent->change_life->src_cards.list.pcards[n].card.color));
			MSG_OUT("    change_life.src_cards.list.pcards[%d].card.value=%s;\n", n, card_value_id_str(pEvent->change_life->src_cards.list.pcards[n].card.value));
			MSG_OUT("    change_life.src_cards.list.pcards[%d].card.flag=0x%x;\n", n, pEvent->change_life->src_cards.list.pcards[n].card.flag);
			MSG_OUT("    change_life.src_cards.list.pcards[%d].where=%s;\n", n, card_where_id_str(pEvent->change_life->src_cards.list.pcards[n].where));
			MSG_OUT("    change_life.src_cards.list.pcards[%d].pos=%d;\n", n, pEvent->change_life->src_cards.list.pcards[n].pos);
		}
		MSG_OUT("    change_life.src_skill=%d;\n", pEvent->change_life->src_skill);
	}

}

static void game_event_param__discard_card(GameContext* pGame, GameEventContext* pEvent)
{
	char  buf[512];
	if(NULL == pEvent->change_life)
	{
		MSG_OUT("    discard_card=NULL;\n");
	}
	else
	{
		MSG_OUT("    discard_card.num=%d;\n", pEvent->discard_card->num);
		MSG_OUT("    discard_card.where=%s;\n", card_pattern_where_str(pEvent->discard_card->where, buf, sizeof(buf)));
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
	case GameEvent_OutCardPrepare:
	case GameEvent_PerOutCard:
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
	case GameEvent_PerDiscardCard:
	case GameEvent_PostDiscardCard:
	case GameEvent_PerCardCalc:
	case GameEvent_CardCalc:
	case GameEvent_PostCardCalc:
	case GameEvent_FiniCardCalc:
	case GameEvent_PostGetCard:
	case GameEvent_PerLostCard:
	case GameEvent_PostLostCard:
		game_event_param__pos_card(pGame, pEvent);
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
	case GameEvent_RoundDiscardCard:
	case GameEvent_PassiveDiscardCard:
		game_event_param__discard_card(pGame, pEvent);
		break;
	default:
		/* do nothing */
		break;
	}

}

RESULT game_event_info(GameContext* pGame, GameEventContext* pEvent)
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

		MSG_OUT("[%d] �¼���%s��, �����ߡ�%s��,Ŀ�꡾%s��\n", deep,buf, ptr ? ptr->name :"��",pta ? pta->name : "��" );
		game_event_param(pGame, pe);

		pe = pe->parent_event;
	}
	return R_SUCC;

}


