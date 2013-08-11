#ifndef __LOAD_H__
#define __LOAD_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagCard        Card;
typedef struct tagGameContext GameContext;

// operator of get card stack

RESULT game_load(GameContext* pGame, const char* file_name);


#ifdef __cplusplus
}
#endif

#endif /* __LOAD_H__ */


