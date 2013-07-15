#include "config.h"
#include "game.h"
#include "comm.h"
#include "hero.h"
#include "cmd.h"
#include "event.h"
#include "get.h"
#include "stack.h"
#include "discard.h"
#include "../pkg/lua_export.h"


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
	GameEventContext* pEvent = (GameEventContext*)lua_touserdata(L, 1);

	ret = game_loop(pGame, pEvent);
	lua_pushnumber(L, ret);
	return 1;
}

#define REPLACE_CHAR(str, chfrom, chto)  do { char* p = (str); while(*p) { if(*p == (chfrom) ) { *p = (chto); } p++; } } while(0)
#define REGULAR_PATH(str)  REPLACE_CHAR((str),'\\', '/')
#define WIN_PATH(str)  REPLACE_CHAR((str),'/', '\\')


char* get_path_name(const char* file_path, char* buf, int buflen)
{
	char*  last_sls;

	if(file_path != buf)
	{
		strncpy(buf, file_path, buflen);
	}

	last_sls = strrchr(buf, '/');
	if(last_sls) 
		*last_sls = '\0';

	return buf;
}


char* get_app_path(char*  buf, int buflen)
{
#ifdef WIN32
	GetModuleFileName(NULL, buf, buflen);
#elif defined(LINUX)
	readlink("/proc/self/exe", buf, buflen);
#endif
	REGULAR_PATH(buf);


	return get_path_name(buf, buf, buflen);
}

char* get_cur_path(char*  buf, int buflen)
{
	if(NULL == getcwd(buf, buflen))
		return NULL;

	REGULAR_PATH(buf);

	return buf;
}

int set_cur_path(const char* path)
{
#ifdef WIN32
	char  win_path[MAX_PATH];
	strncpy(win_path, path, sizeof(win_path));
	WIN_PATH(win_path);
	path = win_path;
#endif
	return chdir(path);
}


RESULT game_main_prepare(lua_State* L)
{
	int   state;
	char  base_path[MAX_PATH];

	get_app_path(base_path, sizeof(base_path));

	set_cur_path(base_path);

	// load lua base libraries
	luaL_openlibs(L);
	// load game core libraries
	tolua_game_open(L);
	
	// load lua files
	lua_pushstring(L, base_path);
	lua_setfield(L, LUA_REGISTRYINDEX, "__import_path");
	lua_getglobal(L, "import");
	lua_pushstring(L, "./script/main.lua");
	state = lua_pcall(L, 1, 0, 0);
	if(state != 0)
	{
		MSG_OUT("%s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return R_ABORT;
	}
	
	lua_pushnil(L);
	lua_setfield(L, LUA_REGISTRYINDEX, "__import_path");

	return R_SUCC;
}

RESULT game_main(GameContext* pGame, GameEventContext* pEvent)
{
	int     state;
	RESULT  ret;
	lua_State* L;

	L = lua_open();

	if(L == NULL)
	{
		MSG_OUT("Create lua script engine for game logic failed!\n");
		return R_ABORT;
	}

	do{


		ret = game_main_prepare(L);
		if(ret != R_SUCC)
		{
			ret = R_ABORT;
			break;
		}


		pGame->L = L;

		lua_pushcfunction(L, lua_game_main);
		lua_pushlightuserdata(L, pGame);
		lua_pushlightuserdata(L, pEvent);

		state = lua_pcall(L, 2, 1, 0);

		if(state != 0)
		{
			MSG_OUT("%s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
			ret = R_ABORT;
			break;
		}


		ret = (RESULT)lua_tointeger(L, -1);

		lua_pop(L, 1);
	}
	while(0);

	pGame->L = NULL;

	memset(pGame, 0, sizeof(*pGame));

	lua_close(L);

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

static void game_save_card(Card* pCard, FILE* file, int tabs)
{
	fprintf_tab(file, 0, "id = %2d, ", pCard->id);
	fprintf_tab(file, 0, "color = %2d, ", pCard->color);
	fprintf_tab(file, 0, "value = %2d, ", pCard->value);
	fprintf_tab(file, 0, "flag = 0x%08x, ", pCard->flag);

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

	fprintf_tab(file, tabs, "status = %d,\n", pPlayer->status);
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
	fprintf_tab(file, tabs, "status = %d,\n", pGame->status);


	fprintf_tab(file, 0, "};\n");

	fclose(file);

	return R_SUCC;
}


#define LOAD_FIELD(p,field,L,fn)  \
	lua_getfield(L, -1, #field); \
	(p)->field = fn(L, -1); \
	lua_pop(L, 1);

#define LOAD_STRING(p,field,L)  \
	lua_getfield(L, -1, #field); \
	strncpy((p)->field, lua_tostring(L, -1), sizeof((p)->field)); \
	lua_pop(L, 1);


RESULT game_load_card(Card* pCard, lua_State* L)
{
	LOAD_FIELD(pCard, id, L, (CardID)lua_tointeger);
	LOAD_FIELD(pCard, color, L, (CardColor)lua_tointeger);
	LOAD_FIELD(pCard, value, L, (CardValue)lua_tointeger);
	LOAD_FIELD(pCard, flag, L, (CardFlag)lua_tointeger);
	return R_SUCC;
}

RESULT game_load_cardstack(CardStack* pCardStack, lua_State* L)
{
	int n;
	LOAD_FIELD(pCardStack, count, L, lua_tointeger);

	if(pCardStack->count > CARD_STACK_SIZE)
	{
		MSG_OUT("error card stack count!\n");
		return R_E_FAIL;
	}

	lua_getfield(L, -1, "cards");
	for(n = 0; n < pCardStack->count; n++)
	{
		lua_pushnumber(L, n+1);
		lua_gettable(L, -2);
		
		if(R_SUCC != game_load_card(&pCardStack->cards[n], L))
			return R_E_FAIL;

		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	return R_SUCC;
}


RESULT game_load_player(Player* pPlayer, lua_State* L)
{
	int n;

	LOAD_FIELD(pPlayer, id, L, (PlayerID)lua_tointeger);
	LOAD_FIELD(pPlayer, hero, L, (HeroID)lua_tointeger);
	LOAD_FIELD(pPlayer, maxLife, L, lua_tointeger);
	LOAD_FIELD(pPlayer, curLife, L, lua_tointeger);
	LOAD_STRING(pPlayer, name, L);
	LOAD_FIELD(pPlayer, nHandCardNum, L, lua_tointeger);
	LOAD_FIELD(pPlayer, nJudgmentCardNum, L, lua_tointeger);

	if(pPlayer->nHandCardNum > MAX_HAND_CARD)
	{
		MSG_OUT("error player hand card count!\n");
		return R_E_FAIL;
	}
	if(pPlayer->nJudgmentCardNum > MAX_JUDGMENT_CARD)
	{
		MSG_OUT("error player judgment card count!\n");
		return R_E_FAIL;
	}


	lua_getfield(L, -1, "stHandCards");
	for(n = 0; n < pPlayer->nHandCardNum; n++)
	{
		lua_pushnumber(L, n+1);
		lua_gettable(L, -2);
		
		if(R_SUCC != game_load_card(&pPlayer->stHandCards[n], L))
			return R_E_FAIL;
		
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	lua_getfield(L, -1, "stEquipCard");
	for(n = 0; n < EquipIdx_Max; n++)
	{
		lua_pushnumber(L, n+1);
		lua_gettable(L, -2);
		
		if(R_SUCC != game_load_card(&pPlayer->stEquipCard[n], L))
			return R_E_FAIL;
		
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	lua_getfield(L, -1, "stJudgmentCards");
	for(n = 0; n < pPlayer->nJudgmentCardNum; n++)
	{
		lua_pushnumber(L, n+1);
		lua_gettable(L, -2);
		
		if(R_SUCC != game_load_card(&pPlayer->stJudgmentCards[n], L))
			return R_E_FAIL;
		
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	LOAD_FIELD(pPlayer, status, L, (PlayerStatus)lua_tointeger);
	LOAD_FIELD(pPlayer, flag, L, (PlayerFlag)lua_tointeger);

	lua_getfield(L, -1, "params");
	for(n = 0; n < MAX_PLAYER_PARAM; n++)
	{
		lua_pushnumber(L, n+1);
		lua_gettable(L, -2);
		pPlayer->params[n] = lua_tointeger(L, -1);
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	return R_SUCC;
}

RESULT game_load(GameContext* pGame, const char* file_name)
{
	RESULT   ret;
	int state;
	lua_State* L;
	int n;
	char  base[MAX_PATH];
	char  path[MAX_PATH];


	snprintf(path, sizeof(path), "%s/sav/%s.sav", get_app_path(base, sizeof(base)), file_name);


	//FILE* file;

	//file = fopen(path, "rb");

	//if(file == NULL)
	//{
	//	MSG_OUT("open file \"%s\" error: (%d) %s\n", path, errno, strerror(errno));
	//	return R_E_FAIL;
	//}

	L = lua_open();

	if(L == NULL)
	{
		MSG_OUT("create LUA engine failed: out of memory\n");	
		return R_E_MEMORY;
	}

	ret = R_SUCC;

	do{

		state = luaL_dofile(L, path);

		if(state != 0)
		{
			MSG_OUT("%s\n", lua_tostring(L, -1));
			ret = R_E_FAIL;
			break;
		}


		lua_getglobal(L,  "game");
		if(!lua_istable(L, -1))
		{
			MSG_OUT("global define 'game' is not found!\n");
			ret = R_E_FAIL;
			break;
		}

		memset(pGame, 0, sizeof(*pGame));

		// start load...
		LOAD_FIELD(pGame, nPlayerCount, L, lua_tointeger);
		LOAD_FIELD(pGame, nMinsterCount, L, lua_tointeger);
		LOAD_FIELD(pGame, nSpyCount, L, lua_tointeger);
		LOAD_FIELD(pGame, nMutineerCount, L, lua_tointeger);

		if(pGame->nPlayerCount > MAX_PLAYER_NUM)
		{
			MSG_OUT("error player count!\n");
			ret = R_E_FAIL;
			break;
		}
	
		// players
		lua_getfield(L, -1, "players");

		//if(!lua_istable(L, -1))
		//{
		//	MSG_OUT("in 'game' table, sub table 'players' is not found!\n");
		//	ret = R_E_FAIL;
		//	break;
		//}

		for(n = 0; n < pGame->nPlayerCount; n++)
		{
			lua_pushnumber(L, n+1);
			lua_gettable(L, -2);
			if(R_SUCC != (ret = game_load_player(&pGame->players[n], L)))
			{
				break;
			}
			lua_pop(L, 1);
		}
		if(ret != R_SUCC)
			break;

		lua_pop(L, 1);

		// get stack
		lua_getfield(L, -1, "stGetCardStack");
		if(R_SUCC != (ret = game_load_cardstack(&pGame->stGetCardStack, L)))
		{
			break;
		}
		lua_pop(L, 1);
		// get stack
		lua_getfield(L, -1, "stDiscardCardStack");
		game_load_cardstack(&pGame->stDiscardCardStack, L);
		lua_pop(L, 1);

		LOAD_FIELD(pGame, nCurDiscardCardNum, L, lua_tointeger);
		if(pGame->nCurDiscardCardNum > MAX_CUR_DISCARD_NUM)
		{
			MSG_OUT("error nCurDiscardCardNum!\n");
			ret = R_E_FAIL;
			break;
		}

		lua_getfield(L, -1, "stCurDiscardCards");
		for(n = 0; n < pGame->nCurDiscardCardNum; n++)
		{
			lua_pushnumber(L, n+1);
			lua_gettable(L, -2);
			if(R_SUCC != (ret = game_load_card(&pGame->stCurDiscardCards[n], L)))
			{
				break;
			}
			lua_pop(L, 1);
		}
		if(ret != R_SUCC)
			break;

		lua_pop(L, 1);

		LOAD_FIELD(pGame, nRoundNum, L, lua_tointeger);
		LOAD_FIELD(pGame, nRoundPlayer, L, lua_tointeger);
		LOAD_FIELD(pGame, nCurPlayer, L, lua_tointeger);
		LOAD_FIELD(pGame, status, L, (Status)lua_tointeger);

		lua_pop(L, 1);

		ret = R_SUCC;
		break;
	} while(0);

	lua_close(L);

	//fclose(file);
	return ret;
}


static int file_name_cmp(lua_State* L)
{
	int ret;
	const char*  f1 = luaL_optstring(L, 1, "");
	const char*  f2 = luaL_optstring(L, 2, "");
	ret = strcasecmp(f1, f2);
	//MSG_OUT("file_name_cmp ('%s','%s') = %d\n", f1, f2, ret);
	lua_pushboolean(L, ret < 0);
	return 1;
}

void game_import_file(lua_State* L, const char* pattern)
{
	struct _finddata_t  fdata;
	long  fid;
	const char* last_path;
	char  rel_path[MAX_PATH];
	char* base_name;
	char  cwd_path[MAX_PATH];
	char  cur_path[MAX_PATH];
	char  full_path[MAX_PATH];
	int  n;

	lua_getfield(L, LUA_REGISTRYINDEX, "__import_path");

	if(lua_isnil(L, -1))
	{
		lua_pushstring(L, "import call only while importing lua files");
		lua_error(L);
		return;
	}

	last_path = lua_tostring(L, -1);

	get_cur_path(cwd_path, sizeof(cwd_path));

	strncpy(rel_path, pattern, sizeof(rel_path));
	REGULAR_PATH(rel_path);

	base_name = strrchr(rel_path, '/');

	if(base_name != NULL)
	{
		*base_name = '\0';
		base_name++;

		if(0 != set_cur_path(rel_path))
		{
			lua_pushfstring(L, "import \"%s\" error: (%d) %s", pattern, errno, strerror(errno));
			lua_error(L);
			return;
		}
	}
	else
	{
		base_name = rel_path;
	}

	get_cur_path(cur_path, sizeof(cur_path));


	//path = pattern;
	
	fid = _findfirst(base_name, &fdata);

	if(fid == -1)
	{
		set_cur_path(cwd_path);
		lua_pushfstring(L, "import \"%s\" error: (%d) %s", pattern, errno, strerror(errno));
		lua_error(L);
		return;
	}

	lua_newtable(L);   // ... [t]
	n = 1;	

	do {
		//MSG_OUT("%s\n", fdata.name);
		lua_pushnumber(L, n);   // ... [t] [n]
		lua_pushstring(L, fdata.name);   // ... [t] [n] [fdata.name]
		lua_rawset(L, -3);      // ... [t]
		n++;
	} while(0 == _findnext(fid, &fdata));

	_findclose(fid);

	//call table.sort
	lua_getglobal(L, "table");   // ... [t] [table]
	lua_getfield(L, -1, "sort"); // ... [t] [table] [table.sort]
	lua_pushvalue(L, -3);       
	lua_pushcfunction(L, file_name_cmp);   // ... [t] [table] [table.sort] [t] [file_name_cmp]
	if(0 != lua_pcall(L, 2, 0, 0))  // ... [t] [table] 
	{
		set_cur_path(cwd_path);
		lua_error(L);
		return;
	}

	lua_pop(L, 1);   // ... [t]

	for(n = 1; ; n ++)
	{
		lua_pushnumber(L, n);   // ... [t] [n]
		lua_rawget(L, -2);      // ... [t] [t[n]]
		if(lua_isnil(L, -1))
		{
			lua_pop(L, 1);     // ... [t]
			break;
		}


		snprintf(full_path, sizeof(full_path), "%s/%s", cur_path, lua_tostring(L, -1));

		lua_pop(L, 1);     // ... [t]

		// is imported?
		lua_getfield(L, LUA_REGISTRYINDEX, "__imported_table");   // ... [t] [__imported_table]
		if(lua_isnil(L, -1)) 
		{
			lua_pop(L, 1);
			lua_newtable(L);
			lua_pushvalue(L, -1);
			lua_setfield(L, LUA_REGISTRYINDEX, "__imported_table");
		}

		lua_getfield(L, -1, full_path); // ... [t] [__imported_table]  [__imported_table[full_path]]

		if(!lua_isnil(L, -1))
		{
			lua_pop(L, 2);
			// imported already
			continue;
		}

		lua_pop(L, 2);  // ... [t]

		// is in importing?

		lua_getfield(L, LUA_REGISTRYINDEX, "__importing_table");  // ... [t] [__importing_table]
		if(lua_isnil(L, -1)) 
		{
			lua_pop(L, 1);
			lua_newtable(L);
			lua_pushvalue(L, -1);
			lua_setfield(L, LUA_REGISTRYINDEX, "__importing_table");
		}

		lua_getfield(L, -1, full_path);  // ... [t] [__importing_table] [__importing_table[full_path]] 

		if(!lua_isnil(L, -1))
		{
			// is in importing
			lua_pop(L, 2);
			lua_pushfstring(L, "import file \"%s\" is circular dependency.", full_path);
			set_cur_path(cwd_path);
			lua_error(L);
		}

		lua_pop(L, 1);  // ... [t] 
		
		// add to impirting table
		lua_pushstring(L, full_path);
		lua_pushnumber(L, 1);
		lua_settable(L, -3);  // ... [t] [__importing_table]

		lua_pop(L, 1);  // ... [t] 

		MSG_OUT("importing (%d) \"%s\" ...\n", n, full_path);

		if(0 != luaL_dofile(L, full_path))
		{
			set_cur_path(cwd_path);
			lua_error(L);
			return;
		}
		// remove from importing table
		lua_getfield(L, LUA_REGISTRYINDEX, "__importing_table");  // ... [t] [__importing_table]
		lua_pushstring(L, full_path);
		lua_pushnil(L);
		lua_settable(L, -3);  // ... [t] [__importing_table]
		
		lua_pop(L, 1);  // ... [t] 

		// add to imported table;
		lua_getfield(L, LUA_REGISTRYINDEX, "__imported_table");  // ... [t] [__importing_table]
		lua_pushstring(L, full_path);
		lua_pushnumber(L, 1);
		lua_settable(L, -3);  // ... [t] [__importing_table]

		lua_pop(L, 1);  // ... [t] 

	}

	// ... [t]

	lua_pop(L, 1);

	// [__import_path]
	lua_pop(L, 1);
	set_cur_path(cwd_path);
}


