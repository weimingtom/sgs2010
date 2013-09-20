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


RESULT script_test_list();


RESULT script_test_run(GameContext* pGame, int index);

// 继续测试
RESULT script_test_continue(const char* msg, int msg_sz, char* buf, int len, int* mode);

// 协程处理出错
void script_test_error();

// wrapped function from lua
char* get_enumt_str(const char* enum_type, int val, int is_bit_or, const char* or_splitter, const char* or_head, const char* or_tail,  char* buf, int buflen);
#define  ENUM2STR(type, val)   get_enumt_str(#type, (val), 0, NULL, NULL, NULL, NULL, 0)
#define  BITOR2STRC(type, val)  get_enumt_str(#type, (val), 1, "|", NULL, NULL, NULL, 0)
#define  BITOR2STRL(type, val)  get_enumt_str(#type, (val), 1, ", ", "bitor(", ")", NULL, 0)



#define GL(L)  ((L)?(L): get_game_script())


#ifdef __cplusplus
};
#endif



#endif /* __SCRIPT_H__ */



