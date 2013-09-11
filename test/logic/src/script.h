#ifndef __SCRIPT_H__
#define __SCRIPT_H__
#include "config.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagGameContext GameContext;


RESULT  init_game_script();
lua_State* get_game_script();
void  close_game_script();
RESULT  reload_game_script();


int script_pcall (lua_State *L, int narg, int result);

lua_State* get_ai_script();


// ¼ÌÐø²âÊÔ
RESULT script_test_continue(GameContext* pGame, const char* msg, int msg_sz, char* buf, int len);


// wrapped function from lua
char* get_enumt_str(const char* enum_type, int val, int is_bit_or, char* buf, int buflen);
#define  ENUM2STR(type, val)   get_enumt_str(#type, (val), 0, NULL, 0)
#define  BITOR2STR(type, val)  get_enumt_str(#type, (val), 1, NULL, 0)



#define GL(L)  ((L)?(L): get_game_script())


#ifdef __cplusplus
};
#endif



#endif /* __SCRIPT_H__ */



