#ifndef __GAME_H__
#define __GAME_H__


#include "config.h"
#include "player.h"
#include "card.h"
#include "card_stack.h"


#ifdef __cplusplus
extern "C" {
#endif


// tolua_begin

#define MAX_CUR_DISCARD_NUM     32

// tolua_end

// foward  decalare
typedef struct tagGameEventContext GameEventContext;
typedef struct tagOutCard  OutCard;
typedef struct tagPatternOut PatternOut;


// tolua_begin
typedef enum _Status
{
	Status_None = 0, // not start game yet
	Status_NewGame=1, // the first time all player get card
	Status_Round_Begin,
	Status_Round_Judge,
	Status_Round_Get,
	Status_Round_Out,
	Status_Round_Discard,
	Status_Round_End,
}Status;

// tolua_end

typedef struct tagGameContext GameContext;


struct tagGameContext
{
	int        nPlayerCount;
	int        nMinsterCount;
	int        nSpyCount;
	int        nMutineerCount;
	Player     players[MAX_PLAYER_NUM];
	CardStack  stGetCardStack;              // card stack for get card from
	CardStack  stDiscardCardStack;          // card stack for discard card to
	int        nCurDiscardCardNum;          // the current out discard card number
	Card       stCurDiscardCards[MAX_CUR_DISCARD_NUM];           // ...
	int        nRoundNum;
	int        nRoundPlayer;
	int        nCurPlayer;
	Status     status;
	//jmp_buf    __jb__;  // for quit a game directly
	lua_State  *L;      // for run card/hero logic in lua engine
};

// tolua_begin
typedef enum _GameResult
{
	GameResult_Invalid = -1,
	GameResult_InPlaying = 0,
	GameResult_Canceled = 1,
	GameResult_WithDraw = 2,
	GameResult_MasterWin = 3,
	GameResult_MutineerWin = 4,
	GameResult_SpyWin = 5,
}GameResult;

// tolua_end

#define ROUND_PLAYER(pGame)   (&(pGame)->players[(pGame)->nRoundPlayer])
#define CUR_PLAYER(pGame)   (&(pGame)->players[(pGame)->nCurPlayer])
#define GAME_PLAYER(pGame, pl)   (&(pGame)->players[(pl)])


// game process base funs

RESULT init_game_context(GameContext* pGame, int minsters, int spies, int mutineers);
//RESULT game_loop(GameContext* pGame, GameEventContext* pEvent);
RESULT game_main(GameContext* pGame, GameEventContext* pEvent);

RESULT game_save(GameContext* pGame, const char* file_name);
RESULT game_load(GameContext* pGame, const char* file_name);

// get game base status
int get_game_cur_player(GameContext* pGame);
int get_game_round_player(GameContext* pGame);
int get_game_master_player(GameContext* pGame);

int game_next_player(GameContext* pGame, int player);
int game_prev_player(GameContext* pGame, int player);

RESULT set_game_cur_player(GameContext* pGame, int player);

/// calc player distance to other

int game_player_dis(GameContext* pGame, int p1, int p2);


// tolua_begin

Status get_game_status(GameContext* pGame);



// tolua_end




#ifdef __cplusplus
}
#endif





#endif /* __GAME_H__ */

