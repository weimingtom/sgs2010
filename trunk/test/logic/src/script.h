#ifndef __SCRIPT_H__
#define __SCRIPT_H__
#include "config.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagScriptHandle  *script_t;
typedef struct tagScriptObjectHandle  *script_object_t;


script_t script_create();
void script_destroy(script_t s);


RESULT script_push_int(script_t s, int n);
RESULT script_push_float(script_t s, float f);
RESULT script_push_string(script_t s, const char* str);
RESULT script_push_obj(script_t s, script_object_t o);
RESULT script_call(script_t s, int num_param, int num_ret);
RESULT script_pop_int(script_t s, int* pn);
RESULT script_pop_float(script_t s, float* pf);
RESULT script_pop_string(script_t s, char* sz_buffer, int* plen);
RESULT script_pop_object(script_t s, script_object_t* po);



#ifdef __cplusplus
};
#endif



#endif /* __SCRIPT_H__ */



