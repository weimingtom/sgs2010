#include "config.h"
#include "game.h"
#include "info.h"
#include "comm.h"


static const char* game_status_str(Status s)
{
	switch(s)
	{
	case Status_None: return  "������Ϸ��";
	case Status_NewGame: return  "��ʼ����Ϸ";
	case Status_Round_Begin: return  "�غϿ�ʼ�׶�";
	case Status_Round_Judge: return  "�ж��׶�";
	case Status_Round_Get: return  "���ƽ׶�";
	case Status_Round_Out: return  "���ƽ׶�";
	case Status_Round_Discard: return  "���ƽ׶�";
	case Status_Round_End: return  "�غϽ����׶�";
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
	MSG_OUT("Round [%d] Playe [%s], Phase [%s]\n", pGame->nRoundNum, ROUND_PLAYER(pGame)->name, game_status_str(get_game_status(pGame)));
	// base info
	MSG_OUT("Current Player: %d, %s, %s, life: %d/%d\n", pGame->nCurPlayer, player_id_str(p->id), p->name, p->curLife, p->maxLife);
	// hand cards
	MSG_OUT("Hand cards (%d):\n",  p->nHandCardNum);
	for(n = 0; n < p->nHandCardNum; n++)
	{
		//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
		MSG_OUT("  (%d) %s;\n", idx++, card_str(&p->stHandCards[n], buf, sizeof(buf)));
	}

	// equiped cards
	if(CARD_VALID(&p->stEquipCard[EquipIdx_Weapon]))
		MSG_OUT("Weapon    : (%d) %s\n", idx++, card_str(&p->stEquipCard[EquipIdx_Weapon], buf, sizeof(buf)) );
	if(CARD_VALID(&p->stEquipCard[EquipIdx_Armor]))
		MSG_OUT("Armor     : (%d) %s\n", idx++, card_str(&p->stEquipCard[EquipIdx_Armor], buf, sizeof(buf)) );
	if(CARD_VALID(&p->stEquipCard[EquipIdx_HorseInc]))
		MSG_OUT("Horse(+1) : (%d) %s\n", idx++, card_str(&p->stEquipCard[EquipIdx_HorseInc], buf, sizeof(buf)) );
	if(CARD_VALID(&p->stEquipCard[EquipIdx_HorseDec]))
		MSG_OUT("Horse(-1) : (%d) %s\n", idx++, card_str(&p->stEquipCard[EquipIdx_HorseDec], buf, sizeof(buf)) );

	// judgment cards
	MSG_OUT("Judgment cards (%d):\n",  p->nJudgmentCardNum);
	for(n = 0; n < p->nJudgmentCardNum; n++)
	{
		//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
		MSG_OUT("  (%d) %s;\n", idx++, card_str(&p->stJudgmentCards[n], buf, sizeof(buf)));
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
		pGame->nRoundPlayer == player ? "R":"-", pGame->nCurPlayer == player  ? "C":"-",
		game_player_dis(pGame, pGame->nCurPlayer, player),
		player_id_str( (player == pGame->nCurPlayer || IS_PLAYER_SHOW(pPlayer) ) ? pPlayer->id : PlayerID_Unknown),
		pPlayer->name, pPlayer->curLife, pPlayer->maxLife, pPlayer->nHandCardNum);

	// equiped cards
	if(CARD_VALID(&pPlayer->stEquipCard[EquipIdx_Weapon]))
		MSG_OUT("    Weapon : %s\n",  card_str(&pPlayer->stEquipCard[EquipIdx_Weapon], buf, sizeof(buf)) );
	if(CARD_VALID(&pPlayer->stEquipCard[EquipIdx_Armor]))
		MSG_OUT("    Armor : %s\n",  card_str(&pPlayer->stEquipCard[EquipIdx_Armor], buf, sizeof(buf)) );
	if(CARD_VALID(&pPlayer->stEquipCard[EquipIdx_HorseInc]))
		MSG_OUT("    Horse(+1) : %s\n", card_str(&pPlayer->stEquipCard[EquipIdx_HorseInc], buf, sizeof(buf)) );
	if(CARD_VALID(&pPlayer->stEquipCard[EquipIdx_HorseDec]))
		MSG_OUT("    Horse(-1) : %s\n",  card_str(&pPlayer->stEquipCard[EquipIdx_HorseDec], buf, sizeof(buf)) );

	// judgment cards
	//MSG_OUT("    Judgment cards (%d):\n",  pPlayer->nJudgmentCardNum);
	for(n = 0; n < pPlayer->nJudgmentCardNum; n++)
	{
		//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
		MSG_OUT("    Judgment cards: (%d) %s;\n", n+1, card_str(&pPlayer->stJudgmentCards[n], buf, sizeof(buf)));
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
	MSG_OUT("Round [%d] Playe [%s], Phase [%s]\n", pGame->nRoundNum, ROUND_PLAYER(pGame)->name, game_status_str(get_game_status(pGame)));

	// all players
	MSG_OUT("(*) %d players (%d+%d+%d+%d): \n", pGame->nPlayerCount, 1, pGame->nMinsterCount, pGame->nSpyCount, pGame->nMutineerCount);

	for(k = 0; k < pGame->nPlayerCount; k++)
	{
		p = &pGame->players[k];

		if(IS_PLAYER_DEAD(p) )
			continue;

		MSG_OUT("  (%d) %s%s +%d %s, %s, life: %d/%d, hand cards: %d\n", k + 1,
			pGame->nRoundPlayer == k ? "R":"-", pGame->nCurPlayer == k ? "C":"-",
			game_player_dis(pGame, pGame->nCurPlayer, k),
			player_id_str((k == pGame->nCurPlayer || IS_PLAYER_SHOW(p) ) ? p->id : PlayerID_Unknown ),
			p->name, p->curLife, p->maxLife, p->nHandCardNum);

		// equiped cards
		MSG_OUT("    Weapon : %s\n",  card_str_def(&p->stEquipCard[EquipIdx_Weapon], buf, sizeof(buf), "None") );
		MSG_OUT("    Armor : %s\n",  card_str_def(&p->stEquipCard[EquipIdx_Armor], buf, sizeof(buf), "None") );
		MSG_OUT("    Horse(+1) : %s\n", card_str_def(&p->stEquipCard[EquipIdx_HorseInc], buf, sizeof(buf), "None") );
		MSG_OUT("    Horse(-1) : %s\n",  card_str_def(&p->stEquipCard[EquipIdx_HorseDec], buf, sizeof(buf), "None") );

		// judgment cards
		MSG_OUT("    Judgment cards (%d):\n",  p->nJudgmentCardNum);
		for(n = 0; n < p->nJudgmentCardNum; n++)
		{
			//if(n > 0 && n % 4 == 0) MSG_OUT("\n           ");
			MSG_OUT("      (%d) %s;\n", n+1, card_str(&p->stJudgmentCards[n], buf, sizeof(buf)));
		}
	}
	MSG_OUT("(*) stack   cards: %d\n", pGame->stGetCardStack.count);
	MSG_OUT("(*) discard cards: %d\n", pGame->stDiscardCardStack.count);
	MSG_OUT("(*) cur dis cards: %d\n", pGame->nCurDiscardCardNum);

	// [RoleName] Out Cards: [(card str) (card str)] from [RoleName] As (card str)
	// [RoleName] Passive Out: [(card str) (card str)] from [RoleName] As (card str) 
	return R_SUCC;
}

