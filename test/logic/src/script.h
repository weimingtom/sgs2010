#ifndef __SCRIPT_H__
#define __SCRIPT_H__
#include "config.h"


#ifdef __cplusplus
extern "C" {
#endif

RESULT  init_game_script();
lua_State* get_game_script();
void  close_game_script();
RESULT  reload_game_script();

lua_State* get_ai_script();



#ifdef __cplusplus
};
#endif



#endif /* __SCRIPT_H__ */



