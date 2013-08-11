#include "config.h"
#include "game.h"
#include "comm.h"
#include "hero.h"
#include "cmd.h"
#include "event.h"
#include "get.h"
#include "judge.h"
#include "stack.h"
#include "discard.h"
#include "script.h"
#include "load.h"


//#define NEXT_ROUND(pGame)   ((pGame)->round_player = game_next_player((pGame), (pGame)->round_player))
//#define NEXT_CUR(pGame)   ((pGame)->cur_player = game_next_player((pGame),(pGame)->cur_player))

const char* game_status_str(Status s)
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




Status get_game_status(GameContext* pGame)
{
	return pGame->status;
}

int get_game_cur_player(GameContext* pGame)
{
	return pGame->cur_player;
}

int get_game_round_player(GameContext* pGame)
{
	return pGame->round_player;
}

int get_game_master_player(GameContext* pGame)
{
	int n;
	for(n = 0; n < pGame->player_count; n++)
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
		idx = (idx +  1) % pGame->player_count;
	} while(idx != player && IS_PLAYER_DEAD(GAME_PLAYER(pGame, idx)));

	return idx;
}

int game_prev_player(GameContext* pGame, int player)
{
	// TODO: must think about the player is die
	int  idx = player;
	do {
		idx = (idx - 1 + pGame->player_count) % pGame->player_count;
	} while(idx != player && IS_PLAYER_DEAD(GAME_PLAYER(pGame, idx)));

	return idx;
}


Player* get_game_player(GameContext* pGame, int player)
{
	return (player >= 0 && player < pGame->player_count) ? &pGame->players[player] : NULL;
}


// 计算当前指定玩家的行动顺序。当前玩家为0，下一家为1 ....
int get_game_act_order(GameContext* pGame, int player)
{
	if(!IS_PLAYER_VALID(pGame, player))
		return -1;

	int order = 0;
	int cp = pGame->cur_player;
	for(order = 0; order <= pGame->player_count && player !=  cp; order++)
	{
		cp = game_next_player(pGame, cp);
	}

	return order;
}



RESULT set_game_cur_player(GameContext* pGame, int player)
{
	if(player < 0 || player >= pGame->player_count)
		return R_E_PARAM;

	if(player != pGame->cur_player)
	{
		pGame->cur_player = player;
		MSG_OUT("当前玩家切换为【%s】\n", CUR_PLAYER(pGame)->name);
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
			pn = (pn + 1) % pGame->player_count;
		} while(pn != p2 && IS_PLAYER_DEAD(GAME_PLAYER(pGame, pn)));
		do {
			pp = (pp - 1 + pGame->player_count) % pGame->player_count;
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


RESULT init_game_context(GameContext* pGame, int ministers, int spies, int mutineers)
{
	int n, c;
	int pids[MAX_PLAYER_NUM];
	int hids[MAX_HERO_NUM];
	int hcnt;
	int hmcnt;
	int hscnt;

	//const HeroConfig* pHero;
	char        temp[128];
	char        title[512];
	SelOption   sel_opts[5];
	int         sel_n;


	if(ministers < 1 || spies < 1 || mutineers < 1 || ministers + spies + mutineers + 1 > MAX_PLAYER_NUM)
	{
		MSG_OUT("error player config num!\n");
		return R_E_PARAM;
	}
	

	hcnt = hero_maxid();

	if(hcnt == 0)
	{
		MSG_OUT("没有任何武将！\n");
		return R_E_FAIL;
	}

	// the hero over MAX_HERO_NUM is ignored
	if(hcnt > MAX_HERO_NUM)
		hcnt = MAX_HERO_NUM;
	hmcnt = 0;
	for(n = 0; n < hcnt; n++)
	{
		//pHero = get_hero_config((HeroID)n);
		//if(pHero)
		{
			if(YES == hero_master((HeroID)(n+1)))
			{
				if(n > hmcnt)
				{
					hids[n] = hids[hmcnt];
				}
				hids[hmcnt] = n+1;
				hmcnt++;
			}
			else
			{
				hids[n] = n+1;
			}
			//hcnt++;
		}
	}


	//memset(pGame, 0, sizeof(*pGame));

	pGame->player_count = ministers + spies + mutineers + 1;
	pGame->minister_count = ministers;
	pGame->spy_count = spies;
	pGame->mutineer_count = mutineers;
	
	MSG_OUT("开始新游戏: 配置为【%d】玩家 - (%d 主公, %d 忠臣, %d 内奸, %d 反贼)\n", 
		pGame->player_count, 1, pGame->minister_count, pGame->spy_count, pGame->mutineer_count);

	// init players
	c = 0;
	pids[c++] = PlayerID_Master;
	for(n = 0; n < ministers; n++) pids[c++] = PlayerID_Minister;
	for(n = 0; n < spies; n++) pids[c++] = PlayerID_Spy;
	for(n = 0; n < mutineers; n++) pids[c++] = PlayerID_Mutineer;

	rand_array_i(pids, c, c * 2);

	pGame->status = Status_NewGame;
	pGame->round_num = 0;
	pGame->round_player = 0;
	pGame->cur_player = -1;


	for(n = 0; n < pGame->player_count; ++n)
	{
		if(pids[n] == PlayerID_Master)
		{
			//set_game_cur_player(pGame, n);
			pGame->cur_player = n;
			pGame->round_player = n;
			break;
		}
	}

	for(n = 0; n < pGame->player_count; n++)
	{
		if(hcnt > 0)
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
				snprintf(title, sizeof(title), "玩家 [%d], 身份：%s， 请选择武将：", pGame->cur_player+1, player_id_str((PlayerID)pids[pGame->cur_player]));
				sel_n = 0;
				ST_ZERO(sel_opts);
				for(c = 0; c < hscnt; c++)
				{
					//pHero = get_hero_config((HeroID)hids[c]);
					snprintf(sel_opts[sel_n].text, sizeof(sel_opts[sel_n].text), "[%s], %s最大体力: %d", 
						hero_name((HeroID)hids[c], temp, sizeof(temp)), hero_master((HeroID)hids[c])==YES ? "[主公], ":"", hero_life((HeroID)hids[c]));
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
			init_player(&pGame->players[pGame->cur_player], (PlayerID)pids[pGame->cur_player], (HeroID)hids[c-1]);
			if(c < hcnt)
			{
				hids[c-1] = hids[hcnt-1];
			}
			hcnt--;
		}
		else
		{
			init_player(&pGame->players[pGame->cur_player], (PlayerID)pids[pGame->cur_player], HeroID_None);
			snprintf(pGame->players[pGame->cur_player].name, MAX_NAME_LEN, "玩家%d", pGame->cur_player+1);
		}

		pGame->cur_player = (pGame->cur_player + 1) % pGame->player_count;
		
	}


	// init card stack

	game_init_getstack(&pGame->get_card_stack);
	card_stack_random(&pGame->get_card_stack);

	// clear out cards;
	pGame->discard_card_stack.count = 0;
	ST_ZERO(pGame->cur_discard_cards);
	pGame->cur_discard_card_num = 0;


	pGame->round_num = 0;

	// first get 4 cards per player

	game_first_getcard(pGame);


	return R_SUCC;
}




extern "C" {

static  RESULT game_next_round(GameContext* pGame, GameEventContext* pEvent);

static  RESULT game_round_begin(GameContext* pGame, GameEventContext* pEvent)
{
	MSG_OUT("第[%d]回合，开始阶段，当前回合玩家是【%s】。\n", pGame->round_num, ROUND_PLAYER(pGame)->name);

	GameEventContext  event;
	INIT_EVENT(&event, GameEvent_RoundBegin, pGame->round_player, INVALID_PLAYER, pEvent);


	trigger_game_event(pGame, &event);

	game_flush_discard_cur(pGame);

	return R_SUCC;
}

static RESULT game_round_judge(GameContext* pGame, GameEventContext* pEvent)
{
	RESULT ret;
	GameEventContext  event;

	MSG_OUT("第[%d]回合，判定阶段，当前回合玩家是【%s】。\n", pGame->round_num, ROUND_PLAYER(pGame)->name);
	INIT_EVENT(&event, GameEvent_PerRoundJudge, pGame->round_player, INVALID_PLAYER, pEvent);
	trigger_game_event(pGame, &event);
	game_flush_discard_cur(pGame);

	// is ot not skip this round step ?
	if(event.result == R_SKIP)
	{
		// skip this step
		return R_SUCC;
	}

	ret = game_round_do_judge(pGame, pEvent, pGame->round_player);

	CHECK_RET(ret,ret);

	game_flush_discard_cur(pGame);


	INIT_EVENT(&event, GameEvent_PostRoundJudge, pGame->round_player, INVALID_PLAYER, pEvent);
	trigger_game_event(pGame, &event);
	game_flush_discard_cur(pGame);

	return R_SUCC;
}


static RESULT game_round_getcard(GameContext* pGame, GameEventContext* pEvent)
{
	RESULT  ret;
	GetCard stGetCard;
	GameEventContext  event;

	MSG_OUT("第[%d]回合，摸牌阶段，当前回合玩家是【%s】。\n", pGame->round_num, ROUND_PLAYER(pGame)->name);

	ST_ZERO(stGetCard);
	stGetCard.num = 2;  // in get round init to get 2 card 
	INIT_EVENT(&event, GameEvent_PerRoundGet, pGame->round_player, INVALID_PLAYER, pEvent);
	event.get_card = &stGetCard;
	trigger_game_event(pGame, &event);

	game_flush_discard_cur(pGame);


	if(event.result == R_SKIP)
	{
		// skip get card step
		return R_SUCC;
	}
	
	ret = game_round_do_get(pGame, pEvent, pGame->round_player, stGetCard.num);
	CHECK_RET(ret,ret);

	game_flush_discard_cur(pGame);


	INIT_EVENT(&event, GameEvent_PostRoundGet, pGame->round_player, INVALID_PLAYER, pEvent);
	event.get_card = &stGetCard;
	trigger_game_event(pGame, &event);

	game_flush_discard_cur(pGame);

	return R_SUCC;
}

static RESULT game_round_outcard(GameContext* pGame, GameEventContext* pEvent)
{
	GameEventContext  event;

	MSG_OUT("第[%d]回合，出牌阶段，当前回合玩家是【%s】。\n", pGame->round_num, ROUND_PLAYER(pGame)->name);

	INIT_EVENT(&event, GameEvent_PerRoundOut, pGame->round_player, INVALID_PLAYER, pEvent);
	trigger_game_event(pGame, &event);
	game_flush_discard_cur(pGame);

	if(event.result == R_SKIP)
	{
		// skip out card  step
		return R_SUCC;
	}

	while (R_SUCC == game_round_do_out(pGame, pEvent, pGame->round_player))
	{
		// do nothing
		game_flush_discard_cur(pGame);
	}

	INIT_EVENT(&event, GameEvent_PostRoundOut, pGame->round_player, INVALID_PLAYER, pEvent);
	trigger_game_event(pGame, &event);
	game_flush_discard_cur(pGame);

	return R_SUCC;
}

static RESULT game_round_discardcard(GameContext* pGame, GameEventContext* pEvent)
{
	RESULT ret;
	GameEventContext  event;

	MSG_OUT("第[%d]回合，弃牌阶段，当前回合玩家是【%s】。\n", pGame->round_num, ROUND_PLAYER(pGame)->name);

	// trigger round discard event
	INIT_EVENT(&event, GameEvent_PerRoundDiscard, pGame->round_player, INVALID_PLAYER, pEvent);
	trigger_game_event(pGame, &event);
	game_flush_discard_cur(pGame);

	if(event.result == R_SKIP)
	{
		// skip discard card step
		return R_SUCC;
	}

	// discard loop
	ret = game_round_discard_card(pGame, pEvent, pGame->round_player);

	CHECK_RET(ret,ret);
	game_flush_discard_cur(pGame);


	// wait cmd_loop discard cmd execute
	INIT_EVENT(&event, GameEvent_PostRoundDiscard, pGame->round_player, INVALID_PLAYER, pEvent);
	trigger_game_event(pGame, &event);
	game_flush_discard_cur(pGame);

	return R_SUCC;
}

static RESULT game_round_end(GameContext* pGame, GameEventContext* pEvent)
{
	// trigger round end event
	GameEventContext  event;

	MSG_OUT("第[%d]回合，结束阶段，当前回合玩家是【%s】。\n", pGame->round_num, ROUND_PLAYER(pGame)->name);


	INIT_EVENT(&event, GameEvent_RoundEnd, pGame->round_player, INVALID_PLAYER, pEvent);

	trigger_game_event(pGame, &event);
	game_flush_discard_cur(pGame);

	return R_SUCC;
}


static RESULT game_next_round(GameContext* pGame, GameEventContext* pEvent)
{
	// calc next round player
	for(;;)
	{
		pGame->round_player = game_next_player(pGame, pGame->round_player);
		pGame->round_num++;

		if(!PLAYER_CHK_FLAG(ROUND_PLAYER(pGame), PlayerFlag_SkipNextRound))
			break;

		// clear flags
		PLAYER_CLR_ALL_FLAG(ROUND_PLAYER(pGame));
	}


	// MSG_OUT("next round: num [%d], round player is set to: %d, [%s]\n", pGame->round_num, pGame->round_player, ROUND_PLAYER(pGame)->name);

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
		pGame->round_player = get_game_master_player(pGame);
		pGame->round_num = 1;
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
		ret = init_game_context(pGame, pEvent->new_game_config->ministers, pEvent->new_game_config->spies, pEvent->new_game_config->mutineers);
		break;
	case GameEvent_LoadGame:
		ret = game_load(pGame, pEvent->file_name);
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
			ret = R_E_FAIL;
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
			if(pGame->status == Status_GameOver)
			{
				MSG_OUT("游戏结束。\n");
			}
			else if(pGame->status == Status_GameAbort)
			{
				MSG_OUT("游戏退出：%s\n", lua_tostring(L, -1));
			}
			else
			{
				MSG_OUT("ERROR: %s\n", lua_tostring(L, -1));
			}
			lua_pop(L, 1);
			ret = R_SUCC;
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




// if the game is over, this function do not return.
void game_check_gameover(GameContext* pGame, int player)
{
	
	// TODO: not yet implements
	// calc the live players of each actor

	int n;
	int cnt_master;
	int cnt_minister;
	int cnt_spy;
	int cnt_mutineer;
	Player* p;

	char buf[256];
	int  len;

	GameResult  result = GameResult_InPlaying;

	cnt_master = 0;
	cnt_minister = 0;
	cnt_spy = 0;
	cnt_mutineer = 0;

	for(n = 0; n < pGame->player_count; n++)
	{
		p = GAME_PLAYER(pGame, n);
		if(!IS_PLAYER_DEAD(p))
		{
			switch(p->id)
			{
			case PlayerID_Master: cnt_master++; break;
			case PlayerID_Minister: cnt_minister++; break;
			case PlayerID_Spy: cnt_spy++; break;
			case PlayerID_Mutineer: cnt_mutineer++;break;
			default: break;
			}
		}
	}

	// when master is dead. 
	if(cnt_master == 0)
	{	
		if(cnt_spy == 1 && cnt_minister == 0 && cnt_mutineer == 0)
		{
			// lived spy win
			result = GameResult_SpyWin;
			len = 0;

			for(n = 0; n < pGame->player_count; n++)
			{
				p = GAME_PLAYER(pGame, n);
				if(!IS_PLAYER_DEAD(p) && p->id == PlayerID_Spy)
				{
					snprintf(buf+len,sizeof(buf)-len,"【%s】",p->name);
				}
			}


			MSG_OUT("内奸获得胜利：%s\n", buf);			
		}
		else
		{
			// all mutineers win
			result = GameResult_MutineerWin;
			len = 0;

			for(n = 0; n < pGame->player_count; n++)
			{
				p = GAME_PLAYER(pGame, n);
				if(p->id == PlayerID_Mutineer)
				{
					snprintf(buf+len,sizeof(buf)-len,"【%s】",p->name);
				}
			}

			MSG_OUT("反贼获得胜利：%s\n", buf);			
		}
	}
	else if(cnt_spy == 0 && cnt_mutineer == 0)
	{
		// master and all ministers win
		result = GameResult_MasterWin;
		len = 0;

		for(n = 0; n < pGame->player_count; n++)
		{
			p = GAME_PLAYER(pGame, n);
			if(p->id == PlayerID_Master || p->id == PlayerID_Minister)
			{
				snprintf(buf+len,sizeof(buf)-len,"【%s】",p->name);
			}
		}
		MSG_OUT("主公和忠臣获得胜利：%s\n", buf);
	}

	// not yet game over
	if(result != GameResult_InPlaying)
	{
		pGame->status = Status_GameOver;
		luaL_error(get_game_script(), "Game Over");
	}
}



