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
	int  idx = player;
	do {
		idx = (idx +  1) % pGame->nPlayerCount;
	} while(idx != player && IS_PLAYER_DEAD(GAME_PLAYER(pGame, idx)));

	return idx;
}

int game_prev_player(GameContext* pGame, int player)
{
	// TODO: must think about the player is die
	int  idx = player;
	do {
		idx = (idx - 1 + pGame->nPlayerCount) % pGame->nPlayerCount;
	} while(idx != player && IS_PLAYER_DEAD(GAME_PLAYER(pGame, idx)));

	return idx;
}

RESULT set_game_cur_player(GameContext* pGame, int player)
{
	if(player < 0 || player >= pGame->nPlayerCount)
		return R_E_PARAM;

	if(player != pGame->nCurPlayer)
	{
		pGame->nCurPlayer = player;
		printf("the current player is set to [%s]\n", CUR_PLAYER(pGame)->name);
	}

	return R_SUCC;
}


int game_player_dis(GameContext* pGame, int p1, int p2)
{
	// from p1 to p2
	int dis = 0;
	int pn = p1;
	int pp = p1;

	while(pn != p2 && pp != p2)
	{
		dis++;
		do {
			pn = (pn + 1) % pGame->nPlayerCount;
		} while(pn != p2 && IS_PLAYER_DEAD(GAME_PLAYER(pGame, pn)));
		do {
			pp = (pn - 1 + pGame->nPlayerCount) % pGame->nPlayerCount;
		} while(pp != p2 && IS_PLAYER_DEAD(GAME_PLAYER(pGame, pp)));
	}

	return dis;
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
	pGame->nCurPlayer = -1;


	for(n = 0; n < pGame->nPlayerCount; ++n)
	{
		if(pids[n] == PlayerID_Master)
		{
			set_game_cur_player(pGame, n);
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
	printf("the round [%d] is start, round player is [%s]\n", pGame->nRoundNum, ROUND_PLAYER(pGame)->name);

	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_RoundBegin, pGame->nRoundPlayer, 0, pEvent);


	trigger_game_event(pGame, &event);

	return R_SUCC;
}

static RESULT game_round_judge(GameContext* pGame, GameEventContext* pEvent)
{
	printf("enter the round [%d]  judgment phase, round player is [%s]\n", pGame->nRoundNum, ROUND_PLAYER(pGame)->name);

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
				if(pCardConfig->out != NULL)
				{
					INIT_EVENT(&event, GameEvent_CardCalc, pGame->nRoundPlayer, 0, pEvent);
					event.pCard = &stCard;
					(*pCardConfig->out)(pGame, &event, pGame->nCurPlayer);
				}

				INIT_EVENT(&event, GameEvent_PostCardCalc, pGame->nRoundPlayer, 0, pEvent);
				event.pCard = &stCard;
				trigger_game_event(pGame, &event);

			}

			// after calc
			if(stCard.id != CardID_None)
			{
				if(pCardConfig->out != NULL)
				{
					INIT_EVENT(&event, GameEvent_FiniCardCalc, pGame->nRoundPlayer, 0, pEvent);
					event.pCard = &stCard;
					(*pCardConfig->out)(pGame, &event, pGame->nCurPlayer);
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
	int num;
	GameEventContext  event;

	printf("enter the round [%d]  get card phase, round player is [%s]\n", pGame->nRoundNum, ROUND_PLAYER(pGame)->name);


	num = 2;  // in get round init to get 2 card 
	INIT_EVENT(&event, GameEvent_PerRoundGet, pGame->nRoundPlayer, 0, pEvent);
	event.pNum = &num;
	trigger_game_event(pGame, &event);

	if(event.result == R_CANCEL)
	{
		// skip getcard step
		return R_SUCC;
	}
	
	game_round_do_get(pGame, pEvent, pGame->nRoundPlayer, num);

	INIT_EVENT(&event, GameEvent_PostRoundGet, pGame->nRoundPlayer, 0, pEvent);
	trigger_game_event(pGame, &event);

	return R_SUCC;
}

static RESULT game_round_outcard(GameContext* pGame, GameEventContext* pEvent)
{
	GameEventContext  event;

	printf("enter the round [%d]  out card phase, round player is [%s]\n", pGame->nRoundNum, ROUND_PLAYER(pGame)->name);

	INIT_EVENT(&event, GameEvent_PerRoundOut, pGame->nRoundPlayer, 0, pEvent);
	trigger_game_event(pGame, &event);

	if(event.result == R_CANCEL)
	{
		// skip getcard step
		return R_SUCC;
	}

	while (R_SUCC == game_round_do_out(pGame, pEvent, pGame->nRoundPlayer))
	{
		// do nothing
	}

	INIT_EVENT(&event, GameEvent_PostRoundOut, pGame->nRoundPlayer, 0, pEvent);
	trigger_game_event(pGame, &event);

	return R_SUCC;
}

static RESULT game_round_discardcard(GameContext* pGame, GameEventContext* pEvent)
{
	GameEventContext  event;

	printf("enter the round [%d]  discard card phase, round player is [%s]\n", pGame->nRoundNum, ROUND_PLAYER(pGame)->name);

	// trigger round discard event
	INIT_EVENT(&event, GameEvent_PerRoundDiscard, pGame->nRoundPlayer, 0, pEvent);
	trigger_game_event(pGame, &event);

	if(event.result == R_CANCEL)
	{
		// skip getcard step
		return R_SUCC;
	}

	// wait cmd_loop discard cmd execute
	INIT_EVENT(&event, GameEvent_PostRoundDiscard, pGame->nRoundPlayer, 0, pEvent);
	trigger_game_event(pGame, &event);

	return R_SUCC;
}

static RESULT game_round_end(GameContext* pGame, GameEventContext* pEvent)
{
	// trigger round end event
	GameEventContext  event;

	printf("the round [%d] is finish, round player is [%s]\n", pGame->nRoundNum, ROUND_PLAYER(pGame)->name);


	INIT_EVENT(&event, GameEvent_RoundEnd, pGame->nRoundPlayer, 0, pEvent);

	trigger_game_event(pGame, &event);

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
		pGame->nRoundNum = 1;
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
	while(ret == R_SUCC)
	{
		ret = game_next_status(pGame, pEvent);
		if(ret == R_SUCC)
		{
			ret = game_step(pGame, pEvent);
		}
	}
	return ret;
}

RESULT game_player_add_life(GameContext* pGame, GameEventContext* pParentEvent, int player, int life_inc)
{
	//RESULT ret;
	GameEventContext   event;
	Player* pPlayer;
	
	// event perlost life
	INIT_EVENT(&event, life_inc < 0 ? GameEvent_PerLostLife: GameEvent_PerAddLife, player, 0, pParentEvent);
	event.pNum = &life_inc;

	trigger_game_event(pGame, &event);

	if(event.result == R_CANCEL || life_inc == 0)
		return R_CANCEL;

	pPlayer = GAME_PLAYER(pGame, player);

	pPlayer->curLife += life_inc;

	if(pPlayer->curLife > pPlayer->maxLife)
		pPlayer->curLife = pPlayer->maxLife;
	else if(pPlayer->curLife <= 0)
	{
		// event perdead ?
		INIT_EVENT(&event, GameEvent_PerDead, player, 0, pParentEvent);
		trigger_game_event(pGame, &event);

		// event dead

		if(pPlayer->curLife <= 0)
		{
			INIT_EVENT(&event, GameEvent_Dead, player, 0, pParentEvent);
			trigger_game_event(pGame, &event);
		}
	}

	INIT_EVENT(&event, life_inc < 0 ? GameEvent_PostLostLife: GameEvent_PostAddLife, player, 0, pParentEvent);
	event.pNum = &life_inc;

	trigger_game_event(pGame, &event);

	return R_SUCC;
}


RESULT game_player_discard_card(GameContext* pGame, GameEventContext* pParentEvent, int player, int where, int pos)
{
	PosCard  stCard;
	char buf[128];
	Player* pPlayer = GAME_PLAYER(pGame, player);

	// event: per lost card 
	
	if(R_SUCC == player_remove_card(pPlayer, where, pos, &stCard.card))
	{
		stCard.card.flag = CardFlag_None;
		stCard.where = where;
		stCard.pos = pos;

		printf("[%s] discard a card %s\n", pPlayer->name, card_str(&stCard.card, buf, sizeof(buf)));

		// event: post lost card

		card_stack_push(&pGame->cardOut, &stCard.card);
	}
	return R_SUCC;
}

RESULT game_player_equip_card(GameContext* pGame, GameEventContext* pParentEvent, int player, int pos, Card* pCard)
{
	char buf[128];
	Player* pPlayer = GAME_PLAYER(pGame, player);

	// add event per equip card

	if(CARD_VALID(&pPlayer->stEquipCard[pos]))
	{
		game_player_discard_card(pGame, pParentEvent, player, PlayerCard_Equip, pos);
	}


	printf("[%s] equip a [%s] card %s\n", pPlayer->name, equip_idx_str(pos), card_str(pCard, buf, sizeof(buf)));

	pPlayer->stEquipCard[pos] = *pCard;
	pPlayer->stEquipCard[pos].flag = CardFlag_None;

	// add event post equip card

	return R_SUCC;
}



RESULT game_select_target(GameContext* pGame, GameEventContext* pParentEvent, int player, int base_dist, YESNO self_select, YESNO may_cancel,const char* alter_text, int* out_target)
{
	int n;
	int t;
	RESULT  ret;
	AttackDis   dis;
	GameEventContext  event;
	Player* pPlayer; 
	Player* pTarget;
	int idcnt;
	int idbegin ;
	SelOption   sel_opts[MAX_PLAYER_NUM + 1];

	ST_ZERO(sel_opts);

	set_game_cur_player(pGame,player );


	pPlayer = GAME_PLAYER(pGame, player);

	idbegin = self_select == YES ? 0 : 1;
	idcnt = 0;

	for(n = idbegin; n < pGame->nPlayerCount; n++)
	{
		t = (player + n) % pGame->nPlayerCount;
		pTarget = GAME_PLAYER(pGame, t);
		if(!IS_PLAYER_DEAD(pTarget))
		{
			snprintf(sel_opts[idcnt].text, sizeof(sel_opts[idcnt].text), "%s, %s, life: %d/%d%s, hand cards: %d", 
				player_id_str( (t == player || IS_PLAYER_SHOW(pTarget) || IS_PLAYER_DEAD(pTarget)) ? pTarget->id : PlayerID_Unknown),
				pTarget->name, pTarget->curLife, pTarget->maxLife, IS_PLAYER_DEAD(pTarget)?"(Dead)":"", pTarget->nHandCardNum);

			snprintf(sel_opts[idcnt].input, sizeof(sel_opts[idcnt].input), "%d", idbegin + idcnt);
			sel_opts[idcnt].value = t;
			idcnt++;
		}
	}

	// 
	if(may_cancel == YES)
	{
		snprintf(sel_opts[idcnt].text, sizeof(sel_opts[idcnt].text),"Cancel");
		snprintf(sel_opts[idcnt].input, sizeof(sel_opts[idcnt].input), "%s\n%s", "c", "cancel");
		sel_opts[idcnt].value =  -100;
		idcnt++;
	}


	while(1)
	{

		ret = select_loop(pGame, pParentEvent, sel_opts, idcnt, alter_text ? alter_text : "Select a target role:", &t);

		if(ret != R_SUCC)
			return ret;

		if(idcnt == -100)
			return R_CANCEL;


		pTarget = GAME_PLAYER(pGame, t);


		// can set target?
		INIT_EVENT(&event, GameEvent_SelectTarget, player, t, pParentEvent);

		trigger_game_event(pGame, &event);

		if(event.result == R_CANCEL)
		{
			// cannot select this player as target
			printf("The selected role cannot be as a target, please select another!\n");
			continue;
		}

		// need check dis ?  check dist 
		if(base_dist >= 0)
		{
			// calc final dist, if base_distancc is -1, means ignore distance to target
			dis.base = base_dist;   // calc the attack range
			dis.inc = 0;            // calc the attack range append increase
			dis.dis = game_player_dis(pGame, player, t);   // calc the player to target's distance
			dis.flag = 0;

			INIT_EVENT(&event, GameEvent_CalcAttackDis, player, t, pParentEvent);
			event.pAttackDis = &dis;

			// calc the skill and equip effect to target distance or attack range
			// target effect
			trigger_player_event(pGame, &event, t);
			// attacker effect
			trigger_player_event(pGame, &event, player);

			if(dis.base + dis.inc < dis.dis)
			{
				// Attack range less the distance to taget
				printf("The selected role is out of your attack range, please select another!\n");
				continue;
			}
		}
		*out_target = t;
		return R_SUCC;
	}

	return R_E_FAIL;
}

YESNO game_select_yesno(GameContext* pGame, GameEventContext* pParentEvent, int player, const char* alter_text)
{
	int result;
	SelOption   sel_opts[2];
	ST_ZERO(sel_opts);

	set_game_cur_player(pGame,player );


	snprintf(sel_opts[0].text, sizeof(sel_opts[0].text),"[ Yes ]");
	snprintf(sel_opts[0].input, sizeof(sel_opts[0].input), "y\nyes");
	sel_opts[0].value =  YES;

	snprintf(sel_opts[1].text, sizeof(sel_opts[0].text),"[ No  ]");
	snprintf(sel_opts[1].input, sizeof(sel_opts[0].input), "n\nno");
	sel_opts[1].value =  NO;

	if(select_loop(pGame, pParentEvent, sel_opts, 2, alter_text, &result) == R_SUCC)
		return (YESNO) result;

	return NO;
}

