/**
 * ScriptItf.h : interface of script engine
 * 
 * 
 */

#ifndef __ITF_SCRIPT_H__
#define __ITF_SCRIPT_H__

struct IScript;
struct IScriptHostObject;
struct IScriptErrorInfo;



struct ScriptMethodParam
{
	struct varchar {
		size_t   len;
		char     str[1];
	};

	enum {
		t_null = 0,
		t_empty = t_null,
		t_long,
		t_double,
		t_varchar,
		t_object,
		/*
		t_long_out,
		t_double_out,
		t_varchar_out,
		t_object_out,
		*/
		t_unknown = -1,
	};
	int type;
	union {
		/* for input params */
		long        vl;
		double      vd;
		varchar*   vsp;
		IScriptHostObject* vo;
		/* for input/output params */
		/*
		long*                pvl;
		double*              pvd;
		var_char**           pvsp;
		IScriptHostObject**  pvo;
		*/
	};
};

#define SCRIPT_METHOD_NAME_MAX_LENGTH 128

struct ScriptMethodInfo
{
	char  name[SCRIPT_METHOD_NAME_MAX_LENGTH + 1];
	int   type;
	int   in_param_count;
	int   out_param_count;
};

enum enScriptMothodType
{
	enScriptMothodType_none = 0,     // not exist
	enScriptMothodType_method = 1,   // a method, can call it
	enScriptMothodType_prop_rw = 2,  // a property, can set/get it
	enScriptMothodType_prop_ro = 3,  // a property, can get it only
	enScriptMothodType_prop_wo = 4,  // a property, can set it only
	enScriptMothodType_prop_oo = 5,  // a property, cannot bot get and set 
};

enum enScriptMothodResult
{
	enScriptMothodResult_success = 0,
	enScriptMothodResult_failed = 1,
	enScriptMothodResult_out_of_memory = 2,
	enScriptMothodResult_not_exist = 3,
	enScriptMothodResult_invalid_Param = 4,
	enScriptMothodResult_invalid_Param_type = 5,
	enScriptMothodResult_invalid_Param_count = 6,
	enScriptMothodResult_invalid_index = 7,
	enScriptMothodResult_read_only = 8,
	enScriptMothodResult_write_only = 9,
	enScriptMothodResult_error_system = 99,
	enScriptMothodResult_error_custom_first = 100,
};

// host object that register to script engine
struct IScriptHostObject
{
public:
	virtual void release() = 0;
	virtual int getProperty(IScript* s, const char* name, ScriptMethodParam* out_value, IScriptErrorInfo** error_info) = 0;
	virtual int setProperty(IScript* s, const char* name, const ScriptMethodParam* in_value, IScriptErrorInfo** error_info) = 0;
	virtual int callMethod(IScript* s, const char* name, const ScriptMethodParam* in_params, int in_param_count, 
						ScriptMethodParam** out_params, int* out_params_count, IScriptErrorInfo** error_info) = 0;
	virtual int getMethodCount(IScript* s) = 0;
	virtual int findMethod(IScript* s, const char* name) = 0;
	virtual int getMethodInfo(IScript* s, int index, ScriptMethodInfo* out_info) = 0;
};

struct IScriptErrorInfo
{
public:
	virtual int getErrorInfo(int result_code, char* msg, int msg_len) = 0;
};

// script host
struct IScript
{
public:
	virtual void release() = 0;
	virtual bool addHostObject(const char* name, IScriptHostObject* host_object) = 0;
	virtual bool removeHostObject(const char* name) = 0;
	virtual int  call(const char* obj_name, const char* method_name, const ScriptMethodParam* in_params, int in_param_count, 
						ScriptMethodParam** out_params, int* out_params_count, IScriptErrorInfo** error_info) = 0;
	virtual int  get(const char* obj_name, const char* prop_name, ScriptMethodParam* out_value, IScriptErrorInfo** error_info) = 0
	virtual int  set(const char* obj_name, const char* prop_name, const ScriptMethodParam* out_value, IScriptErrorInfo** error_info) = 0
};


#endif /* __ITF_SCRIPT_H__ */

