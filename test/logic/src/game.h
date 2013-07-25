#ifndef __GAME_H__
#define __GAME_H__


#include "config.h"
#include "player.h"
#include "card.h"
#include "card_stack.h"


#ifdef __cplusplus
extern "C" {
#endif



// foward  decalare
typedef struct tagGameEventContext GameEventContext;
typedef struct tagOutCard  OutCard;
typedef struct tagPatternOut PatternOut;

// tolua_begin

#define MAX_CUR_DISCARD_NUM     32

#define INVALID_PLAYER         -1

// tolua_end

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


typedef enum _DisCardReson
{
	DisCardReson_None = 0,
	DisCardReson_Out,
	DisCardReson_PassiveOut,
	DisCardReson_Discard,
	DisCardReson_PassiveDiscard,
	DisCardReson_DecideCard,
	DisCardReson_Judgment,
} DisCardReson;

typedef struct tagDisCard
{
	Card        card;
	int         from_player;
	CardWhere   from_where;
	DisCardReson   reson;
} DisCard;

// tolua_end


typedef struct tagGameContext GameContext;


struct tagGameContext
{
	int        player_count;
	int        minster_count;
	int        spy_count;
	int        mutineer_count;
	Player     players[MAX_PLAYER_NUM];
	CardStack  get_card_stack;              // card stack for get card from
	CardStack  discard_card_stack;          // card stack for discard card to
	int        cur_discard_card_num;          // the current out discard card number
	Card       cur_discard_cards[MAX_CUR_DISCARD_NUM];           // ...
	int        round_num;
	int        round_player;
	int        cur_player;
	Status     status;
	//jmp_buf    __jb__;  // for quit a game directly
	//lua_State  *L;      // for run card/hero logic in lua engine
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

#define ROUND_PLAYER(pGame)   (&(pGame)->players[(pGame)->round_player])
#define CUR_PLAYER(pGame)   (&(pGame)->players[(pGame)->cur_player])
#define GAME_PLAYER(pGame, pl)   (&(pGame)->players[(pl)])

#define IS_PLAYER_VALID(pGame, p) ((p)>=0 && (p)<(pGame)->player_count && (pGame)->players[(p)].status != PlayerStatus_Dead)

// game process base funs

RESULT init_game_context(GameContext* pGame, int minsters, int spies, int mutineers);
//RESULT game_loop(GameContext* pGame, GameEventContext* pEvent);
RESULT game_main(GameContext* pGame, GameEventContext* pEvent);

RESULT game_save(GameContext* pGame, const char* file_name);
RESULT game_load(GameContext* pGame, const char* file_name);


RESULT set_game_cur_player(GameContext* pGame, int player);

// if the game is over, this function do not return.
void game_check_gameover(GameContext* pGame, int player);




// tolua_begin
//{{

// get game base status
int get_game_cur_player(GameContext* pGame);
int get_game_round_player(GameContext* pGame);
int get_game_master_player(GameContext* pGame);

int game_next_player(GameContext* pGame, int player);
int game_prev_player(GameContext* pGame, int player);

Status get_game_status(GameContext* pGame);

const char* game_status_str(Status s);


/// calc player distance to other

int game_player_dis(GameContext* pGame, int p1, int p2);

Player* get_game_player(GameContext* pGame, int player);



//}}
// tolua_end




#ifdef __cplusplus
}
#endif





#endif /* __GAME_H__ */

