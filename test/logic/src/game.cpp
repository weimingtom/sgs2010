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


	pGame->status = Status_FirstGetCard;
	pGame->nRoundNum = 0;

	// first get 4 cards per player

	game_first_getcard(pGame);


	return R_SUCC;
}


static RESULT game_next_round(GameContext* pGame, GameEventContext* pEvent);

static RESULT game_round_begin(GameContext* pGame, GameEventContext* pEvent)
{

	if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipNextRound))
		return game_next_round(pGame, pEvent);

	pGame->nCurPlayer = pGame->nRoundPlayer;

	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_RoundBegin, pGame->nRoundPlayer, 0, NULL);


	trigger_game_event(pGame, &event);


	pGame->status = Status_Round_Judge;

	return R_SUCC;
}

static RESULT game_round_judge(GameContext* pGame, GameEventContext* pEvent)
{

	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PerRoundJudge, pGame->nRoundPlayer, 0, pEvent);
	trigger_game_event(pGame, &event);

	// is ot not skip this round step ?
	

	// judge cards
	Player* pPlayer = ROUND_PLAYER(pGame);
	Card stCard;
	const CardConfig* pCardConfig;



	while(pPlayer->nJudgmentCardNum > 0)
	{
		stCard = pPlayer->stJudgmentCards[pPlayer->nJudgmentCardNum-1];
		pCardConfig = get_card_config(stCard.id);

		if(pCardConfig)
		{
			INIT_EVENT(&event, GameEvent_PerCardCalc, pGame->nRoundPlayer, 0, pEvent);
			event.card = stCard;
			trigger_game_event(pGame, &event);

			if(event.result != R_CANCEL) // if card calc is cancel .
			{
				if(pCardConfig->calc != NULL)
				{
					INIT_EVENT(&event, GameEvent_CardCalc, pGame->nRoundPlayer, 0, pEvent);
					event.card = stCard;

					pGame->nCurPlayer = pGame->nRoundPlayer;
					(*pCardConfig->calc)(pGame, &event, pGame->nCurPlayer);
				}

				INIT_EVENT(&event, GameEvent_PostCardCalc, pGame->nRoundPlayer, 0, pEvent);
				event.card = stCard;
				trigger_game_event(pGame, &event);

				stCard = event.card;

			}

			// after calc
			if(stCard.id == CardID_None)
			{
				// card is lost
				pPlayer->nJudgmentCardNum--;
			}
			else
			{
				if(pCardConfig->fini != NULL)
				{
					(*pCardConfig->fini)(pGame, NULL, pGame->nCurPlayer);
				}
				else
				{
					// after calc discard card?
					card_stack_push(&pGame->cardOut, &stCard);

					pPlayer->nJudgmentCardNum--;

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

	pGame->status = Status_Round_Get;

	return R_SUCC;
}


static RESULT game_round_getcard(GameContext* pGame, GameEventContext* pEvent)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PerRoundGet, pGame->nRoundPlayer, 0, pEvent);
	trigger_game_event(pGame, &event);






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
	pGame->nRoundPlayer = (pGame->nRoundPlayer + 1) & pGame->nPlayerCount;
	pGame->nRoundNum++;


	printf("next round: num [%d], round player is set to: %d, [%s]\n", pGame->nRoundNum, pGame->nRoundPlayer, ROUND_PLAYER(pGame)->name);

	// set status round begin
	pGame->status = Status_Round_Begin;
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
	}
	return R_E_STATUS;
}

// 
RESULT game_loop(GameContext* pGame, GameEventContext* pEvent)
{
	RESULT ret = R_SUCC;
	while(ret != R_SUCC)
	{
		ret = game_step(pGame, pEvent);
	}
	return ret;
}

/*
int game_loop(GameContext* pGame)
{
	// first get card
	get_first_hand_card(pGame);

	// begin round

	while(pGame->status != Status_None)
	{
		// a round
		do {

			// round is skip ?
			if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipNextRound))
			{
				PLAYER_CLR_FLAG(ROUND_PLAYER(pGame),PlayerFlag_SkipNextRound);
				break;
			}

			pGame->nCurPlayer = pGame->nRoundPlayer;
			
			// (1) begin round
			game_round_begin(pGame);

			// (2) judge
			if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRound))
			{
				break;
			}

			if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRoundJudge))
			{
				PLAYER_CLR_FLAG(ROUND_PLAYER(pGame),PlayerFlag_SkipThisRoundJudge);
			}
			else
			{
				game_round_judge(pGame);
			}

			// (3) get card
			if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRound))
			{
				break;
			}

			if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRoundGet))
			{
				PLAYER_CLR_FLAG(ROUND_PLAYER(pGame),PlayerFlag_SkipThisRoundGet);
			}
			else
			{
				game_round_getcard(pGame);
			}

			// (4) out card
			if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRound))
			{
				break;
			}

			if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRoundOut))
			{
				PLAYER_CLR_FLAG(ROUND_PLAYER(pGame),PlayerFlag_SkipThisRoundOut);
			}
			else
			{
				game_round_outcard(pGame);
			}

			// (5) discard card
			if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRound))
			{
				break;
			}

			if(PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipThisRoundDiscard))
			{
				PLAYER_CLR_FLAG(ROUND_PLAYER(pGame),PlayerFlag_SkipThisRoundDiscard);
			}
			else
			{
				game_round_discardcard(pGame);
			}

			// (6) end round
			game_round_end(pGame);
		} while(0);

		PLAYER_CLR_FLAG(ROUND_PLAYER(pGame),PlayerFlag_AllThisSkipFlag);

		NEXT_ROUND(pGame);

	}

	return 0;
}

*/




static RESULT per_out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, OutCard* pOut)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PerOutCard, trigger, target, pParentEvent);
	event.out = *pOut;

	trigger_game_event(pGame, &event);

	return event.result;
}

static RESULT post_out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, OutCard* pOut)
{
	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_PostOutCard, trigger, target, pParentEvent);
	event.out = *pOut;

	trigger_game_event(pGame, &event);

	*pOut = event.out;

	return event.result;	
}

static RESULT remove_out_card(GameContext* pGame, int supply, OutCard* pOut)
{
	int n;
	char buf[128];
	if(pOut->nrcard == 0)
	{
		if(0 != player_remove_card(&pGame->players[supply], pOut->vcard.pos))
		{
			printf("remove out card [%s]  from player [%d] pos [%d] failed ", card_str(&pOut->vcard.card, buf, sizeof(buf)), supply, pOut->vcard.pos);
			return R_E_FAIL;
		}
	}
	else
	{
		for(n = 0; n < pOut->nrcard; n++)
		{
			player_remove_card(&pGame->players[supply], pOut->rcards[n].pos);
			printf("remove out card [%s] from player [%d] failed ", card_str(&pOut->rcards[n].card, buf, sizeof(buf)), supply, pOut->rcards[n].pos);
			return R_E_FAIL;
		}
	}
	return R_SUCC;
}

static RESULT add_out_stack(GameContext* pGame, OutCard* pOut)
{
	int n;
	char buf[128];
	if(pOut->nrcard > 0)
	{
		for(n = 0; n < pOut->nrcard; n++)
		{
			if(0 != card_stack_push(&pGame->cardOut, &pOut->rcards[n].card))
			{
				printf("add out card [%s] failed ", card_str(&pOut->rcards[n].card, buf, sizeof(buf)));
				return R_E_FAIL;
			}
		}
	}
	else if(pOut->vcard.card.id != CardID_None)
	{
		if(0 != card_stack_push(&pGame->cardOut, &pOut->vcard.card))
		{
			printf("add out card [%s] failed ", card_str(&pOut->vcard.card, buf, sizeof(buf)));
			return R_E_FAIL;
		}
	}
	return R_SUCC;
}

static RESULT out_card(GameContext* pGame, GameEventContext* pParentEvent, int trigger, int target, int supply, OutCard* pOut)
{
	if(R_CANCEL == per_out_card(pGame, pParentEvent, trigger, target, pOut))
		return R_CANCEL;

	remove_out_card(pGame, supply, pOut);

	post_out_card(pGame, pParentEvent, trigger, target, pOut);

	add_out_stack(pGame, pOut);

	return R_SUCC;
}







/* 
struct CmdLoopCallback
{
	const CardPattern* patterns;
	int num;
	int where;
	int cancel;
	int ret;
};

int f_callback(const char** argv, int argc, GameContext* pGame, void* ud)
{
	CmdLoopCallback* pu = (CmdLoopCallback*)ud;
	
	
	if(!strcmp(argv[0], "out") || !strcmp(argv[0], "o"))
	{

		return CMD_RET_BACK;
	}
	else if(!strcmp(argv[0], "cancel") || !strcmp(argv[0], "c"))
	{
		return CMD_RET_BACK;
	}


	return CMD_RET_DEF;
}

int game_appoint_out(GameContext* pGame, int player, int where, const CardPattern* patterns, int num, int canCancel, const char* alter_text)
{
	CmdLoopCallback  callback;
	callback.patterns = patterns;
	callback.num = num;
	callback.where = where;
	callback.cancel = canCancel;
	callback.ret = 0;

	cmd_loop(pGame, alter_text, f_callback, &callback);
	
	return callback.ret;
}

*/
