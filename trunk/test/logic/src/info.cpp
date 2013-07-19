#include "config.h"
#include "game.h"
#include "info.h"
#include "comm.h"


static const char* game_status_str(Status s)
{
	switch(s)
	{
	case Status_None: return  "不在游戏中";
	case Status_NewGame: return  "开始新游戏";
	case Status_Round_Begin: return  "回合开始阶段";
	case Status_Round_Judge: return  "判定阶段";
	case Status_Round_Get: return  "摸牌阶段";
	case Status_Round_Out: return  "出牌阶段";
	case Status_Round_Discard: return  "弃牌阶段";
	case Status_Round_End: return  "回合结束阶段";
	default: return "";
	}
}


RESULT game_cur_info(GameContext* pGame, GameEventContext* pEvent)
{
	int n;
	char buf[128];
	Player* p;
	const HeroConfig*  pHero;
	int  idx = 1;

	if(get_game_status(pGame) == Status_None)
	{
		MSG_OUT("not in game!\n");
		return R_E_STATUS;
	}

	p = CUR_PLAYER(pGame);
	// round info
	MSG_OUT("Round [%d] Playe [%s], Phase [%s]\n", pGame->round_num, ROUND_PLAYER(pGame)->name, game_status_str(get_game_status(pGame)));
	// base info
	MSG_OUT("Current Player: %d, %s, %s, life: %d/%d\n", pGame->cur_player, player_id_str(p->id), p->name, p->cur_life, p->max_life);
	// hand cards
	MSG_OUT("Hand cards (%d):\n",  p->hand_card_num);
	for(n = 0; n < p->hand_card_num; n++)
	{
		//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
		MSG_OUT("  (%d) %s;\n", idx++, card_str(&p->hand_cards[n], buf, sizeof(buf)));
	}

	// equiped cards
	if(CARD_VALID(&p->equip_cards[EquipIdx_Weapon]))
		MSG_OUT("Weapon    : (%d) %s\n", idx++, card_str(&p->equip_cards[EquipIdx_Weapon], buf, sizeof(buf)) );
	if(CARD_VALID(&p->equip_cards[EquipIdx_Armor]))
		MSG_OUT("Armor     : (%d) %s\n", idx++, card_str(&p->equip_cards[EquipIdx_Armor], buf, sizeof(buf)) );
	if(CARD_VALID(&p->equip_cards[EquipIdx_HorseInc]))
		MSG_OUT("Horse(+1) : (%d) %s\n", idx++, card_str(&p->equip_cards[EquipIdx_HorseInc], buf, sizeof(buf)) );
	if(CARD_VALID(&p->equip_cards[EquipIdx_HorseDec]))
		MSG_OUT("Horse(-1) : (%d) %s\n", idx++, card_str(&p->equip_cards[EquipIdx_HorseDec], buf, sizeof(buf)) );

	// judgment cards
	MSG_OUT("Judgment cards (%d):\n",  p->judgment_card_num);
	for(n = 0; n < p->judgment_card_num; n++)
	{
		//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
		MSG_OUT("  (%d) %s;\n", idx++, card_str(&p->judgment_cards[n], buf, sizeof(buf)));
	}

	// skills

	pHero = get_hero_config(p->hero);

	if(pHero && pHero->skillNum > 0)
	{
		MSG_OUT("Hero Skills:\n");
		for(n = 0; n < pHero->skillNum; n++)
		{
			MSG_OUT(" skill (%d) %s: %s\n", n + 1, pHero->skills[n].name, pHero->skills[n].desc);
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

	MSG_OUT("  (%d) %s%s +%d %s, %s, life: %d/%d, hand cards: %d\n", player + 1,
		pGame->round_player == player ? "R":"-", pGame->cur_player == player  ? "C":"-",
		game_player_dis(pGame, pGame->cur_player, player),
		player_id_str( (player == pGame->cur_player || IS_PLAYER_SHOW(pPlayer) ) ? pPlayer->id : PlayerID_Unknown),
		pPlayer->name, pPlayer->cur_life, pPlayer->max_life, pPlayer->hand_card_num);

	// equiped cards
	if(CARD_VALID(&pPlayer->equip_cards[EquipIdx_Weapon]))
		MSG_OUT("    Weapon : %s\n",  card_str(&pPlayer->equip_cards[EquipIdx_Weapon], buf, sizeof(buf)) );
	if(CARD_VALID(&pPlayer->equip_cards[EquipIdx_Armor]))
		MSG_OUT("    Armor : %s\n",  card_str(&pPlayer->equip_cards[EquipIdx_Armor], buf, sizeof(buf)) );
	if(CARD_VALID(&pPlayer->equip_cards[EquipIdx_HorseInc]))
		MSG_OUT("    Horse(+1) : %s\n", card_str(&pPlayer->equip_cards[EquipIdx_HorseInc], buf, sizeof(buf)) );
	if(CARD_VALID(&pPlayer->equip_cards[EquipIdx_HorseDec]))
		MSG_OUT("    Horse(-1) : %s\n",  card_str(&pPlayer->equip_cards[EquipIdx_HorseDec], buf, sizeof(buf)) );

	// judgment cards
	//MSG_OUT("    Judgment cards (%d):\n",  pPlayer->judgment_card_num);
	for(n = 0; n < pPlayer->judgment_card_num; n++)
	{
		//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
		MSG_OUT("    Judgment cards: (%d) %s;\n", n+1, card_str(&pPlayer->judgment_cards[n], buf, sizeof(buf)));
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
	MSG_OUT("Round [%d] Playe [%s], Phase [%s]\n", pGame->round_num, ROUND_PLAYER(pGame)->name, game_status_str(get_game_status(pGame)));

	// all players
	MSG_OUT("(*) %d players (%d+%d+%d+%d): \n", pGame->player_count, 1, pGame->minster_count, pGame->spy_count, pGame->mutineer_count);

	for(k = 0; k < pGame->player_count; k++)
	{
		p = &pGame->players[k];

		if(IS_PLAYER_DEAD(p) )
			continue;

		MSG_OUT("  (%d) %s%s +%d %s, %s, life: %d/%d, hand cards: %d\n", k + 1,
			pGame->round_player == k ? "R":"-", pGame->cur_player == k ? "C":"-",
			game_player_dis(pGame, pGame->cur_player, k),
			player_id_str((k == pGame->cur_player || IS_PLAYER_SHOW(p) ) ? p->id : PlayerID_Unknown ),
			p->name, p->cur_life, p->max_life, p->hand_card_num);

		// equiped cards
		if(CARD_VALID(&p->equip_cards[EquipIdx_Weapon]))
			MSG_OUT("    Weapon : %s\n",  card_str_def(&p->equip_cards[EquipIdx_Weapon], buf, sizeof(buf), "None") );
		if(CARD_VALID(&p->equip_cards[EquipIdx_Armor]))
			MSG_OUT("    Armor : %s\n",  card_str_def(&p->equip_cards[EquipIdx_Armor], buf, sizeof(buf), "None") );
		if(CARD_VALID(&p->equip_cards[EquipIdx_HorseInc]))
			MSG_OUT("    Horse(+1) : %s\n", card_str_def(&p->equip_cards[EquipIdx_HorseInc], buf, sizeof(buf), "None") );
		if(CARD_VALID(&p->equip_cards[EquipIdx_HorseDec]))
			MSG_OUT("    Horse(-1) : %s\n",  card_str_def(&p->equip_cards[EquipIdx_HorseDec], buf, sizeof(buf), "None") );

		// judgment cards
		if( p->judgment_card_num > 0)
		{
			MSG_OUT("    Judgment cards (%d):\n",  p->judgment_card_num);
			for(n = 0; n < p->judgment_card_num; n++)
			{
				//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
				MSG_OUT("      (%d) %s;\n", n+1, card_str(&p->judgment_cards[n], buf, sizeof(buf)));
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

