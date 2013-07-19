#include "config.h"
#include "game.h"
#include "comm.h"
#include "hero.h"
#include "cmd.h"
#include "event.h"
#include "get.h"
#include "stack.h"
#include "discard.h"
#include "script.h"


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


Player* get_game_player(GameContext* pGame, int player)
{
	return (player >= 0 && player < pGame->nPlayerCount) ? &pGame->players[player] : NULL;
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


static void game_init_getstack(CardStack* pCardStack)
{
	lua_State* L = get_game_script();
	lua_getglobal(L, "init_card_stack");
	tolua_pushusertype(L, pCardStack, "CardStack");
	lua_call(L, 1, 0);
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

	// 这里不能清空。因为这Game->L已经赋值，外面已经清除
	//memset(pGame, 0, sizeof(*pGame));

	pGame->nPlayerCount = minsters + spies + mutineers + 1;
	pGame->nMinsterCount = minsters;
	pGame->nSpyCount = spies;
	pGame->nMutineerCount = mutineers;
	
	MSG_OUT("new game: %d players - (%d master + %d minster + %d spy + %d mutineer)\n", pGame->nPlayerCount, 1, pGame->nMinsterCount, pGame->nSpyCount, pGame->nMutineerCount);

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
				snprintf(sel_opts[sel_n].text, sizeof(sel_opts[sel_n].text), "[%s], %smax life %d", pHero->name, pHero->isMaster ? "[主公], ":"", pHero->life);
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

	game_init_getstack(&pGame->stGetCardStack);
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
	PosCard stCard;
	//const CardConfig* pCardConfig;


	while(pPlayer->nJudgmentCardNum > 0)
	{
		stCard.card = pPlayer->stJudgmentCards[pPlayer->nJudgmentCardNum-1];
		stCard.where = CardWhere_PlayerJudgment;
		stCard.pos = pPlayer->nJudgmentCardNum-1;
		pPlayer->nJudgmentCardNum--;

		//pCardConfig = get_card_config(stCard.id);

		//if(pCardConfig)
		{
			INIT_EVENT(&event, GameEvent_PerCardCalc, pGame->nRoundPlayer, 0, pEvent);
			event.pPosCard = &stCard;
			trigger_game_event(pGame, &event);

			if(event.result != R_CANCEL) // if card calc is cancel .
			{
				//if(pCardConfig->out != NULL)
				{
					INIT_EVENT(&event, GameEvent_CardCalc, pGame->nRoundPlayer, 0, pEvent);
					event.pPosCard = &stCard;
					//(*pCardConfig->out)(pGame, &event, pGame->nCurPlayer);
					card_out_call(stCard.card.id, pGame, &event, pGame->nCurPlayer);
				}

				INIT_EVENT(&event, GameEvent_PostCardCalc, pGame->nRoundPlayer, 0, pEvent);
				event.pPosCard = &stCard;
				trigger_game_event(pGame, &event);

			}

			// after calc
			if(stCard.card.id != CardID_None)
			{
				//if(pCardConfig->out != NULL)
				{
					INIT_EVENT(&event, GameEvent_FiniCardCalc, pGame->nRoundPlayer, 0, pEvent);
					event.pPosCard = &stCard;
					//(*pCardConfig->out)(pGame, &event, pGame->nCurPlayer);
					card_out_call(stCard.card.id, pGame, &event, pGame->nCurPlayer);
				}
				//else
				//{
				//	// after calc discard card (default fini action)?
				//	game_add_discard_cur(pGame, &stCard);
				//}
			}
		}
		//else
		//{
		//	MSG_OUT("card config [%d] not found!\n", stCard.id);
		//}
		
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
	case Status_NewGame: // 直接跳到下一个状态
		return R_SUCC;
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
static RESULT game_loop(GameContext* pGame, GameEventContext* pEvent)
{
	RESULT ret = R_SUCC;
	while(ret == R_SUCC)
	{
		ret = game_step(pGame, pEvent);
		if(ret == R_SUCC)
		{
			ret = game_next_status(pGame, pEvent);
		}
	}
	return ret;
}


static int lua_game_main(lua_State* L)
{
	RESULT  ret;
	GameContext* pGame = (GameContext*)lua_touserdata(L, 1);
	GameEventContext* pEvent = (GameEventContext*)lua_touserdata(L, 2);

	ret = game_loop(pGame, pEvent);
	lua_pushnumber(L, ret);
	return 1;
}




static int lua_game_init(lua_State* L)
{
	RESULT  ret;
	GameContext* pGame = (GameContext*)lua_touserdata(L, 1);
	GameEventContext* pEvent = (GameEventContext*)lua_touserdata(L, 2);

	switch(pEvent->id)
	{
	case GameEvent_NewGame:
		ret = init_game_context(pGame, pEvent->pNewGameConfig->minsters, pEvent->pNewGameConfig->spies, pEvent->pNewGameConfig->mutineers);
		break;
	case GameEvent_LoadGame:
		ret = game_load(pGame, pEvent->szFileName);
		break;
	default:
		ret = R_E_PARAM;
		break;
	}

	lua_pushinteger(L, ret);
	return 1;
}

RESULT game_main(GameContext* pGame, GameEventContext* pEvent)
{
	int     state;
	RESULT  ret;
	lua_State* L = get_game_script();

	do{

		memset(pGame, 0, sizeof(*pGame));


		lua_pushcfunction(L, lua_game_init);
		lua_pushlightuserdata(L, pGame);
		lua_pushlightuserdata(L, pEvent);

		state = lua_pcall(L, 2, 1, 0);

		if(state != 0)
		{
			MSG_OUT("init game failed: %s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
			ret = R_ABORT;
			break;
		}


		ret = (RESULT)lua_tointeger(L, -1);

		lua_pop(L, 1);


		if(ret != R_SUCC)
		{
			//ret = R_ABORT;
			break;
		}


		lua_pushcfunction(L, lua_game_main);
		lua_pushlightuserdata(L, pGame);
		lua_pushlightuserdata(L, pEvent);

		state = lua_pcall(L, 2, 1, 0);

		if(state != 0)
		{
			MSG_OUT("Game Abort: %s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
			ret = R_ABORT;
			break;
		}


		ret = (RESULT)lua_tointeger(L, -1);

		lua_pop(L, 1);
	}
	while(0);

	pGame->status = Status_None;

	//memset(pGame, 0, sizeof(*pGame));

	// 	ret = (RESULT)setjmp(pGame->__jb__);
	// 	if(ret== 0)
	// 	{
	// 		ret = game_loop(pGame, pEvent);
	// 	}

	return ret;
}




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


static const char* game_status_id_str(Status s)
{
	switch(s)
	{
	case Status_None: return  "Status_None";
	case Status_NewGame: return  "Status_NewGame";
	case Status_Round_Begin: return  "Status_Round_Begin";
	case Status_Round_Judge: return  "Status_Round_Judge";
	case Status_Round_Get: return  "Status_Round_Get";
	case Status_Round_Out: return  "Status_Round_Out";
	case Status_Round_Discard: return  "Status_Round_Discard";
	case Status_Round_End: return  "Status_Round_End";
	default: return "-1";
	}
}

const char* player_id_id_str(PlayerID id)
{
	switch(id)
	{
	case PlayerID_Unknown: return "PlayerID_Unknown";
	case PlayerID_None: return "PlayerID_None";
	case PlayerID_Master: return "PlayerID_Master";
	case PlayerID_Minster: return "PlayerID_Minster";
	case PlayerID_Spy: return "PlayerID_Spy";
	case PlayerID_Mutineer: return "PlayerID_Mutineer";
	default: return "PlayerID_Unknown";
	}
}

const char* card_color_id_str(CardColor color)
{
	switch(color)
	{
	case CardColor_Unknown: return "CardColor_Unknown";
	case CardColor_None: return "CardColor_None"; 
	case CardColor_Spade: return "CardColor_Spade";
	case CardColor_Club: return "CardColor_Club";
	case CardColor_Heart: return "CardColor_Heart";
	case CardColor_Diamond : return "CardColor_Diamond";
	case CardColor_GeneralBlack: return "CardColor_GeneralBlack";
	case CardColor_GeneralRed: return "CardColor_GeneralRed";
	default: return "CardColor_Unknown";
	};
}

const char* card_value_id_str(CardValue value)
{
	switch(value)
	{
	case CardValue_Unknown: return "CardValue_Unknown";
	case CardValue_None: return "CardValue_None";
	case CardValue_2: return "CardValue_2";
	case CardValue_3: return "CardValue_3";
	case CardValue_4: return "CardValue_4";
	case CardValue_5: return "CardValue_5";
	case CardValue_6: return "CardValue_6";
	case CardValue_7: return "CardValue_7";
	case CardValue_8: return "CardValue_8";
	case CardValue_9: return "CardValue_9";
	case CardValue_10: return "CardValue_10";
	case CardValue_J: return "CardValue_J";
	case CardValue_Q: return "CardValue_Q";
	case CardValue_K: return "CardValue_K";
	case CardValue_A: return "CardValue_A";
	default: return "CardValue_Unknown";
	}
}


static void game_save_card(Card* pCard, FILE* file, int tabs)
{
	char  temp[128];
	fprintf_tab(file, 0, "sid = \'%s\', ", card_sid(pCard->id, temp, sizeof(temp)));
	fprintf_tab(file, 0, "color = %s, ", card_color_id_str(pCard->color));
	fprintf_tab(file, 0, "value = %s, ", card_value_id_str(pCard->value));
	fprintf_tab(file, 0, "flag = 0x%x, ", pCard->flag);

}


static void game_save_cardstack(CardStack* pCardStack, FILE* file, int tabs)
{
	int n;
	fprintf_tab(file, tabs, "count = %d,\n", pCardStack->count);

	fprintf_tab(file, tabs, "cards = {\n");
	for(n = 0; n < pCardStack->count; n++)
	{
		fprintf_tab(file, tabs+1, "{ ");
		game_save_card(&pCardStack->cards[n], file, 0);
		fprintf_tab(file, 0, "},\n");
	}
	fprintf_tab(file, tabs, "},\n");
}



static void game_save_player(Player* pPlayer, FILE* file, int tabs)
{
	int n;
	fprintf_tab(file, tabs, "id = %s,\n", player_id_id_str(pPlayer->id));
	fprintf_tab(file, tabs, "hero = %d,\n", pPlayer->hero);
	fprintf_tab(file, tabs, "maxLife = %d,\n", pPlayer->maxLife);
	fprintf_tab(file, tabs, "curLife = %d,\n", pPlayer->curLife);
	fprintf_tab(file, tabs, "name = \'%s\',\n", pPlayer->name);
	fprintf_tab(file, tabs, "nHandCardNum = %d,\n", pPlayer->nHandCardNum);
	fprintf_tab(file, tabs, "nJudgmentCardNum = %d,\n", pPlayer->nJudgmentCardNum);

	fprintf_tab(file, tabs, "stHandCards = {\n");
	for(n = 0; n < pPlayer->nHandCardNum; n++)
	{
		fprintf_tab(file, tabs+1, "{ ");
		game_save_card(&pPlayer->stHandCards[n], file, 0);
		fprintf_tab(file, 0, "},\n");
	}
	fprintf_tab(file, tabs, "},\n");


	fprintf_tab(file, tabs, "stEquipCard = {\n");
	for(n = 0; n < EquipIdx_Max; n++)
	{
		fprintf_tab(file, tabs+1, "{ ");
		game_save_card(&pPlayer->stEquipCard[n], file, 0);
		fprintf_tab(file, 0, "},\n");
	}
	fprintf_tab(file, tabs, "},\n");

	fprintf_tab(file, tabs, "stJudgmentCards = {\n");
	for(n = 0; n < pPlayer->nJudgmentCardNum; n++)
	{
		fprintf_tab(file, tabs+1, "{ ");
		game_save_card(&pPlayer->stJudgmentCards[n], file, 0);
		fprintf_tab(file, 0, "},\n");
	}
	fprintf_tab(file, tabs, "},\n");

	fprintf_tab(file, tabs, "status = %d,\n", (pPlayer->status));
	fprintf_tab(file, tabs, "flag = %d,\n", pPlayer->flag);

	fprintf_tab(file, tabs, "params = {");
	for(n = 0; n < MAX_PLAYER_PARAM; n++)
	{
		fprintf_tab(file, 0, " %d,", pPlayer->params[n]);
	}
	fprintf_tab(file, 0, " },\n");
}


// save game context to file
RESULT game_save(GameContext* pGame, const char* file_name)
{
	int n;
	int tabs;
	FILE* file;
	char  base[MAX_PATH];
	char  path[MAX_PATH];


	snprintf(path, sizeof(path), "%s/sav/%s.sav", get_app_path(base, sizeof(base)), file_name);

	file = fopen(path, "wb");

	if(file == NULL)
	{
		MSG_OUT("open file \"%s\" error: (%d) %s\n", path, errno, strerror(errno));
		return R_E_FAIL;
	}

	tabs = 1;

	fprintf_tab(file, 0, "game = {\n");
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
		fprintf_tab(file, tabs+1, "{ ");
		game_save_card(&pGame->stCurDiscardCards[n], file, 0);
		fprintf_tab(file, tabs+1, "},\n");
	}
	fprintf_tab(file, tabs, "},\n");

	fprintf_tab(file, tabs, "nRoundNum = %d,\n", pGame->nRoundNum);
	fprintf_tab(file, tabs, "nRoundPlayer = %d,\n", pGame->nRoundPlayer);
	fprintf_tab(file, tabs, "nCurPlayer = %d,\n", pGame->nCurPlayer);
	fprintf_tab(file, tabs, "status = %s,\n", game_status_id_str(pGame->status));

	fprintf_tab(file, 0, "};\n");

	fclose(file);

	return R_SUCC;
}



#define LOAD_INT(p,field,L)  do{ \
	lua_getfield(L, -1, #field); \
	if(lua_isnoneornil(L, -1)) {\
		luaL_error(L, "attemp to get field \"%s\", a nil value!", #field);\
	} else if(!lua_isnumber(L, -1)){ \
		luaL_error(L, "attemp to get field \"%s\", not a number!", #field);\
	} \
	(p)->field = lua_tointeger(L, -1); \
	lua_pop(L, 1); \
} while(0)

#define LOAD_INT_CAST(p,field,L,t)  do{ \
	lua_getfield(L, -1, #field); \
	if(lua_isnoneornil(L, -1)) {\
		luaL_error(L, "attemp to get field \"%s\", a nil value!", #field);\
	} else if(!lua_isnumber(L, -1)){ \
		luaL_error(L, "attemp to get field \"%s\", not a number!", #field);\
	} \
	(p)->field = (t)lua_tointeger(L, -1); \
	lua_pop(L, 1); \
} while(0)

#define LOAD_STRING(p,field,L)  do{ \
	lua_getfield(L, -1, #field); \
	strncpy((p)->field, lua_tostring(L, -1), sizeof((p)->field)); \
	lua_pop(L, 1); \
} while(0)


void game_load_card(lua_State* L, Card* pCard)
{
	//LOAD_INT_CAST(pCard, id, L, CardID);
	lua_getfield(L, -1, "sid");
	pCard->id = card_sid2id(lua_tostring(L, -1));
	lua_pop(L, 1);
	LOAD_INT_CAST(pCard, color, L, CardColor);
	LOAD_INT_CAST(pCard, value, L, CardValue);
	LOAD_INT_CAST(pCard, flag, L, CardFlag);
}

void game_load_cardstack(lua_State* L, CardStack* pCardStack)
{
	int n;
	LOAD_INT(pCardStack, count, L);

	if(pCardStack->count > CARD_STACK_SIZE)
	{
		luaL_error(L, "error card stack count!");
	}

	lua_getfield(L, -1, "cards"); // ... [cardstack] [cardstack.cards]
	for(n = 0; n < pCardStack->count; n++)
	{
		lua_pushnumber(L, n+1);
		lua_gettable(L, -2);
		game_load_card(L, &pCardStack->cards[n]);
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
}


void game_load_player(lua_State* L, Player* pPlayer)
{
	int n;

	LOAD_INT_CAST(pPlayer, id, L, PlayerID);
	LOAD_INT_CAST(pPlayer, hero, L, HeroID);
	LOAD_INT(pPlayer, maxLife, L);
	LOAD_INT(pPlayer, curLife, L);
	LOAD_STRING(pPlayer, name, L);
	LOAD_INT(pPlayer, nHandCardNum, L);
	LOAD_INT(pPlayer, nJudgmentCardNum, L);

	if(pPlayer->nHandCardNum > MAX_HAND_CARD)
	{
		luaL_error(L, "error player hand card count!");
	}
	if(pPlayer->nJudgmentCardNum > MAX_JUDGMENT_CARD)
	{
		luaL_error(L, "error player judgment card count!");
	}

	lua_getfield(L, -1, "stHandCards");
	for(n = 0; n < pPlayer->nHandCardNum; n++)
	{
		lua_pushnumber(L, n+1);
		lua_gettable(L, -2);
		game_load_card(L, &pPlayer->stHandCards[n]);		
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	lua_getfield(L, -1, "stEquipCard");
	for(n = 0; n < EquipIdx_Max; n++)
	{
		lua_pushnumber(L, n+1);
		lua_gettable(L, -2);
		game_load_card(L, &pPlayer->stEquipCard[n]);
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	lua_getfield(L, -1, "stJudgmentCards");
	for(n = 0; n < pPlayer->nJudgmentCardNum; n++)
	{
		lua_pushnumber(L, n+1);
		lua_gettable(L, -2);
		game_load_card(L, &pPlayer->stJudgmentCards[n]);		
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	LOAD_INT_CAST(pPlayer, status, L, PlayerStatus);
	LOAD_INT_CAST(pPlayer, flag, L, PlayerFlag);

	lua_getfield(L, -1, "params");
	for(n = 0; n < MAX_PLAYER_PARAM; n++)
	{
		lua_pushnumber(L, n+1);
		lua_gettable(L, -2);
		pPlayer->params[n] = lua_tointeger(L, -1);
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
}

static int lua_game_load(lua_State* L)
{
	int top;
	int n;
	GameContext* pGame;
	const char* file_name;

	pGame = (GameContext*)lua_touserdata(L, 1);
	file_name = luaL_checkstring(L, 2);


	//ret = R_SUCC;

	top = lua_gettop(L);

	// do {
		lua_newtable(L);  // [t]
		lua_pushvalue(L, -1);  // [t] [t]
		lua_pushstring(L, "__index");
		lua_pushvalue(L, LUA_GLOBALSINDEX);  // [t] [t] ['__index'] [_G]
		lua_rawset(L, -3);   // [t] [t]
		lua_setmetatable(L, -2);  // [t]

		if(0 != luaL_loadfile(L, file_name))   // [t] [f]
		{
			luaL_error(L, "%s", lua_tostring(L, -1));
		}

		lua_pushvalue(L, -2);   // [t] [f] [t]
		lua_setfenv(L, -2);     // [t] [f]

		lua_call(L, 0, 0);

		//stack:  [t] 
		

		lua_getfield(L, -1, "game"); // [t] [t.game]
		if(!lua_istable(L, -1))
		{
			luaL_error(L, "global define 'game' is not found!");
		}
		
		// do not clear game, because the game is already inited with L
		//memset(pGame, 0, sizeof(*pGame));

		// start load...
		LOAD_INT(pGame, nPlayerCount, L);
		LOAD_INT(pGame, nMinsterCount, L);
		LOAD_INT(pGame, nSpyCount, L);
		LOAD_INT(pGame, nMutineerCount, L);

		if(pGame->nPlayerCount > MAX_PLAYER_NUM)
		{
			luaL_error(L, "error player count!");
		}
	
		// players
		lua_getfield(L, -1, "players"); // [t] [t.game] [t.game.players] 

		//if(!lua_istable(L, -1))
		//{
		//	MSG_OUT("in 'game' table, sub table 'players' is not found!\n");
		//	ret = R_E_FAIL;
		//	break;
		//}

		for(n = 0; n < pGame->nPlayerCount; n++)
		{
			lua_pushnumber(L, n+1);
			lua_gettable(L, -2);   // [t] [t.game] [t.game.players] [t.game.players[n+1]]
			game_load_player(L, &pGame->players[n]);
			lua_pop(L, 1);     // [t] [t.game] [t.game.players] 
		}

		lua_pop(L, 1);  // [t] [t.game]

		// get stack
		lua_getfield(L, -1, "stGetCardStack"); // [t] [t.game] [t.game.stGetCardStack]
		game_load_cardstack(L, &pGame->stGetCardStack);
		lua_pop(L, 1); // [t] [t.game]

		// get stack
		lua_getfield(L, -1, "stDiscardCardStack"); // [t] [t.game] [t.game.stDiscardCardStack]
		game_load_cardstack(L, &pGame->stDiscardCardStack);
		lua_pop(L, 1);  // [t] [t.game]

		LOAD_INT(pGame, nCurDiscardCardNum, L);
		if(pGame->nCurDiscardCardNum > MAX_CUR_DISCARD_NUM)
		{
			luaL_error(L, "error nCurDiscardCardNum!");
		}

		lua_getfield(L, -1, "stCurDiscardCards");  // [t] [t.game] [t.game.stCurDiscardCards]
		for(n = 0; n < pGame->nCurDiscardCardNum; n++)
		{
			lua_pushnumber(L, n+1);
			lua_gettable(L, -2);   // [t] [t.game] [t.game.stCurDiscardCards] [t.game.stCurDiscardCards[n+1]]
			game_load_card(L, &pGame->stCurDiscardCards[n]);
			lua_pop(L, 1);    // [t] [t.game] [t.game.stCurDiscardCards]
		}
		lua_pop(L, 1);    // [t] [t.game] 

		LOAD_INT(pGame, nRoundNum, L);
		LOAD_INT(pGame, nRoundPlayer, L);
		LOAD_INT(pGame, nCurPlayer, L);
		LOAD_INT_CAST(pGame, status, L, Status);

		lua_pop(L, 1);   // [t]  

		lua_pop(L, 1);   
	//} while(0);

	lua_settop(L, top);

	return 0;
}

RESULT game_load(GameContext* pGame, const char* file_name)
{
	int state;
	lua_State* L;
	char  base[MAX_PATH];
	char  path[MAX_PATH];


	snprintf(path, sizeof(path), "%s/sav/%s.sav", get_app_path(base, sizeof(base)), file_name);

	L = get_game_script();

	if(L == NULL)
		return R_E_PARAM;

	
	lua_pushcfunction(L, lua_game_load);
	lua_pushlightuserdata(L, pGame);
	lua_pushstring(L, path);

	state = lua_pcall(L, 2, 0, 0);

	if(state != 0)
	{
		MSG_OUT("load game from \"%s\" failed: \n%s\n", path, lua_tostring(L, -1));
		lua_pop(L, 1);
		return R_E_FAIL;
	}
	
	return R_SUCC;
}



// if the game is over, this function do not return.
void game_check_gameover(GameContext* pGame, int player)
{

	// TODO: not yet implements
}



