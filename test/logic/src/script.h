#ifndef __SCRIPT_H__
#define __SCRIPT_H__
#include "config.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagScriptHandle  *script_t;
typedef struct tagScriptObjectHandle  *script_object_t;
typedef int script_function_t;

script_t script_create();
void script_destroy(script_t s);


RESULT script_add_param_int(script_t s, int n);
RESULT script_add_param_float(script_t s, float f);
RESULT script_add_param_string(script_t s, const char* str);
RESULT script_add_param_obj(script_t s, script_object_t o);
RESULT script_add_param_cfun(script_t s, script_function_t fun);
RESULT script_call(script_t s);
void   script_raw_call(script_t s);
RESULT script_get_result_int(script_t s, int index, int* pn);
RESULT script_get_result_float(script_t s, int index, float* pf);
RESULT script_get_result_string(script_t s, int index, char* sz, int* szlen);
RESULT script_get_result_obj(script_t s, int index, script_object_t* po);



#ifdef __cplusplus
};
#endif



#endif /* __SCRIPT_H__ */



