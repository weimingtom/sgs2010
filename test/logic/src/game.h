#ifndef __GAME_H__
#define __GAME_H__


#include "config.h"
#include "player.h"
#include "card_stack.h"


enum Status
{
	Status_None = 0, // not start game yet
	Status_NewGame=1, // the first time all player get card
	Status_FirstGetCard, // the first time all player get card
	Status_Round_Begin,
	Status_Round_Judge,
	Status_Round_Get,
	Status_Round_Out,
	Status_Round_Discard,
	Status_Round_End,
};



typedef struct tagGameContext
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
	int        status;
} GameContext;


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


enum GameEvent
{
	GameEvent_None = 0,
	GameEvent_RoundBegin = 1,   // 回合开始时
	GameEvent_PerRoundJudge,    
	GameEvent_PostRoundJudge,
	GameEvent_PerJudgeCard,
	GameEvent_PostJudgeCard,
	GameEvent_PerRoundGet,
	GameEvent_PostRoundGet,
	GameEvent_PerGetCard,
	GameEvent_PostGetCard,
	GameEvent_PerRoundOut,
	GameEvent_PostRoundOut,
	GameEvent_PerOutCard,
	GameEvent_PostOutCard,
	GameEvent_PerCardCalc,
	GameEvent_PostCardCalc,
	GameEvent_PerRoundDiscard,
	GameEvent_PostRoundDiscard,
	GameEvent_PerDiscardCard,
	GameEvent_PostDiscardCard,
	GameEvent_RoundEnd,
	GameEvent_PerLostCard,
	GameEvent_PostLostCard,
	GameEvent_HandCardEmpty,
	GameEvent_PerAttack,
	GameEvent_PostAttack,
	GameEvent_PerBeAttacked,
	GameEvent_PostBeAttacked,
	GameEvent_PerLostLife,
	GameEvent_PostLostLife,
	GameEvent_LostOneLife,
	GameEvent_PerDie,
	GameEvent_PostDie,
	GameEvent_PerKill,
	GameEvent_PostKill,
	GameEvent_PerCardJudge,    
	GameEvent_PostCardJudge,
	GameEvent_PerCardJudgeCalc,    
	GameEvent_PostCardJudgeCalc,
	GameEvent_OutCardCheck,
	GameEvent_PassiveOutCard,
	GameEvent_SupplyCard,
};

enum EventResult
{
	Result_None = 0,
	Result_Yes = 1,
	Result_No  = 2,
	Result_Cancel = 3,
};

typedef struct tagGameEventContext GameEventContext;


struct tagGameEventContext
{
	int      id;
	int      trigger;
	int      target;
	GameEventContext* parent_event;
	int      result;
	int      block;
	Card     card;
	OutCard  out;
};


#define INIT_EVENT(event, eid, tr, tg, p)   \
do { \
	memset((event), 0, sizeof(*event)); \
	(event)->id = (eid); \
	(event)->trigger = (tr); \
	(event)->target = (tg); \
	(event)->parent_event = (p); \
	(event)->block = 0; \
	(event)->result = 0; \
} while(0)


int init_game_context(GameContext* pGame, int minsters, int spies, int mutineers);


int trigger_game_event(GameContext* pGame, GameEventContext* pEvent);

int game_continue(GameContext* pGame);
int game_cur_info(GameContext* pGame);
int game_global_info(GameContext* pGame);
int game_other_player_info(GameContext* pGame, int player);

int game_getcard(GameContext* pGame);
int game_outcard(GameContext* pGame, int idx);
int game_useskill(GameContext* pGame, int idx);


int game_status(GameContext* pGame);
int get_game_cur_player(GameContext* pGame);
int get_game_round_player(GameContext* pGame);

// 按指定的方式出牌
int game_appoint_out(GameContext* pGame, int player, int where, const Card* pCard, int num, int canCancel, const char* alter_text);

#endif /* __GAME_H__ */


