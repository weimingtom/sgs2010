#include "config.h"
#include "comm.h"


void fill_array_inc_i(int* array, int size, int first, int inc)
{
	int n;
	for(n = 0; n < size; n++)
	{
		array[n] = first;
		first += inc;
	}
}

void rand_array_i(int* array, int size, int rn)
{
	int t, a, b;

	while(rn-->0)
	{
		a = rand() % size;
		b = rand() % size;

		if(a != b)
		{
			t = array[a];
			array[a] = array[b];
			array[b] = t;
		}
	}
}

// shell sort
void sort_array_i(int* array, int size, int (*comp)(int,int,void*), void* ud)
{
	int i, j, inc;
	int t;

	// calc the start inc 
	inc = 1;
	do {
		inc = (inc << 2 ) + inc + 1;
	} while(inc <= size);

	// loop for part sort
	do {
		inc /= 3;
		// insert sort outer loop 
		for(i = inc; i < size; i++)
		{
			t = array[i];
			j = i;
			// insert sort inner loop
			while((*comp)(t, array[j - inc], ud) < 0)
			{
				array[j] = array[j - inc];
				j -= inc;
				if (j < inc)
					break;
			}
			array[j] = t;
		}
	} while(inc > 1);
}



void rand_array_t(void* pv, int memsize, int count, int rn)
{
	int a, b;
	void*  t;

	t = alloca(memsize);

	while(rn-->0)
	{
		a = rand() % count;
		b = rand() % count;

		if(a != b)
		{
			memcpy(t, (char*)pv + a * memsize, memsize);
			memcpy((char*)pv + a * memsize, (char*)pv + b * memsize, memsize);
			memcpy((char*)pv + b * memsize, t, memsize);
		}
	}
}

void sort_array_t(void* pv, int memsize, int count, COMPFUN comp, void* ud)
{
	int i, j, inc;
	void* t;

	t = alloca(memsize);

	// calc the start inc 
	inc = 1;
	do {
		inc = (inc << 2 ) + inc + 1;
	} while(inc <= count);

	// loop for part sort
	do {
		inc /= 3;
		// insert sort outer loop 
		for(i = inc; i < count; i++)
		{
			memcpy(t, (char*)pv + (i * memsize), memsize);
			j = i;
			// insert sort inner loop
			while((*comp)(t, (char*)pv + (j - inc) * memsize, ud) < 0)
			{
				memcpy((char*)pv + j * memsize, (char*)pv + (j - inc) * memsize, memsize);
				j -= inc;
				if (j < inc)
					break;
			}
			memcpy((char*)pv + j * memsize, t, memsize);
		}
	} while(inc > 1);

}



int arrray_insert_t(void* pv, int memsize, int* pCount, int pos, const void* val)
{
	if(pos < 0 || pos > *pCount)
	{
		pos = *pCount;
	}

	if(pos < *pCount)
	{
		memmove((char*)pv + (pos + 1) * memsize, (char*)pv + pos * memsize, memsize * (*pCount - pos));
	}
	memcpy((char*)pv + pos * memsize, val, memsize);

	(*pCount)++;

	return pos;
}

int arrray_remove_t(void* pv, int memsize, int* pCount, int pos, void* val)
{
	if(pos < 0 || pos >= *pCount)
	{
		return -1;
	}

	if(val)
	{
		memcpy(val, (char*)pv + pos * memsize, memsize);
	}

	if(pos < *pCount - 1)
	{
		memmove((char*)pv + pos * memsize, (char*)pv + (pos + 1) * memsize, memsize * (*pCount - pos - 1));
	}

	(*pCount)--;

	return pos;
}

int arrray_remove_value_first_t(void* pv, int memsize, int* pCount, const void* val, COMPFUN comp, void* ud)
{
	int n;
	for(n = 0; n < *pCount; n++)
	{
		if(0 == (*comp)(val, (char*)pv + n * memsize, ud))
		{
			return arrray_remove_t(pv, memsize, pCount, n, NULL);
		}
	}
	return -1;
}

int arrray_remove_value_last_t(void* pv, int memsize, int* pCount, const void* val, COMPFUN comp, void* ud)
{
	int n;
	for(n = *pCount - 1; n >= 0; n--)
	{
		if(0 == (*comp)(val, (char*)pv + n * memsize, ud))
		{
			return arrray_remove_t(pv, memsize, pCount, n, NULL);
		}
	}
	return -1;
}

int arrray_bsearch_t(void* pv, int memsize, int count, const void* val, COMPFUN comp, void* ud)
{
	int a, b, n;
	int c;

	a = 0; 
	b = count;

	while(a < b)
	{
		n = (a + b) / 2;
		c = (*comp)(val, (char*)pv + n * memsize, ud);

		if(c < 0)
		{
			b = n;
		}
		else if(c > 0)
		{
			a = n + 1;
		}
		else
		{
			return n;
		}
	}

	return -a - 1;
}


int compare_i(const void* a,const void* b,void*)
{
	return *(int*)a - *(int*)b;
}

int compare_l(const void* a,const void* b,void*)
{
	return *(long*)a - *(long*)b;
}

int compare_f(const void* a,const void* b,void*)
{
	if(*(float*)a < *(float*)b) return -1;
	if(*(float*)a > *(float*)b) return 1;
	return 0;
}

int compare_ul(const void* a,const void* b,void*)
{
	if(*(float*)a < *(float*)b) return -1;
	if(*(float*)a > *(float*)b) return 1;
	return 0;
}

