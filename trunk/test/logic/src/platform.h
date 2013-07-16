#ifndef __PLATFORM_H__
#define __PLATFORM_H__



#ifdef __cplusplus
extern "C" {
#endif


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





#ifdef __cplusplus
}
#endif


#endif

