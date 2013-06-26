#ifndef __GAME_H__
#define __GAME_H__


#include "config.h"
#include "player.h"
#include "card.h"
#include "card_stack.h"



// foward  decalare
typedef struct tagGameEventContext GameEventContext;
typedef struct tagOutCard  OutCard;
typedef struct tagPassiveOut PassiveOut;


enum Status
{
	Status_None = 0, // not start game yet
	Status_NewGame=1, // the first time all player get card
	Status_Round_Begin,
	Status_Round_Judge,
	Status_Round_Get,
	Status_Round_Out,
	Status_Round_Discard,
	Status_Round_End,
};

typedef struct tagGameContext GameContext;


struct tagGameContext
{
	int        nPlayerCount;
	int        nMinsterCount;
	int        nSpyCount;
	int        nMutineerCount;
	Player     players[MAX_PLAYER_NUM];
	CardStack  cardStack;
	CardStack  cardOut;
	int        nRoundNum;
	int        nRoundPlayer;
	int        nCurPlayer;
	Status     status;
	jmp_buf    __jb__;  // for quit a game directly
};


enum GameResult
{
	GameResult_Invalid = -1,
	GameResult_InPlaying = 0,
	GameResult_Canceled = 1,
	GameResult_WithDraw = 2,
	GameResult_MasterWin = 3,
	GameResult_MutineerWin = 4,
	GameResult_SpyWin = 5,
};



#define ROUND_PLAYER(pGame)   (&(pGame)->players[(pGame)->nRoundPlayer])
#define CUR_PLAYER(pGame)   (&(pGame)->players[(pGame)->nCurPlayer])
#define GAME_PLAYER(pGame, pl)   (&(pGame)->players[(pl)])



RESULT init_game_context(GameContext* pGame, int minsters, int spies, int mutineers);



RESULT game_loop(GameContext* pGame, GameEventContext* pEvent);
RESULT game_cur_info(GameContext* pGame, GameEventContext* pEvent);
RESULT game_global_info(GameContext* pGame, GameEventContext* pEvent);
RESULT game_other_player_info(GameContext* pGame, GameEventContext* pEvent, int player);



Status game_status(GameContext* pGame);
int get_game_cur_player(GameContext* pGame);
int get_game_round_player(GameContext* pGame);
int get_game_master_player(GameContext* pGame);

int game_next_player(GameContext* pGame, int player);
int game_prev_player(GameContext* pGame, int player);

int game_player_dis(GameContext* pGame, int p1, int p2);

RESULT game_player_discard_card(GameContext* pGame, GameEventContext* pParentEvent, int player, int where, int pos);

RESULT game_player_equip_card(GameContext* pGame, GameEventContext* pParentEvent, int player, int pos, Card* pCard);
RESULT game_player_add_life(GameContext* pGame, GameEventContext* pParentEvent, int player, int life_inc);

// select target process
RESULT game_select_target(GameContext* pGame, GameEventContext* pParentEvent, int player, int base_dist, YESNO self_select, YESNO may_cancel, const char* alter_text, int* out_target);



#endif /* __GAME_H__ */

