#ifndef __SAVE_H__
#define __SAVE_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagCard        Card;
typedef struct tagGameContext GameContext;

// operator of get card stack

RESULT game_save(GameContext* pGame, const char* file_name);


#ifdef __cplusplus
}
#endif

#endif /* __SAVE_H__ */


