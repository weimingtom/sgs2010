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


char* fgetln(char* buffer, int len, FILE* file)
{
	int n,c;

	for(n = 0; n < len-1; n++)
	{
		c = getchar();
		if(c == EOF)
		{
			if(n == 0)
				return NULL;
			break;
		}
		else if(c=='\n')
		{
			break;
		}
		else
		{
			buffer[n] = (char)c;
		}
	}
	buffer[n] = 0;

	return buffer;
}

char* strtrim(char* buffer)
{
	int b;
	int n = strlen(buffer) - 1;

	while(n >= 0 && isspace(C2I(buffer[n])))
	{
		buffer[n] = '\0';
		n--;
	}

	if(n < 0)
		return buffer;

	b = 0;
	while(buffer[b] != '\0' && isspace(C2I(buffer[b])))
	{
		b++;
	}

	memmove(buffer, buffer+b, n-b+1);
	buffer[n-b+1] = '\0';
	return buffer;
}



int multi_snprintf(char* buffer, int len, const char* fmt, ...)
{
	int n;
	int flen;
	const char* sf;

	va_list vl;

	va_start(vl, fmt);

	n = 0;
	sf = fmt;

	// for avoid the first empty string
	if(*sf == 0)
		sf++;

	while(*sf)
	{
		n += vsnprintf(buffer + n, len - n - 2, sf,  vl);
		n += 1;
		flen = strlen(sf);
		sf += flen + 1;
	}

	buffer[n] = 0;
	n++;

	va_end(vl);

	return n;
}



RESULT to_int(const char* text, int* pv)
{
	char* p;
	long l = strtol(text, &p, 0);

	while(*p && isspace(C2I(*p))) p++;

	if(*p != '\0')
		return R_E_FAIL;

	*pv = l;
	return R_SUCC;
}

RESULT to_uint(const char* text, unsigned int* pv)
{
	char* p;
	unsigned long ul = strtoul(text, &p, 0);

	while(*p && isspace(C2I(*p))) p++;

	if(*p != '\0')
		return R_E_FAIL;

	*pv = ul;
	return R_SUCC;

}

RESULT to_float(const char* text, float* pv)
{
	char* p;
	float f = (float)strtod(text, &p);

	while(*p && isspace(C2I(*p))) p++;

	if(*p != '\0')
		return R_E_FAIL;

	*pv = f;
	return R_SUCC;

}

void log_text(const char* fmt, ...)
{
	static char path [MAX_PATH];
	static int  init = 0;
	static int  sz;
	FILE* f;
	va_list vl;

	if(init == 0)
	{
		time_t  t = time(NULL);
		get_app_path(path, sizeof(path));
		sz = strlen(path);
		sz+=snprintf(path+sz, sizeof(path)-sz, "/log");
		mkdir(path, S_ISVTX);
		struct tm*  lt = localtime(&t);
		sz+=snprintf(path+sz, sizeof(path)-sz, "/logic_%04d%02d%02d_%02d%02d%02d.log", 
			lt->tm_year+1900,lt->tm_mon+1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
		init = 1;

		log_text("=========================== LOG START  ======================\n%s", ctime(&t));
	}

	f = fopen(path, "a+");

	if(f)
	{
		va_start(vl, fmt);
		vfprintf(f, fmt, vl);
		va_end(vl);

		fclose(f);
	}
}

#ifdef OUTPUT_UTF8
char* to_utf8(const char*  str, char* buffer, int len)
{
	iconv_t  conv;
	size_t   ulen;
	size_t   ol;
	size_t   sz;
	char*    inbuf;
	char*    outbuf;

	conv = iconv_open("UTF-8", "GBK");
	if(conv == (iconv_t)-1)
	{
		strncpy(buffer, str, len);
	}
	else
	{
		inbuf = (char*)str;
		sz = strlen(str);
		outbuf = buffer;
		ulen = len;
		ol = iconv(conv, &inbuf, &sz, &outbuf, &ulen);
		if(ol == (size_t)-1)
		{
			strncpy(buffer, str, len);
		}
		sz = len - ulen;
		if((int)sz >= len)
			sz = len - 1;
		buffer[sz] = 0;
		iconv_close(conv);
	}

	return buffer;
}
#endif


int message_printf(const char* fmt, ...)
{
	char text[4096];
	char utf8[4096*2];


	va_list vl;
	size_t sz;
	
	va_start(vl, fmt);
	sz = vsnprintf(text, sizeof(text), fmt, vl);
	va_end(vl);

	A2UTF8(text, utf8, sizeof(utf8));

	log_text("%s", utf8);

	return printf("%s", utf8);

}

