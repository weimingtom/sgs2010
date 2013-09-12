#include "game.h"
#include "event.h"
#include "card.h"
#include "card_stack.h"
#include "player.h"
#include "script.h"
#include "cmd.h"
#include "load.h"



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


static void game_load_card(lua_State* L, Card* pCard, int not_none)
{
	//LOAD_INT_CAST(pCard, id, L, CardID);
	lua_getfield(L, -1, "sid");
	pCard->id = card_sid2id(lua_tostring(L, -1));
	lua_pop(L, 1);
	if(not_none && pCard->id == CardID_None)
	{
		luaL_error(L, "expected a valid card id!");
	}
	LOAD_INT_CAST(pCard, color, L, CardColor);
	LOAD_INT_CAST(pCard, value, L, CardValue);
	LOAD_INT_CAST(pCard, flag, L, CardFlag);
}

static void game_load_vcard(lua_State* L, VCard* pVCard, int not_none)
{
	int n;
	//LOAD_INT_CAST(pCard, id, L, CardID);
	lua_getfield(L, -1, "vcard");
	if(lua_istable(L, -1))
	{
		game_load_card(L, &pVCard->vcard, 1);
		lua_pop(L, 1);
		LOAD_INT(pVCard, rnum, L);
		if(pVCard->rnum <= 0 || pVCard->rnum > MAX_RCARD_NUM)
		{
			luaL_error(L, "error rcard num!");
		}
		lua_getfield(L, -1, "rcards"); // ... [cardstack] [cardstack.cards]
		for(n = 0; n < pVCard->rnum; n++)
		{
			lua_pushnumber(L, n+1);
			lua_gettable(L, -2);
			game_load_card(L, &pVCard->rcards[n], 1);
			lua_pop(L, 1);
		}
		lua_pop(L, 1);
	}
	else
	{
		lua_pop(L, 1);
		game_load_card(L, &pVCard->vcard, not_none);
		pVCard->rnum = 1;
		pVCard->rcards[0] = pVCard->vcard;

	}
}


static void game_load_cardstack(lua_State* L, CardStack* pCardStack)
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
		game_load_card(L, &pCardStack->cards[n], 1);
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
}


static void game_load_player(lua_State* L, Player* pPlayer)
{
	int n;

	LOAD_INT_CAST(pPlayer, id, L, PlayerID);
	//LOAD_INT_CAST(pPlayer, hero, L, HeroID);
	lua_getfield(L, -1, "hero");
	pPlayer->hero = hero_sid2id(lua_tostring(L, -1));
	lua_pop(L, 1);
	LOAD_INT(pPlayer, max_life, L);
	LOAD_INT(pPlayer, cur_life, L);
	LOAD_STRING(pPlayer, name, L);
	LOAD_INT(pPlayer, hand_card_num, L);
	LOAD_INT(pPlayer, judgment_card_num, L);

	if(pPlayer->hand_card_num > MAX_HAND_CARD)
	{
		luaL_error(L, "error player hand card count!");
	}
	if(pPlayer->judgment_card_num > MAX_JUDGMENT_CARD)
	{
		luaL_error(L, "error player judgment card count!");
	}

	lua_getfield(L, -1, "hand_cards");
	for(n = 0; n < pPlayer->hand_card_num; n++)
	{
		lua_pushnumber(L, n+1);
		lua_gettable(L, -2);
		game_load_card(L, &pPlayer->hand_cards[n], 1);		
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	lua_getfield(L, -1, "equip_cards");
	for(n = 0; n < EquipIdx_Max; n++)
	{
		lua_pushnumber(L, n+1);
		lua_gettable(L, -2);
		game_load_card(L, &pPlayer->equip_cards[n], 0);
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	lua_getfield(L, -1, "judgment_cards");
	for(n = 0; n < pPlayer->judgment_card_num; n++)
	{
		lua_pushnumber(L, n+1);
		lua_gettable(L, -2);
		game_load_vcard(L, &pPlayer->judgment_cards[n], 1);		
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

	if(pGame == NULL)
	{
		luaL_error(L, "game null pointer!");
	}


	if(pGame->status != Status_None)
	{
		luaL_error(L, "已经在游戏中，不能加载游戏进度!");
	}

	//ret = R_SUCC;

	top = lua_gettop(L);

	// do {
	lua_newtable(L);  // [t]
	lua_pushvalue(L, -1);  // [t] [t]
	lua_pushstring(L, "__index");
	lua_pushvalue(L, LUA_GLOBALSINDEX);  // [t] [t] ['__index'] [_G]
	lua_rawset(L, -3);   // [t] [t]
	lua_setmetatable(L, -2);  // [t]


	if(lua_istable(L, 2))
	{
		lua_pushvalue(L, 2);
		lua_setfield(L, -2, "game");		
	}
	else 
	{
		file_name = luaL_checkstring(L, 2);

		if(0 != luaL_loadfile(L, file_name))   // [t] [f]
		{
			luaL_error(L, "%s", lua_tostring(L, -1));
		}

		lua_pushvalue(L, -2);   // [t] [f] [t]
		lua_setfenv(L, -2);     // [t] [f]

		lua_call(L, 0, 0);
	}

	//stack:  [t] 


	lua_getfield(L, -1, "game"); // [t] [t.game]
	if(!lua_istable(L, -1))
	{
		luaL_error(L, "global define 'game' is not found!");
	}

	// do not clear game, because the game is already inited with L
	//memset(pGame, 0, sizeof(*pGame));

	// start load...
	LOAD_INT(pGame, player_count, L);
	LOAD_INT(pGame, minister_count, L);
	LOAD_INT(pGame, spy_count, L);
	LOAD_INT(pGame, mutineer_count, L);

	if(pGame->player_count > MAX_PLAYER_NUM)
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

	for(n = 0; n < pGame->player_count; n++)
	{
		lua_pushnumber(L, n+1);
		lua_gettable(L, -2);   // [t] [t.game] [t.game.players] [t.game.players[n+1]]
		game_load_player(L, &pGame->players[n]);
		lua_pop(L, 1);     // [t] [t.game] [t.game.players] 
	}

	lua_pop(L, 1);  // [t] [t.game]

	// get stack
	lua_getfield(L, -1, "get_card_stack"); // [t] [t.game] [t.game.get_card_stack]
	game_load_cardstack(L, &pGame->get_card_stack);
	lua_pop(L, 1); // [t] [t.game]

	// get stack
	lua_getfield(L, -1, "discard_card_stack"); // [t] [t.game] [t.game.discard_card_stack]
	game_load_cardstack(L, &pGame->discard_card_stack);
	lua_pop(L, 1);  // [t] [t.game]

	LOAD_INT(pGame, cur_discard_card_num, L);
	if(pGame->cur_discard_card_num > MAX_CUR_DISCARD_NUM)
	{
		luaL_error(L, "error cur_discard_card_num!");
	}

	lua_getfield(L, -1, "cur_discard_cards");  // [t] [t.game] [t.game.cur_discard_cards]
	for(n = 0; n < pGame->cur_discard_card_num; n++)
	{
		lua_pushnumber(L, n+1);
		lua_gettable(L, -2);   // [t] [t.game] [t.game.cur_discard_cards] [t.game.cur_discard_cards[n+1]]
		game_load_card(L, &pGame->cur_discard_cards[n], 0);
		lua_pop(L, 1);    // [t] [t.game] [t.game.cur_discard_cards]
	}
	lua_pop(L, 1);    // [t] [t.game] 

	LOAD_INT(pGame, round_num, L);
	LOAD_INT(pGame, round_player, L);
	LOAD_INT(pGame, cur_player, L);
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


	L = get_game_script();

	if(L == NULL)
		return R_E_PARAM;


	lua_pushcfunction(L, lua_game_load);
	lua_pushlightuserdata(L, pGame);

	if(file_name[0] == '?')
	{
		snprintf(path, sizeof(path), "%s",  file_name);		
		lua_getfield(L, LUA_REGISTRYINDEX, path);
	}
	else
	{
		snprintf(path, sizeof(path), "%s/../sav/%s.sav", get_app_path(base, sizeof(base)), file_name);
		lua_pushstring(L, path);
	}

	state = script_pcall(L, 2, 0);

	if(state != 0)
	{
		MSG_OUT("load game from \"%s\" failed: \n%s\n", path, lua_tostring(L, -1));
		lua_pop(L, 1);
		return R_E_FAIL;
	}

	return R_SUCC;
}

