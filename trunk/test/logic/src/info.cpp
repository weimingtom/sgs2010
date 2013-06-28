#include "config.h"
#include "game.h"
#include "info.h"


RESULT game_cur_info(GameContext* pGame, GameEventContext* pEvent)
{
	int n;
	char buf[128];
	Player* p;
	const HeroConfig*  pHero;
	int  idx = 1;

	if(game_status(pGame) == Status_None)
	{
		printf("not in game!\n");
		return R_E_STATUS;
	}

	p = CUR_PLAYER(pGame);
	// base info
	printf("Current Player: %d, %s, %s, life: %d/%d\n", pGame->nCurPlayer, player_id_str(p->id), p->name, p->curLife, p->maxLife);
	// hand cards
	printf("Hand cards (%d):\n",  p->nHandCardNum);
	for(n = 0; n < p->nHandCardNum; n++)
	{
		//if(n > 0 && n % 4 == 0) printf("\n           ");
		printf("  (%d) %s;\n", idx++, card_str(&p->stHandCards[n], buf, sizeof(buf)));
	}

	// equiped cards
	if(p->stEquipCard[EquipIdx_Weapon].id != CardID_None)
		printf("Weapon    : (%d) %s\n", idx++, card_str(&p->stEquipCard[EquipIdx_Weapon], buf, sizeof(buf)) );
	if(p->stEquipCard[EquipIdx_Armor].id != CardID_None)
		printf("Armor     : (%d) %s\n", idx++, card_str(&p->stEquipCard[EquipIdx_Armor], buf, sizeof(buf)) );
	if(p->stEquipCard[EquipIdx_HorseInc].id != CardID_None)
		printf("Horse(+1) : (%d) %s\n", idx++, card_str(&p->stEquipCard[EquipIdx_HorseInc], buf, sizeof(buf)) );
	if(p->stEquipCard[EquipIdx_HorseDec].id != CardID_None)
		printf("Horse(-1) : (%d) %s\n", idx++, card_str(&p->stEquipCard[EquipIdx_HorseDec], buf, sizeof(buf)) );

	// judgment cards
	printf("Judgment cards (%d):\n",  p->nJudgmentCardNum);
	for(n = 0; n < p->nJudgmentCardNum; n++)
	{
		//if(n > 0 && n % 4 == 0) printf("\n           ");
		printf("  (%d) %s;\n", idx++, card_str(&p->stJudgmentCards[n], buf, sizeof(buf)));
	}

	// skills

	pHero = get_hero_config(p->hero);

	if(pHero && pHero->skillNum > 0)
	{
		printf("Hero Skills:\n");
		for(n = 0; n < pHero->skillNum; n++)
		{
			printf(" skill (%d) %s: %s\n", n + 1, pHero->skills[n].name, pHero->skills[n].desc);
		}
	}

	return R_SUCC;
}

RESULT game_other_player_info(GameContext* pGame, GameEventContext* pEvent, int player)
{
	int n;
	char buf[128];
	Player* pPlayer;

	if(game_status(pGame) == Status_None)
	{
		printf("not in game!\n");
		return R_E_STATUS;
	}

	pPlayer = &pGame->players[player];

	printf("  (%d) %s%s +%d %s, %s, life: %d/%d, hand cards: %d\n", player + 1,
		pGame->nRoundPlayer == player ? "R":"-", pGame->nCurPlayer == player  ? "C":"-",
		game_player_dis(pGame, pGame->nCurPlayer, player),
		player_id_str( (player == pGame->nCurPlayer || IS_PLAYER_SHOW(pPlayer) ) ? pPlayer->id : PlayerID_Unknown),
		pPlayer->name, pPlayer->curLife, pPlayer->maxLife, pPlayer->nHandCardNum);

	// equiped cards
	printf("    Weapon : %s\n",  card_str_def(&pPlayer->stEquipCard[EquipIdx_Weapon], buf, sizeof(buf), "None") );
	printf("    Armor : %s\n",  card_str_def(&pPlayer->stEquipCard[EquipIdx_Armor], buf, sizeof(buf), "None") );
	printf("    Horse(+1) : %s\n", card_str_def(&pPlayer->stEquipCard[EquipIdx_HorseInc], buf, sizeof(buf), "None") );
	printf("    Horse(-1) : %s\n",  card_str_def(&pPlayer->stEquipCard[EquipIdx_HorseDec], buf, sizeof(buf), "None") );

	// judgment cards
	printf("    Judgment cards (%d):\n",  pPlayer->nJudgmentCardNum);
	for(n = 0; n < pPlayer->nJudgmentCardNum; n++)
	{
		//if(n > 0 && n % 4 == 0) printf("\n           ");
		printf("      (%d) %s;\n", n+1, card_str(&pPlayer->stJudgmentCards[n], buf, sizeof(buf)));
	}
	return R_SUCC;
}


RESULT game_global_info(GameContext* pGame, GameEventContext* pEvent)
{
	int n, k;
	char buf[128];
	Player* p;


	if(game_status(pGame) == Status_None)
	{
		printf("not in game!\n");
		return R_E_STATUS;
	}


	printf("game global info: \n");

	printf("(*) %d players (%d+%d+%d+%d): \n", pGame->nPlayerCount, 1, pGame->nMinsterCount, pGame->nSpyCount, pGame->nMutineerCount);

	for(k = 0; k < pGame->nPlayerCount; k++)
	{
		p = &pGame->players[k];

		if(IS_PLAYER_DEAD(p) )
			continue;

		printf("  (%d) %s%s +%d %s, %s, life: %d/%d, hand cards: %d\n", k + 1,
			pGame->nRoundPlayer == k ? "R":"-", pGame->nCurPlayer == k ? "C":"-",
			game_player_dis(pGame, pGame->nCurPlayer, k),
			player_id_str((k == pGame->nCurPlayer || IS_PLAYER_SHOW(p) ) ? p->id : PlayerID_Unknown ),
			p->name, p->curLife, p->maxLife, p->nHandCardNum);

		// equiped cards
		printf("    Weapon : %s\n",  card_str_def(&p->stEquipCard[EquipIdx_Weapon], buf, sizeof(buf), "None") );
		printf("    Armor : %s\n",  card_str_def(&p->stEquipCard[EquipIdx_Armor], buf, sizeof(buf), "None") );
		printf("    Horse(+1) : %s\n", card_str_def(&p->stEquipCard[EquipIdx_HorseInc], buf, sizeof(buf), "None") );
		printf("    Horse(-1) : %s\n",  card_str_def(&p->stEquipCard[EquipIdx_HorseDec], buf, sizeof(buf), "None") );

		// judgment cards
		printf("    Judgment cards (%d):\n",  p->nJudgmentCardNum);
		for(n = 0; n < p->nJudgmentCardNum; n++)
		{
			//if(n > 0 && n % 4 == 0) printf("\n           ");
			printf("      (%d) %s;\n", n+1, card_str(&p->stJudgmentCards[n], buf, sizeof(buf)));
		}
	}
	printf("(*) stack   cards: %d\n", pGame->stGetCardStack.count);
	printf("(*) discard cards: %d\n", pGame->stDiscardCardStack.count);
	printf("(*) cur dis cards: %d\n", pGame->nCurDiscardCardNum);
	return R_SUCC;
}

