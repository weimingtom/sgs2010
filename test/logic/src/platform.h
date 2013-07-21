#ifndef __PLATFORM_H__
#define __PLATFORM_H__


#ifdef WIN32



#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>
#include <io.h>


// windows specify definitions

#define snprintf      _snprintf
#define vsnprintf     _vsnprintf
#define strcasecmp    _stricmp
#define strncasecmp   _strnicmp
#define chdir         _chdir
#define getcwd        _getcwd
#define mkdir(s,m)    _mkdir(s)

#define __ATTR_FMT__(f,m,n)



#elif defined(linux) || defined (__linux__)

#ifndef LINUX
#define LINUX
#endif

// linux specified headers
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fnmatch.h>
#include <iconv.h>
#include <readline/readline.h>
#include <readline/history.h>


#define OUTPUT_UTF8   1 // output message use utf-8 encoding


#define __ATTR_FMT__(f,m,n)   __attribute__((format(f,m,n)));


#else

#error "unknow platform!"

#endif

// 放在这里，如果在cofig.h里定义，tolua编译时会出错 -_-!!!
#ifndef tolua_readonly
#define tolua_readonly
#endif


#define REPLACE_CHAR(str, chfrom, chto)  do { char* p = (str); while(*p) { if(*p == (chfrom) ) { *p = (chto); } p++; } } while(0)
#define REGULAR_PATH(str)  REPLACE_CHAR((str),'\\', '/')
#define WIN_PATH(str)  REPLACE_CHAR((str),'/', '\\')


#ifdef __cplusplus
extern "C" {
#endif


char* get_path_name(const char* file_path, char* buf, int buflen);
char* get_app_path(char*  buf, int buflen);

char* get_cur_path(char*  buf, int buflen);
int set_cur_path(const char* path);



#ifdef __cplusplus
}
#endif


#endif

