#ifndef __COMM_H__
#define __COMM_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef int(*COMPFUN)(const void*,const void*,void*);

void fill_array_inc_i(int* array, int size, int first, int inc);

void rand_array_i(int* array, int size, int rn);

void sort_array_i(int* array, int size, int (*comp)(int,int,void*), void* ud);

void rand_array_t(void* pv, int memsize, int count, int rn);

void sort_array_t(void* pv, int memsize, int count, COMPFUN comp, void* ud);

int arrray_insert_t(void* pv, int memsize, int* pCount, int pos, const void* val);
int arrray_remove_t(void* pv, int memsize, int* pCount, int pos, void* val);

int arrray_remove_value_first_t(void* pv, int memsize, int* pCount, const void* val, COMPFUN comp, void* ud);
int arrray_remove_value_last_t(void* pv, int memsize, int* pCount, const void* val, COMPFUN comp, void* ud);
int arrray_bsearch_t(void* pv, int memsize, int count, const void* val, COMPFUN comp, void* ud);



int compare_i(const void* a,const void* b,void*);
int compare_l(const void* a,const void* b,void*);
int compare_f(const void* a,const void* b,void*);
int compare_ul(const void* a,const void* b,void*);



char* fgetln(char* buffer, int len, FILE* file);

char* strtrim(char* buffer);

// fmt contian a multi-string, splitted with single 0, and end of twice 0.
int multi_snprintf(char* buffer, int len, const char* fmt, ...);


RESULT to_int(const char* text, int* pv);
RESULT to_uint(const char* text, unsigned int* pv);
RESULT to_float(const char* text, float* pv);


void log_text(const char* fmt, ...);

int message_printf(const char* fmt, ...);

#define MSG_OUT   message_printf

#ifdef OUTPUT_UTF8 
char* to_utf8(const char*  str, char* buffer, int len);
#define  A2UTF8(s,b,l)   to_utf8((s),(b),(l))
#else
#define  A2UTF8(s,b,l)   strncpy((b),(s),(l))
#endif


#ifdef __cplusplus
}
#endif

#endif /* __COMM_H__ */


