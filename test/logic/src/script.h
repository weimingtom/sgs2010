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

RESULT script_create_object_table(script_t s, script_object_t* po);
RESULT script_create_object_int(script_t s, script_object_t* po, int val);
RESULT script_create_object_float(script_t s, script_object_t* po, float val);
RESULT script_destroy_object(script_t s, script_object_t o);


RESULT script_load_object(script_t s, const char* str);
RESULT script_get_global_object(script_t s, const char* str, script_object_t* po);
RESULT script_get_object_field_int(script_t s, const char* str, script_object_t o, script_object_t index);


#ifdef __cplusplus
};
#endif



#endif /* __SCRIPT_H__ */



