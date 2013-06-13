#ifndef __COMM_H__
#define __COMM_H__


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

#endif /* __COMM_H__ */


