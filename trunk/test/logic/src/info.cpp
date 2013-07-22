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
	MSG_OUT("回合【%d】,玩家【%s】,阶段【%s】\n", pGame->round_num, ROUND_PLAYER(pGame)->name, game_status_str(get_game_status(pGame)));
	// base info
	MSG_OUT("当前玩家: (%d) 【%s】, %s, 体力: %d/%d\n", pGame->cur_player, p->name, player_id_str(p->id), p->cur_life, p->max_life);
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
			MSG_OUT("%s :%s [%d] %s\n", equip_idx_str(n), cu ? "*":" ", idx++, card_str(&p->equip_cards[n], buf, sizeof(buf)) );
		}
	}

	// judgment cards
	if(p->judgment_card_num > 0)
	{
		MSG_OUT("判定区(共%d张):\n",  p->judgment_card_num);
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
		MSG_OUT("%s 技能[%d]： 【%s】%s%s\n", cu ? "*":" ", n,  hero_skill_name(p->hero, n, buf, sizeof(buf)), 
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

			cu = (USE_MANUAL == call_card_can_use(pos_card.card.id, pGame, pEvent, player, &pos_card));

			if(cu)
			{
				MSG_OUT("%s 技能[%c]： 装备效果 - 【%s】\n", cu ? "*":" ", "waid"[n],  card_name(pos_card.card.id, buf, sizeof(buf)));
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

	MSG_OUT("  (%d) %s%s +%d 【%s】, %s, 体力: %d/%d, 手牌: %d\n", player + 1,
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
		MSG_OUT("判定区 (%d):\n",  pPlayer->judgment_card_num);
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
	MSG_OUT("回合【%d】,玩家【%s】,阶段【%s】\n", pGame->round_num, ROUND_PLAYER(pGame)->name, game_status_str(get_game_status(pGame)));

	// all players
	MSG_OUT("(*) %d players (%d+%d+%d+%d): \n", pGame->player_count, 1, pGame->minster_count, pGame->spy_count, pGame->mutineer_count);

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
		for(n = 0; n < EquipIdx_Max; n++)
		{
			if(CARD_VALID(&p->equip_cards[n]))
				MSG_OUT("    %s : %s\n",  equip_idx_str(n), card_str(&p->equip_cards[n], buf, sizeof(buf)) );
		}

		// judgment cards
		if( p->judgment_card_num > 0)
		{
			MSG_OUT("    判定区 (%d):\n",  p->judgment_card_num);
			for(n = 0; n < p->judgment_card_num; n++)
			{
				//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
				MSG_OUT("      [%d] %s;\n", n+1, card_str(&p->judgment_cards[n], buf, sizeof(buf)));
			}
		}
	}
	MSG_OUT("(*) stack   cards: %d\n", pGame->get_card_stack.count);
	MSG_OUT("(*) discard cards: %d\n", pGame->discard_card_stack.count);
	MSG_OUT("(*) cur dis cards: %d\n", pGame->cur_discard_card_num);

	// [RoleName] Out Cards: [(card str) (card str)] from [RoleName] As (card str)
	// [RoleName] Passive Out: [(card str) (card str)] from [RoleName] As (card str) 
	return R_SUCC;
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

		MSG_OUT("[%d] 事件【%s】, 触发者【%s】,目标【%s】\n", deep,buf, ptr ? ptr->name :"无",pta ? pta->name : "无" );


		pe = pe->parent_event;
	}
	return R_SUCC;

}


