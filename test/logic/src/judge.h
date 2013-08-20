#ifndef __JUDGE_H__
#define __JUDGE_H__

#include "config.h"


#ifdef __cplusplus
extern "C" {
#endif


// tolua_begin

typedef struct tagJudgeCard
{
	PosVCard  pos_vcard;
	int       judge_result;
	char      ud[MAX_UD_LEN];

} JudgeCard ;

// tolua_end


RESULT game_round_do_judge(GameContext* pGame, GameEventContext* pEvent, int player);



#ifdef __cplusplus
}
#endif


#endif /* __JUDGE_H__ */

