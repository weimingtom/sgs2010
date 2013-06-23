#include "config.h"
#include "game.h"
#include "comm.h"
#include "hero.h"
#include "cmd.h"
#include "event.h"
#include "get.h"


//#define NEXT_ROUND(pGame)   ((pGame)->nRoundPlayer = game_next_player((pGame), (pGame)->nRoundPlayer))
//#define NEXT_CUR(pGame)   ((pGame)->nCurPlayer = game_next_player((pGame),(pGame)->nCurPlayer))




Status game_status(GameContext* pGame)
{
	return pGame->status;
}

int get_game_cur_player(GameContext* pGame)
{
	return pGame->nCurPlayer;
}

int get_game_round_player(GameContext* pGame)
{
	return pGame->nRoundPlayer;
}

int get_game_master_player(GameContext* pGame)
{
	int n;
	for(n = 0; n < pGame->nPlayerCount; n++)
	{
		if(pGame->players[n].id == PlayerID_Master)
			return n;
	}
	return -1;
}


int game_next_player(GameContext* pGame, int player)
{
	// TODO: must think about the player is die
	return (player + 1) % pGame->nPlayerCount;
}

int game_prev_player(GameContext* pGame, int player)
{
	// TODO: must think about the player is die
	return (player + pGame->nPlayerCount - 1) % pGame->nPlayerCount;
}


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

	printf("  (%d) %s%s +%d -%d %s, %s, life: %d/%d, hand cards: %d\n", player + 1, pGame->nRoundPlayer == player ? "R":"-", pGame->nCurPlayer == player  ? "C":"-",
		(player - pGame->nCurPlayer + pGame->nPlayerCount) % pGame->nPlayerCount, 
		(pGame->nCurPlayer - player + pGame->nPlayerCount) % pGame->nPlayerCount, 
		player_id_str(pPlayer->id), pPlayer->name, pPlayer->curLife, pPlayer->maxLife, pPlayer->nHandCardNum);

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

		printf("  (%d) %s%s +%d -%d %s, %s, life: %d/%d, hand cards: %d\n", k + 1, pGame->nRoundPlayer == k ? "R":"-", pGame->nCurPlayer == k ? "C":"-",
			(k - pGame->nCurPlayer + pGame->nPlayerCount) % pGame->nPlayerCount, 
			(pGame->nCurPlayer - k + pGame->nPlayerCount) % pGame->nPlayerCount, 
			player_id_str(p->id), p->name, p->curLife, p->maxLife, p->nHandCardNum);

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
	printf("(*) stack cards: %d\n", pGame->cardStack.count);
	printf("(*) out   cards: %d\n", pGame->cardOut.count);
	return R_SUCC;
}


RESULT init_game_context(GameContext* pGame, int minsters, int spies, int mutineers)
{
	int n, c;
	int pids[MAX_PLAYER_NUM];
	int hids[HeroID_Max];
	int hcnt;
	int hmcnt;
	int hscnt;
	const HeroConfig* pHero;


	if(minsters < 1 || spies < 1 || mutineers < 1 || minsters + spies + mutineers + 1 > MAX_PLAYER_NUM)
	{
		printf("error player config num!\n");
		return R_E_PARAM;
	}
	
	hcnt = 0;
	hmcnt = 0;
	for(n = 0; n < HeroID_Max; n++)
	{
		pHero = get_hero_config((HeroID)n);
		if(pHero)
		{
			if(pHero->isMaster)
			{
				if(hcnt > hmcnt)
				{
					hids[hcnt] = hids[hmcnt];
				}
				hids[hmcnt] = n;
				hmcnt++;
			}
			else
			{
				hids[hcnt] = n;
			}
			hcnt++;
		}
	}

	if(hcnt == 0 || hmcnt == 0)
	{
		printf("not any valid hero config.\n");
		return R_E_FAIL;
	}


	memset(pGame, 0, sizeof(*pGame));

	pGame->nPlayerCount = minsters + spies + mutineers + 1;
	pGame->nMinsterCount = minsters;
	pGame->nSpyCount = spies;
	pGame->nMutineerCount = mutineers;
	
	printf("new game: %d players - [%d master + %d minster + %d spy + %d mutineer]\n", pGame->nPlayerCount, 1, pGame->nMinsterCount, pGame->nSpyCount, pGame->nMutineerCount);

	// init players
	c = 0;
	pids[c++] = PlayerID_Master;
	for(n = 0; n < minsters; n++) pids[c++] = PlayerID_Minster;
	for(n = 0; n < spies; n++) pids[c++] = PlayerID_Spy;
	for(n = 0; n < mutineers; n++) pids[c++] = PlayerID_Mutineer;

	rand_array_i(pids, c, c * 2);

	pGame->status = Status_NewGame;
	pGame->nRoundNum = 0;
	pGame->nRoundPlayer = 0;
	pGame->nCurPlayer = 0;


	for(n = 0; n < pGame->nPlayerCount; ++n)
	{
		if(pids[n] == PlayerID_Master)
		{
			pGame->nCurPlayer = n;
			pGame->nRoundPlayer = n;
			break;
		}
	}

	for(n = 0; n < pGame->nPlayerCount; n++)
	{
		if(n == 0)
		{
			if(hcnt > hmcnt)
			{
				rand_array_i(hids + hmcnt, hcnt - hmcnt, (hcnt - hmcnt) * 3);
			}
			hscnt = MIN(5, hcnt);
		}
		else
		{
			rand_array_i(hids, hcnt, hcnt * 3);
			hscnt = MIN(3, hcnt);
		}

		while(1)
		{
			printf("current player [%d], identification is %s, select hero:\n", pGame->nCurPlayer, player_id_str((PlayerID)pids[pGame->nCurPlayer]));
			for(c = 0; c < hscnt; c++)
			{
				pHero = get_hero_config((HeroID)hids[c]);
				printf("  (%d) %s, %slife %d;\n", c + 1, pHero->name, pHero->isMaster ? "MASTER, ":"", pHero->life);
			}

			printf("please select (%d-%d): ", 1, hscnt);
			fflush(stdin);
			if(1 == scanf("%d", &c) && c >= 1 && c <= hscnt)
			{
				break;
			}

		}
		init_player(&pGame->players[pGame->nCurPlayer], (PlayerID)pids[pGame->nCurPlayer], (HeroID)hids[c-1]);
		if(c < hcnt)
		{
			hids[c-1] = hids[hcnt-1];
		}
		hcnt--;

		pGame->nCurPlayer = (pGame->nCurPlayer + 1) % pGame->nPlayerCount;
		
	}


	// init card stack

	init_card_stack(&pGame->cardStack);
	card_stack_random(&pGame->cardStack);

	// clear out cards;
	pGame->cardOut.count = 0;


	pGame->nRoundNum = 0;

	// first get 4 cards per player

	game_first_getcard(pGame);


	return R_SUCC;
}


static RESULT game_next_round(GameContext* pGame, GameEventContext* pEvent);

static RESULT game_round_begin(GameContext* pGame, GameEventContext* pEvent)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_RoundBegin, pGame->nRoundPlayer, 0, pEvent);


	trigger_game_event(pGame, &event);

	return R_SUCC;
}

static RESULT game_round_judge(GameContext* pGame, GameEventContext* pEvent)
{

	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PerRoundJudge, pGame->nRoundPlayer, 0, pEvent);
	trigger_game_event(pGame, &event);

	// is ot not skip this round step ?
	if(event.result == R_CANCEL)
		return R_SUCC;
	
	// judge cards
	Player* pPlayer = ROUND_PLAYER(pGame);
	Card stCard;
	const CardConfig* pCardConfig;


	while(pPlayer->nJudgmentCardNum > 0)
	{
		stCard = pPlayer->stJudgmentCards[pPlayer->nJudgmentCardNum-1];
		pPlayer->nJudgmentCardNum--;

		pCardConfig = get_card_config(stCard.id);

		if(pCardConfig)
		{
			INIT_EVENT(&event, GameEvent_PerCardCalc, pGame->nRoundPlayer, 0, pEvent);
			event.pCard = &stCard;
			trigger_game_event(pGame, &event);

			if(event.result != R_CANCEL) // if card calc is cancel .
			{
				if(pCardConfig->calc != NULL)
				{
					INIT_EVENT(&event, GameEvent_CardCalc, pGame->nRoundPlayer, 0, pEvent);
					event.pCard = &stCard;
					(*pCardConfig->calc)(pGame, &event, pGame->nCurPlayer);
				}

				INIT_EVENT(&event, GameEvent_PostCardCalc, pGame->nRoundPlayer, 0, pEvent);
				event.pCard = &stCard;
				trigger_game_event(pGame, &event);

			}

			// after calc
			if(stCard.id != CardID_None)
			{
				if(pCardConfig->fini != NULL)
				{
					INIT_EVENT(&event, GameEvent_FiniCardCalc, pGame->nRoundPlayer, 0, pEvent);
					event.pCard = &stCard;
					(*pCardConfig->fini)(pGame, &event, pGame->nCurPlayer);
				}
				else
				{
					// after calc discard card (default fini action)?
					card_stack_push(&pGame->cardOut, &stCard);
				}
			}
		}
		else
		{
			printf("card config [%d] not found!\n", stCard.id,  pGame->nRoundPlayer);
		}
	}

	INIT_EVENT(&event, GameEvent_PostRoundJudge, pGame->nRoundPlayer, 0, pEvent);
	trigger_game_event(pGame, &event);

	return R_SUCC;
}


static RESULT game_round_getcard(GameContext* pGame, GameEventContext* pEvent)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PerRoundGet, pGame->nRoundPlayer, 0, pEvent);
	trigger_game_event(pGame, &event);

	if(event.result == R_CANCEL)
	{
		// skip getcard step
	}
	


	INIT_EVENT(&event, GameEvent_PostRoundGet, pGame->nRoundPlayer, 0, pEvent);
	trigger_game_event(pGame, &event);

	pGame->status = Status_Round_Out;

	return R_SUCC;
}

static RESULT game_round_outcard(GameContext* pGame, GameEventContext* pEvent)
{
	
	pGame->status = Status_Round_Discard;
	return R_SUCC;
}

static RESULT game_round_discardcard(GameContext* pGame, GameEventContext* pEvent)
{
	// trigger round discard event

	// wait cmd_loop discard cmd execute


	pGame->status = Status_Round_End;
	return R_SUCC;
}

static RESULT game_round_end(GameContext* pGame, GameEventContext* pEvent)
{
	// trigger round end event
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_RoundBegin, pGame->nRoundPlayer, 0, pEvent);

	trigger_game_event(pGame, &event);

	game_next_round(pGame, pEvent);
	return R_SUCC;
}


static RESULT game_next_round(GameContext* pGame, GameEventContext* pEvent)
{
	// calc next round player
	for(;;)
	{
		pGame->nRoundPlayer = game_next_player(pGame, pGame->nRoundPlayer);
		pGame->nRoundNum++;

		if(!PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipNextRound))
			break;

		// clear flags
		PLAYER_CLR_ALL_FLAG(ROUND_PLAYER(pGame));
	}


	printf("next round: num [%d], round player is set to: %d, [%s]\n", pGame->nRoundNum, pGame->nRoundPlayer, ROUND_PLAYER(pGame)->name);

	// set status round begin
	pGame->status = Status_Round_Begin;

	// clear status
	PLAYER_CLR_ALL_FLAG(ROUND_PLAYER(pGame));

	return R_SUCC;
}

static RESULT game_next_status(GameContext* pGame, GameEventContext* pEvent)
{
	switch(pGame->status)
	{
	case Status_NewGame:
		pGame->status = Status_Round_Begin;
		pGame->nRoundPlayer = get_game_master_player(pGame);
		break;
	case Status_Round_Begin:
		if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRound))
			goto __RoundEnd;
		if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRoundJudge))
			goto __RoundJudge;
		pGame->status = Status_Round_Judge;
		break;
	case Status_Round_Judge:
		if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRound))
			goto __RoundEnd;
	__RoundJudge:
		if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRoundGet))
			goto __RoundGet;
		pGame->status = Status_Round_Get;
		break;
	case Status_Round_Get:
		if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRound))
			goto __RoundEnd;
	__RoundGet:
		if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRoundOut))
			goto __RoundOut;
		pGame->status = Status_Round_Out;
		break;
	case Status_Round_Out:
		if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRound))
			goto __RoundEnd;
	__RoundOut:
		if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRoundDiscard))
			goto __RoundDiscard;
		pGame->status = Status_Round_Discard;
		break;
	case Status_Round_Discard:
		if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRound))
			goto __RoundEnd;
	__RoundDiscard:
		pGame->status = Status_Round_End;
		break;
	case  Status_Round_End:
	__RoundEnd:
		game_next_round(pGame, pEvent);
		break;
	default:
		return R_E_STATUS;
		break;
	}
	return R_SUCC;
}

static RESULT game_step(GameContext* pGame, GameEventContext* pEvent)
{
	switch(pGame->status)
	{
	case Status_Round_Begin:
		return game_round_begin(pGame, pEvent);
		break;
	case Status_Round_Judge:
		return game_round_judge(pGame, pEvent);
		break;
	case Status_Round_Get:
		return game_round_getcard(pGame, pEvent);
		break;
	case Status_Round_Out:
		return game_round_outcard(pGame, pEvent);
		break;
	case Status_Round_Discard:
		return game_round_discardcard(pGame, pEvent);
		break;
	case Status_Round_End:
		return game_round_end(pGame, pEvent);
		break;
	}
	return R_E_STATUS;
}

// 
RESULT game_loop(GameContext* pGame, GameEventContext* pEvent)
{
	RESULT ret = R_SUCC;
	while(ret != R_SUCC)
	{
		ret = game_next_status(pGame, pEvent);
		if(ret == R_SUCC)
		{
			ret = game_step(pGame, pEvent);
		}
	}
	return ret;
}




RESULT game_passive_out(GameContext* pGame, GameEventContext* pParentEvent, int player, const char* alter_text, PassiveOut* pPassiveOut)
{
	GameEventContext  event;
	RESULT ret;


	INIT_EVENT(&event, GameEvent_PassiveOutCard, player, player, pParentEvent);
	event.pPassiveOut =  pPassiveOut;

	pGame->nCurPlayer = player;

	ret = cmd_loop(pGame, &event, alter_text);

	if(event.result == R_SUCC)
	{
		// 
	}

	return event.result;
}

/*
RESULT game_supply_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int player, const CardPattern* pattern, OutCard* pOut)
{
	char   text[1024];
	char   temp[128];
	GameEventContext  event;
	RESULT ret;

	INIT_EVENT(&event, GameEvent_SupplyCard, trigger, player, pParentEvent);
	event.pattern.patterns[0] = *pattern;
	event.pattern.num = 1;

	pGame->nCurPlayer = player;

	snprintf(text, sizeof(text), "player [%s] supply card [%s], please 'out req card' or 'cancel'", CUR_PLAYER(pGame)->name, card_pattern_str(pattern, temp, sizeof(temp)));

	ret = cmd_loop(pGame, &event, text);

	if(event.result == R_SUCC)
	{
		*pOut = event.out;
	}

	return event.result;
}

*/


RESULT game_select_target(GameContext* pGame, GameEventContext* pParentEvent, int player, int base_dist, YESNO self_select, const char* alter_text, int* pTarget)
{
	int n;
	int t;
	RESULT  ret;
	AttackDis   dis;
	GameEvent   event;
	const CardConfig* pCardConfig;
	Player* pPlayer; 
	Player* pTarget;

	pPlayer = GAME_PLAYER(pGame, player);

	//INIT_EVENT(&event, GameEvent_SelectTarget, player, 0, pParentEvent);

	//ret = cmd_loop(pGame, &event, alter_text);

	if(alter_text)
		printf("%s:\n", alter_text);
	else
		printf("选择一个目标角色:\n");
	for(n = (self_select == YES) ? 0 : 1; n < pGame->nPlayerCount; n++)
	{
		t = (player + n) % pGame->nPlayerCount;


		pTarget = GAME_PLAYER(pGame, t);

		printf(" (%d) %s, %s, life: %d/%d, hand cards: %d\n", player_str(pTarget->id), pTarget->name, pTarget->curLife, pTarget->maxLife, pTarget->nHandCardNum);

	}
	
	printf("请选择:");




	if(event.result != R_SUCC)
		return event.result;

	// check dist
	if(base_dist >= 0)
	{
		// calc final dist, if base_distancc is -1, means ignore distance to target
		dis.base = base_dist;
		dis.inc = 0;

		INIT_EVENT(&event, GameEvent_CalcAttackDis, player, 0, pParentEvent);
		event.pAttackDis = &dis;

		// attack weapon attack dist
		if(CARD_VALID(&))
		pCardConfig = get_card_config()

	}
	

	return R_CANCEL;
}

