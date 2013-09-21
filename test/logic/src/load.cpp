#include "game.h"
#include "event.h"
#include "card.h"
#include "card_stack.h"
#include "player.h"
#include "script.h"
#include "cmd.h"
#include "load.h"


#define LOAD_ERROR_TABLE(L)	do { \
	if (lua_isnoneornil(L, -1) )  \
	{ luaL_error(L, "load field \'%s\' is not defined!", lua_tostring(L, -2)); } \
	else if (!lua_istable(L, -1) )  \
	{ luaL_error(L, "load field \'%s\' is not a table!", lua_tostring(L, -2)); } \
} while(0)

#define LOAD_ERROR_FIELD_INT(L, field)	do { \
	if(lua_isnoneornil(L, -1) ) \
	{ luaL_error(L, "load field \"%s.%s\" is not defined!", lua_tostring(L, -3), #field); } \
	else if(!lua_isnumber(L, -1) ) \
	{ luaL_error(L, "load field \"%s.%s\" is not a number!", lua_tostring(L, -3), #field); } \
} while(0)

#define LOAD_ERROR_FIELD_INT_N(L, n)	do { \
	if(lua_isnoneornil(L, -1) ) \
	{ luaL_error(L, "load field \"%s[%d]\" is not defined!", lua_tostring(L, -3), (n)); } \
	else if(!lua_isnumber(L, -1) ) \
	{ luaL_error(L, "load field \"%s[%d]\" is not a number!", lua_tostring(L, -3), (n)); } \
} while(0)

#define LOAD_ERROR_FIELD_STRING(L, field)	do { \
	if(lua_isnoneornil(L, -1) )  \
	{ luaL_error(L, "load field \"%s.%s\" is not defined!", lua_tostring(L, -3), #field); } \
	else if(!lua_isstring(L, -1) ) \
	{ luaL_error(L, "load field \"%s.%s\" is not a string!", lua_tostring(L, -3), #field); } \
} while(0)

#define LOAD_ERROR_CHK(L,field, b, msg)     do { \
	if(!(b)) { luaL_error(L, "load field \"%s.%s\" %s!", lua_tostring(L, -3), #field, ""msg); } \
} while(0)

#define LOAD_BEGIN_FIELD(L,field)   do { \
	lua_pushfstring(L, "%s.%s", lua_tostring(L, -2), #field);  \
	lua_pushstring(L, #field); \
	lua_gettable(L, -3); \
	LOAD_ERROR_TABLE(L); \
} while(0)

#define LOAD_BEGIN_FIELD_N(L, n) 	do { \
	lua_pushfstring(L, "%s[%d]", lua_tostring(L, -2), (n));  \
	lua_pushnumber(L, (n)); \
	lua_gettable(L, -3); \
	LOAD_ERROR_TABLE(L); \
} while(0)


#define LOAD_END_FIELD(L)     do { lua_pop(L, 2); } while(0)




#define LOAD_INT(p,field,L)  do{ \
	lua_getfield(L, -1, #field); \
	LOAD_ERROR_FIELD_INT(L, field); \
	(p)->field = lua_tointeger(L, -1); \
	lua_pop(L, 1); \
} while(0)

#define LOAD_INT_CAST(p,field,L,t)  do{ \
	lua_getfield(L, -1, #field); \
	LOAD_ERROR_FIELD_INT(L, field); \
	(p)->field = (t)lua_tointeger(L, -1); \
	lua_pop(L, 1); \
} while(0)

#define LOAD_STRING(p,field,L)  do{ \
	lua_getfield(L, -1, #field); \
	LOAD_ERROR_FIELD_STRING(L, field); \
	strncpy((p)->field, lua_tostring(L, -1), sizeof((p)->field)); \
	lua_pop(L, 1); \
} while(0)

#define LOAD_FIELD_STRING(L,field) do{ \
	lua_getfield(L, -1, #field); \
	LOAD_ERROR_FIELD_STRING(L, field); \
} while(0)

#define LOAD_FIELD_INT_N(L,n) do{ \
	lua_pushnumber(L, (n));\
	lua_gettable(L, -2); \
	LOAD_ERROR_FIELD_INT_N(L, (n)); \
} while(0)


static void game_load_card(lua_State* L, Card* pCard, int not_none)
{
	//LOAD_INT_CAST(pCard, id, L, CardID);
	LOAD_FIELD_STRING(L,sid);
	LOAD_ERROR_CHK(L, sid, card_sid_valid(lua_tostring(L, -1), not_none), "is not a valid card sid");
	pCard->id = card_sid2id(lua_tostring(L, -1));
	//LOAD_ERROR_CHK(L, sid, (not_none && pCard->id == CardID_None), "is not a valid card sid");
	lua_pop(L, 1);
	LOAD_INT_CAST(pCard, color, L, CardColor);
	LOAD_INT_CAST(pCard, value, L, CardValue);
	LOAD_INT_CAST(pCard, flag, L, CardFlag);
}

static void game_load_vcard(lua_State* L, VCard* pVCard, int not_none)
{
	int n;
	//LOAD_INT_CAST(pCard, id, L, CardID);
	lua_pushfstring(L, "%s.%s", lua_tostring(L, -2), "vcard");
	lua_getfield(L, -2, "vcard");
	if(lua_istable(L, -1))
	{
		game_load_card(L, &pVCard->vcard, 1);
		lua_pop(L, 2);
		LOAD_INT(pVCard, rnum, L);
		LOAD_ERROR_CHK(L, rnum, (pVCard->rnum >= 0 || pVCard->rnum <= MAX_RCARD_NUM), "out of range");
		LOAD_BEGIN_FIELD(L, rcards);
		lua_getfield(L, -1, "rcards"); // ... [cardstack] [cardstack.cards]
		for(n = 0; n < pVCard->rnum; n++)
		{
			LOAD_BEGIN_FIELD_N(L, n+1);
			game_load_card(L, &pVCard->rcards[n], 1);
			LOAD_END_FIELD(L);
		}
		LOAD_END_FIELD(L);
	}
	else
	{
		lua_pop(L, 2);
		game_load_card(L, &pVCard->vcard, not_none);
		pVCard->rnum = 1;
		pVCard->rcards[0] = pVCard->vcard;

	}
}


static void game_load_cardstack(lua_State* L, CardStack* pCardStack)
{
	int n;
	LOAD_INT(pCardStack, count, L);
	LOAD_ERROR_CHK(L, count, (pCardStack->count >= 0 || pCardStack->count <= CARD_STACK_SIZE), "out of range");
	LOAD_BEGIN_FIELD(L,cards);
	for(n = 0; n < pCardStack->count; n++)
	{
		LOAD_BEGIN_FIELD_N(L,n+1);
		game_load_card(L, &pCardStack->cards[n], 1);
		LOAD_END_FIELD(L);
	}
	LOAD_END_FIELD(L);
}


static void game_load_player(lua_State* L, Player* pPlayer)
{
	int n;

	LOAD_INT_CAST(pPlayer, id, L, PlayerID);
	//LOAD_INT_CAST(pPlayer, hero, L, HeroID);
	LOAD_FIELD_STRING(L, hero);
	LOAD_ERROR_CHK(L, hero, hero_sid_valid(lua_tostring(L, -1), 0), "is not a valid hero sid");
	pPlayer->hero = hero_sid2id(lua_tostring(L, -1));
	lua_pop(L, 1);
	LOAD_INT(pPlayer, max_life, L);
	LOAD_INT(pPlayer, cur_life, L);
	LOAD_STRING(pPlayer, name, L);
	LOAD_INT(pPlayer, hand_card_num, L);
	LOAD_INT(pPlayer, judgment_card_num, L);

	LOAD_ERROR_CHK(L, hand_card_num, (pPlayer->hand_card_num >= 0 && pPlayer->hand_card_num <= MAX_HAND_CARD), "out of range");
	LOAD_ERROR_CHK(L, judgment_card_num, (pPlayer->judgment_card_num >= 0 && pPlayer->judgment_card_num <= MAX_JUDGMENT_CARD), "out of range");

	LOAD_BEGIN_FIELD(L, hand_cards);
	for(n = 0; n < pPlayer->hand_card_num; n++)
	{
		LOAD_BEGIN_FIELD_N(L, n+1);
		game_load_card(L, &pPlayer->hand_cards[n], 1);		
		LOAD_END_FIELD(L);
	}
	LOAD_END_FIELD(L);

	LOAD_BEGIN_FIELD(L, equip_cards);
	for(n = 0; n < EquipIdx_Max; n++)
	{
		LOAD_BEGIN_FIELD_N(L, n+1);
		game_load_card(L, &pPlayer->equip_cards[n], 0);
		LOAD_END_FIELD(L);
	}
	LOAD_END_FIELD(L);

	LOAD_BEGIN_FIELD(L, judgment_cards);
	for(n = 0; n < pPlayer->judgment_card_num; n++)
	{
		LOAD_BEGIN_FIELD_N(L, n+1);
		game_load_vcard(L, &pPlayer->judgment_cards[n], 1);		
		LOAD_END_FIELD(L);
	}
	LOAD_END_FIELD(L);

	LOAD_INT_CAST(pPlayer, status, L, PlayerStatus);
	LOAD_INT_CAST(pPlayer, flag, L, PlayerFlag);

	LOAD_BEGIN_FIELD(L, params);
	for(n = 0; n < MAX_PLAYER_PARAM; n++)
	{
		LOAD_FIELD_INT_N(L, n+1);
		pPlayer->params[n] = lua_tointeger(L, -1);
		lua_pop(L, 1);
	}
	LOAD_END_FIELD(L);
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

	lua_pushstring(L, "game");

	lua_getfield(L, -2, "game"); // [t] ['game'] [t.game]
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

	LOAD_ERROR_CHK(L, player_count, pGame->player_count >= 0 && pGame->player_count <= MAX_PLAYER_NUM, "is out of range");

	// players
	LOAD_BEGIN_FIELD(L, players);

	for(n = 0; n < pGame->player_count; n++)
	{
		LOAD_BEGIN_FIELD_N(L, n+1);
		game_load_player(L, &pGame->players[n]);
		LOAD_END_FIELD(L);     // [t] ['game'] [t.game] ['game.players'] [t.game.players] 
	}

	LOAD_END_FIELD(L);  // [t] ['game'] [t.game]

	// get stack
	LOAD_BEGIN_FIELD(L, get_card_stack);
	game_load_cardstack(L, &pGame->get_card_stack);
	LOAD_END_FIELD(L); // [t] ['game'] [t.game]

	// get stack
	LOAD_BEGIN_FIELD(L, discard_card_stack);
	game_load_cardstack(L, &pGame->discard_card_stack);
	LOAD_END_FIELD(L);  // [t] ['game'] [t.game]

	LOAD_INT(pGame, cur_discard_card_num, L);
	LOAD_ERROR_CHK(L, cur_discard_card_num, pGame->cur_discard_card_num >= 0 && pGame->cur_discard_card_num <= MAX_CUR_DISCARD_NUM, "is out of range");

	LOAD_BEGIN_FIELD(L, cur_discard_cards);
	for(n = 0; n < pGame->cur_discard_card_num; n++)
	{
		LOAD_BEGIN_FIELD_N(L, n+1);
		game_load_card(L, &pGame->cur_discard_cards[n], 0);
		LOAD_END_FIELD(L);
	}
	LOAD_END_FIELD(L);

	LOAD_INT(pGame, round_num, L);
	LOAD_INT(pGame, round_player, L);
	LOAD_INT(pGame, cur_player, L);

	LOAD_ERROR_CHK(L, round_player, pGame->round_player >= 0 && pGame->round_player < pGame->player_count, "is out of range");
	LOAD_ERROR_CHK(L, cur_player, pGame->cur_player >= 0 && pGame->cur_player < pGame->player_count, "is out of range");

	LOAD_INT_CAST(pGame, status, L, Status);

	lua_pop(L, 2);   // [t]  

	lua_pop(L, 1);   

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

