#include "config.h"
#include "game.h"
#include "comm.h"
#include "hero.h"
#include "cmd.h"
#include "event.h"
#include "get.h"
#include "stack.h"
#include "discard.h"


//#define NEXT_ROUND(pGame)   ((pGame)->nRoundPlayer = game_next_player((pGame), (pGame)->nRoundPlayer))
//#define NEXT_CUR(pGame)   ((pGame)->nCurPlayer = game_next_player((pGame),(pGame)->nCurPlayer))




Status get_game_status(GameContext* pGame)
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
		MSG_OUT("the current player is set to [%s]\n", CUR_PLAYER(pGame)->name);
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
			pp = (pp - 1 + pGame->nPlayerCount) % pGame->nPlayerCount;
		} while(pp != p2 && IS_PLAYER_DEAD(GAME_PLAYER(pGame, pp)));
	}

	return dis;
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
	char        title[512];
	SelOption   sel_opts[5];
	int         sel_n;


	if(minsters < 1 || spies < 1 || mutineers < 1 || minsters + spies + mutineers + 1 > MAX_PLAYER_NUM)
	{
		MSG_OUT("error player config num!\n");
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
		MSG_OUT("not any valid hero config.\n");
		return R_E_FAIL;
	}


	memset(pGame, 0, sizeof(*pGame));

	pGame->nPlayerCount = minsters + spies + mutineers + 1;
	pGame->nMinsterCount = minsters;
	pGame->nSpyCount = spies;
	pGame->nMutineerCount = mutineers;
	
	MSG_OUT("new game: %d players - [%d master + %d minster + %d spy + %d mutineer]\n", pGame->nPlayerCount, 1, pGame->nMinsterCount, pGame->nSpyCount, pGame->nMutineerCount);

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

		//while(1)
		{
			snprintf(title, sizeof(title), "current player [%d], identification is %s, select hero:", pGame->nCurPlayer, player_id_str((PlayerID)pids[pGame->nCurPlayer]));
			sel_n = 0;
			ST_ZERO(sel_opts);
			for(c = 0; c < hscnt; c++)
			{
				pHero = get_hero_config((HeroID)hids[c]);
				snprintf(sel_opts[sel_n].text, sizeof(sel_opts[sel_n].text), "[%s], %smax life %d", pHero->name, pHero->isMaster ? "[Ö÷¹«], ":"", pHero->life);
				sel_opts[sel_n].value = c+1;
				sel_n++;
			}

			//MSG_OUT("please select (%d-%d): ", 1, hscnt);
			//fflush(stdin);
			//if(1 == scanf("%d", &c) && c >= 1 && c <= hscnt)
			//{
			//	break;
			//}
			if(R_SUCC != select_loop(pGame, NULL, sel_opts, sel_n, title, &c))
				return R_E_FAIL;

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

	init_card_stack(&pGame->stGetCardStack);
	card_stack_random(&pGame->stGetCardStack);

	// clear out cards;
	pGame->stDiscardCardStack.count = 0;
	ST_ZERO(pGame->stCurDiscardCards);
	pGame->nCurDiscardCardNum = 0;


	pGame->nRoundNum = 0;

	// first get 4 cards per player

	game_first_getcard(pGame);


	return R_SUCC;
}




extern "C" {

static  RESULT game_next_round(GameContext* pGame, GameEventContext* pEvent);

static  RESULT game_round_begin(GameContext* pGame, GameEventContext* pEvent)
{
	MSG_OUT("the round [%d] is start, round player is [%s]\n", pGame->nRoundNum, ROUND_PLAYER(pGame)->name);

	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_RoundBegin, pGame->nRoundPlayer, 0, pEvent);


	trigger_game_event(pGame, &event);

	game_flush_discard_cur(pGame);

	return R_SUCC;
}

static RESULT game_round_judge(GameContext* pGame, GameEventContext* pEvent)
{
	MSG_OUT("enter the round [%d]  judgment phase, round player is [%s]\n", pGame->nRoundNum, ROUND_PLAYER(pGame)->name);

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
					game_add_discard_cur(pGame, &stCard);
				}
			}
		}
		else
		{
			MSG_OUT("card config [%d] not found!\n", stCard.id);
		}
		
		game_flush_discard_cur(pGame);
	}

	INIT_EVENT(&event, GameEvent_PostRoundJudge, pGame->nRoundPlayer, 0, pEvent);
	trigger_game_event(pGame, &event);

	return R_SUCC;
}


static RESULT game_round_getcard(GameContext* pGame, GameEventContext* pEvent)
{
	int num;
	GameEventContext  event;

	MSG_OUT("enter the round [%d]  get card phase, round player is [%s]\n", pGame->nRoundNum, ROUND_PLAYER(pGame)->name);


	num = 2;  // in get round init to get 2 card 
	INIT_EVENT(&event, GameEvent_PerRoundGet, pGame->nRoundPlayer, 0, pEvent);
	event.pNum = &num;
	trigger_game_event(pGame, &event);

	game_flush_discard_cur(pGame);


	if(event.result == R_CANCEL)
	{
		// skip getcard step
		return R_SUCC;
	}
	
	game_round_do_get(pGame, pEvent, pGame->nRoundPlayer, num);

	game_flush_discard_cur(pGame);


	INIT_EVENT(&event, GameEvent_PostRoundGet, pGame->nRoundPlayer, 0, pEvent);
	trigger_game_event(pGame, &event);

	game_flush_discard_cur(pGame);

	return R_SUCC;
}

static RESULT game_round_outcard(GameContext* pGame, GameEventContext* pEvent)
{
	GameEventContext  event;

	MSG_OUT("enter the round [%d]  out card phase, round player is [%s]\n", pGame->nRoundNum, ROUND_PLAYER(pGame)->name);

	INIT_EVENT(&event, GameEvent_PerRoundOut, pGame->nRoundPlayer, 0, pEvent);
	trigger_game_event(pGame, &event);
	game_flush_discard_cur(pGame);

	if(event.result == R_CANCEL)
	{
		// skip getcard step
		return R_SUCC;
	}

	while (R_SUCC == game_round_do_out(pGame, pEvent, pGame->nRoundPlayer))
	{
		// do nothing
		game_flush_discard_cur(pGame);
	}

	INIT_EVENT(&event, GameEvent_PostRoundOut, pGame->nRoundPlayer, 0, pEvent);
	trigger_game_event(pGame, &event);
	game_flush_discard_cur(pGame);

	return R_SUCC;
}

static RESULT game_round_discardcard(GameContext* pGame, GameEventContext* pEvent)
{
	GameEventContext  event;

	MSG_OUT("enter the round [%d]  discard card phase, round player is [%s]\n", pGame->nRoundNum, ROUND_PLAYER(pGame)->name);

	// trigger round discard event
	INIT_EVENT(&event, GameEvent_PerRoundDiscard, pGame->nRoundPlayer, 0, pEvent);
	trigger_game_event(pGame, &event);
	game_flush_discard_cur(pGame);

	if(event.result == R_CANCEL)
	{
		// skip getcard step
		return R_SUCC;
	}

	// discard loop
	// game_round_do_discard(pGame, pEvent, player);
	game_flush_discard_cur(pGame);


	// wait cmd_loop discard cmd execute
	INIT_EVENT(&event, GameEvent_PostRoundDiscard, pGame->nRoundPlayer, 0, pEvent);
	trigger_game_event(pGame, &event);
	game_flush_discard_cur(pGame);

	return R_SUCC;
}

static RESULT game_round_end(GameContext* pGame, GameEventContext* pEvent)
{
	// trigger round end event
	GameEventContext  event;

	MSG_OUT("the round [%d] is finish, round player is [%s]\n", pGame->nRoundNum, ROUND_PLAYER(pGame)->name);


	INIT_EVENT(&event, GameEvent_RoundEnd, pGame->nRoundPlayer, 0, pEvent);

	trigger_game_event(pGame, &event);
	game_flush_discard_cur(pGame);

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


	MSG_OUT("next round: num [%d], round player is set to: %d, [%s]\n", pGame->nRoundNum, pGame->nRoundPlayer, ROUND_PLAYER(pGame)->name);

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
	default: break;
	}
	return R_E_STATUS;
}


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


extern "C" {

RESULT game_main(GameContext* pGame, GameEventContext* pEvent)
{
	//return script_call_c(pGame->script, game_loop, pGame, pEvent, 0);
	return R_SUCC;
}



}


#define TAB_N(n)   ("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t"+(((n)<16)?(16-(n)):0))

static int fprintf_tab(FILE* pf, int tabs, const char* fmt, ...) __ATTR_FMT__(printf,3,4);

static int fprintf_tab(FILE* pf, int tabs, const char* fmt, ...) 
{
	int n;
	va_list vl;
	while(tabs-->0) { fputc('\t', pf); }
	va_start(vl, fmt);
	n = vfprintf(pf, fmt, vl); 
	va_end(vl);
	return n + tabs;
}

static void game_save_card(Card* pCard, FILE* file, int tabs)
{
	fprintf_tab(file, tabs, "id = %d,\n", pCard->id);
	fprintf_tab(file, tabs, "color = %d,\n", pCard->color);
	fprintf_tab(file, tabs, "value = %d,\n", pCard->value);
	fprintf_tab(file, tabs, "flag = %d,\n", pCard->flag);

}


static void game_save_cardstack(CardStack* pCardStack, FILE* file, int tabs)
{
	int n;
	fprintf_tab(file, tabs, "count = %d,\n", pCardStack->count);

	fprintf_tab(file, tabs, "cards = {\n");
	for(n = 0; n < pCardStack->count; n++)
	{
		fprintf_tab(file, tabs+1, "{\n");
		game_save_card(&pCardStack->cards[n], file, tabs+2);
		fprintf_tab(file, tabs+1, "},\n");
	}
	fprintf_tab(file, tabs, "},\n");
}



static void game_save_player(Player* pPlayer, FILE* file, int tabs)
{
	int n;
	fprintf_tab(file, tabs, "id = %d,\n", pPlayer->id);
	fprintf_tab(file, tabs, "hero = %d,\n", pPlayer->hero);
	fprintf_tab(file, tabs, "maxLife = %d,\n", pPlayer->maxLife);
	fprintf_tab(file, tabs, "curLife = %d,\n", pPlayer->curLife);
	fprintf_tab(file, tabs, "name = \'%s\',\n", pPlayer->name);
	fprintf_tab(file, tabs, "nHandCardNum = %d,\n", pPlayer->nHandCardNum);
	fprintf_tab(file, tabs, "nJudgmentCardNum = %d,\n", pPlayer->nJudgmentCardNum);

	fprintf_tab(file, tabs, "stHandCards = {\n");
	for(n = 0; n < pPlayer->nHandCardNum; n++)
	{
		fprintf_tab(file, tabs+1, "{\n");
		game_save_card(&pPlayer->stHandCards[n], file, tabs+2);
		fprintf_tab(file, tabs+1, "},\n");
	}
	fprintf_tab(file, tabs, "},\n");


	fprintf_tab(file, tabs, "stEquipCard = {\n");
	for(n = 0; n < EquipIdx_Max; n++)
	{
		fprintf_tab(file, tabs+1, "{\n");
		game_save_card(&pPlayer->stEquipCard[n], file, tabs+2);
		fprintf_tab(file, tabs+1, "},\n");
	}
	fprintf_tab(file, tabs, "},\n");

	fprintf_tab(file, tabs, "stJudgmentCards = {\n");
	for(n = 0; n < pPlayer->nJudgmentCardNum; n++)
	{
		fprintf_tab(file, tabs+1, "{\n");
		game_save_card(&pPlayer->stJudgmentCards[n], file, tabs+2);
		fprintf_tab(file, tabs+1, "},\n");
	}
	fprintf_tab(file, tabs, "},\n");

	fprintf_tab(file, tabs, "status = %d,\n", pPlayer->status);
	fprintf_tab(file, tabs, "flag = %d,\n", pPlayer->flag);

	fprintf_tab(file, tabs, "params = {\n");
	for(n = 0; n < MAX_PLAYER_PARAM; n++)
	{
		fprintf_tab(file, tabs+1, "%d,\n", pPlayer->params[n]);
	}
	fprintf_tab(file, tabs, "},\n");
}


// save game context to file
RESULT game_save(GameContext* pGame, const char* file_name)
{
	int n;
	int tabs;
	FILE* file;

	file = fopen(file_name, "wb");

	if(file == NULL)
	{
		MSG_OUT("open file \"%s\" failed: (%d) %s\n", file_name, errno, strerror(errno));
		return R_E_FAIL;
	}

	tabs = 1;

	fprintf_tab(file, 0, "{\n");
	fprintf_tab(file, tabs, "nPlayerCount = %d,\n", pGame->nPlayerCount);
	fprintf_tab(file, tabs, "nMinsterCount = %d,\n", pGame->nMinsterCount);
	fprintf_tab(file, tabs, "nSpyCount = %d,\n", pGame->nSpyCount);
	fprintf_tab(file ,tabs, "nMutineerCount = %d,\n", pGame->nMutineerCount);
	fprintf_tab(file, tabs, "players = {\n");

	for(n = 0; n < pGame->nPlayerCount; n++)
	{
		fprintf_tab(file, tabs+1, "{\n");

		game_save_player(&pGame->players[n], file, tabs+2);
		
		fprintf_tab(file, tabs+1, "},\n");
	}

	fprintf_tab(file, tabs, "},\n");

	fprintf_tab(file, tabs, "stGetCardStack = {\n");
	game_save_cardstack(&pGame->stGetCardStack, file, tabs+1);
	fprintf_tab(file, tabs, "},\n");

	fprintf_tab(file, tabs, "stDiscardCardStack = {\n");
	game_save_cardstack(&pGame->stDiscardCardStack, file,  tabs+1);
	fprintf_tab(file, tabs, "},\n");

	fprintf_tab(file, tabs, "nCurDiscardCardNum = %d,\n", pGame->nCurDiscardCardNum);

	fprintf_tab(file, tabs, "stCurDiscardCards = {\n");
	for(n = 0; n < pGame->nCurDiscardCardNum; n++)
	{
		fprintf_tab(file, tabs+1, "{\n");

		game_save_card(&pGame->stCurDiscardCards[n], file, tabs+2);

		fprintf_tab(file, tabs+1, "},\n");
	}
	fprintf_tab(file, tabs, "},\n");

	fprintf_tab(file, tabs, "nRoundNum = %d,\n", pGame->nRoundNum);
	fprintf_tab(file, tabs, "nRoundPlayer = %d,\n", pGame->nRoundPlayer);
	fprintf_tab(file, tabs, "nCurPlayer = %d,\n", pGame->nCurPlayer);
	fprintf_tab(file, tabs, "status = %d,\n", pGame->status);


	fprintf_tab(file, 0, "},\n");

	fclose(file);

	return R_SUCC;
}


RESULT game_load(GameContext* pGame, const char* file_name)
{
	FILE* file;

	file = fopen(file_name, "rb");

	if(file == NULL)
	{
		MSG_OUT("open file \"%s\" failed: (%d) %s\n", file_name, errno, strerror(errno));
		return R_E_FAIL;
	}


	fclose(file);
	return R_SUCC;
}



