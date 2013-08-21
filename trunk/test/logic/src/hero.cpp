#include "config.h"
#include "hero.h"
#include "script.h"
//#include "hero_impl/hero_impl.h"


const char* hero_group_str(HeroGroup group)
{
	switch(group)
	{
	case HeroGroup_None: return "无";
	case HeroGroup_Wei: return "魏";
	case HeroGroup_Shu: return "蜀";
	case HeroGroup_Wu: return "吴";
	case HeroGroup_People: return "群雄";
	}
	return "Invalid";
}

const char* hero_sex_str(HeroSex sex)
{
	switch(sex)
	{
	case HeroSex_None: return "无";
	case HeroSex_Male: return "男";
	case HeroSex_Female: return "女";
	}
	return "Invalid";	
}



/*
const HeroConfig* get_hero_config(HeroID id)
{
	switch(id)
	{
	case HeroID_CaoCao: return get_hero_cacao();
	case HeroID_LiuBei: return  get_hero_liube();
	case HeroID_SunQuan: return get_hero_sunquan();
	case HeroID_GuanYu: return get_hero_guanyu();
	case HeroID_ZhangFei: return get_hero_zhangfei();
	case HeroID_ZhaoYun: return get_hero_zhaoyu();
	case HeroID_MaChao: return get_hero_machao();
	case HeroID_XiaHouDun: return get_hero_xiahoudun();
	case HeroID_ZhangLiao: return get_hero_zhanliao();
	case HeroID_LuXun: return get_hero_luxun();
	case HeroID_SunShangXiang: return get_hero_sunshangxiang();
	default: return NULL;
	}
	return NULL;
}
*/


/////////////////////////////////////////////////////////////////////////////////
//


HeroID hero_maxid(void)
{
	HeroID id = HeroID_None;
	lua_State* L = get_game_script();
	lua_getglobal(L, "get_hero_maxid");
	lua_call(L, 0, 1);
	if(lua_isnumber(L, -1))
	{
		id = (HeroID)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return id;
}

int hero_id_valid(HeroID id)
{
	int r = 0;
	lua_State* L = get_game_script();
	lua_getglobal(L, "is_hero_id_valid");
	lua_pushnumber(L, id);	
	lua_call(L, 1, 1);
	r = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return r;
}

int hero_sid_valid(const char* sid)
{
	int r = 0;
	lua_State* L = get_game_script();
	lua_getglobal(L, "is_hero_sid_valid");
	lua_pushstring(L, sid);	
	lua_call(L, 1, 1);
	r = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return r;
}



HeroID hero_sid2id(const char* sid)
{
	HeroID id = HeroID_None;
	lua_State* L = get_game_script();
	lua_getglobal(L, "get_hero_id_by_sid");
	lua_pushstring(L, sid);
	lua_call(L, 1, 1);
	if(lua_isnumber(L, -1))
	{
		id = (HeroID)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return id;
}

char* hero_sid(HeroID id, char* buf, int buflen)
{
	lua_State* L = get_game_script();
	lua_getglobal(L, "get_hero_sid");
	lua_pushnumber(L, id);	
	lua_call(L, 1, 1);
	if(lua_isstring(L, -1))
	{
		strncpy(buf, lua_tostring(L, -1), buflen);
	}
	else
	{
		buf[0] = 0;
	}
	lua_pop(L, 1);
	return buf;
}

char* hero_name(HeroID id, char* buf, int buflen)
{
	lua_State* L = get_game_script();
	lua_getglobal(L, "get_hero_name");
	lua_pushnumber(L, id);	
	lua_call(L, 1, 1);
	if(lua_isstring(L, -1))
	{
		strncpy(buf, lua_tostring(L, -1), buflen);
	}
	else
	{
		buf[0] = 0;
	}
	lua_pop(L, 1);
	return buf;
}

char* hero_desc(HeroID id, char* buf, int buflen)
{
	lua_State* L = get_game_script();
	lua_getglobal(L, "get_hero_desc");
	lua_pushnumber(L, id);	
	lua_call(L, 1, 1);
	if(lua_isstring(L, -1))
	{
		strncpy(buf, lua_tostring(L, -1), buflen);
	}
	else
	{
		buf[0] = 0;
	}
	lua_pop(L, 1);
	return buf;
}

HeroGroup hero_group(HeroID id)
{
	HeroGroup  g  =  HeroGroup_None;
	lua_State* L = get_game_script();
	lua_getglobal(L, "get_hero_group");
	lua_pushnumber(L, id);	
	lua_call(L, 1, 1);
	if(lua_isnumber(L, -1))
	{
		g = (HeroGroup)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return g;

}

HeroSex hero_sex(HeroID id)
{
	HeroSex  sex  =  HeroSex_None;
	lua_State* L = get_game_script();
	lua_getglobal(L, "get_hero_sex");
	lua_pushnumber(L, id);	
	lua_call(L, 1, 1);
	if(lua_isnumber(L, -1))
	{
		sex = (HeroSex)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return sex;

}

int hero_life(HeroID id)
{
	int life  =  0;
	lua_State* L = get_game_script();
	lua_getglobal(L, "get_hero_life");
	lua_pushnumber(L, id);	
	lua_call(L, 1, 1);
	if(lua_isnumber(L, -1))
	{
		life = (int)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return life;
}

YESNO hero_master(HeroID id)
{
	YESNO m  =  NO;
	lua_State* L = get_game_script();
	lua_getglobal(L, "get_hero_master");
	lua_pushnumber(L, id);	
	lua_call(L, 1, 1);
	if(lua_isnumber(L, -1))
	{
		m = (YESNO)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return m;
	
}

int hero_skill_num(HeroID id)
{
	int num  =  0;
	lua_State* L = get_game_script();
	lua_getglobal(L, "get_hero_skill_num");
	lua_pushnumber(L, id);	
	lua_call(L, 1, 1);
	if(lua_isnumber(L, -1))
	{
		num = (int)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return num;
}

char* hero_skill_name(HeroID id, int index, char* buf, int buflen)
{
	lua_State* L = get_game_script();
	lua_getglobal(L, "get_hero_skill_name");
	lua_pushnumber(L, id);	
	lua_pushnumber(L, index);	
	lua_call(L, 2, 1);
	if(lua_isstring(L, -1))
	{
		strncpy(buf, lua_tostring(L, -1), buflen);
	}
	else
	{
		buf[0] = 0;
	}
	lua_pop(L, 1);
	return buf;
}

int hero_skill_flag(HeroID id, int index)
{
	int f  =  0;
	lua_State* L = get_game_script();
	lua_getglobal(L, "get_hero_skill_flag");
	lua_pushnumber(L, id);	
	lua_pushnumber(L, index);	
	lua_call(L, 2, 1);
	if(lua_isnumber(L, -1))
	{
		f = (int)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return f;

}


CANUSE call_hero_skill_can_use(HeroID id, int index, GameContext* pGame, GameEventContext* pEvent, int player)
{
	CANUSE ret = USE_CANNOT;
	lua_State* L = get_game_script();
	lua_getglobal(L, "call_hero_skill_can_use");	
	tolua_pushnumber(L, id);
	tolua_pushnumber(L, index);
	tolua_pushusertype(L, pGame, "GameContext");
	tolua_pushusertype(L, pEvent, "GameEventContext");
	tolua_pushnumber(L, player);
	lua_call(L, 5, 1);
	if(lua_isnumber(L, -1))
	{
		ret = (CANUSE)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return ret;
}

RESULT call_hero_skill_event(HeroID id, int index, GameContext* pGame, GameEventContext* pEvent, int player)
{
	RESULT ret = R_DEF;
	lua_State* L = get_game_script();
	lua_getglobal(L, "call_hero_skill_event");	
	tolua_pushnumber(L, id);
	tolua_pushnumber(L, index);
	tolua_pushusertype(L, pGame, "GameContext");
	tolua_pushusertype(L, pEvent, "GameEventContext");
	tolua_pushnumber(L, player);

	lua_call(L, 5, 1);
	if(lua_isnumber(L, -1))
	{
		ret = (RESULT)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return ret;
}

char* get_hero_sid(HeroID id)
{
	static char buf[128];
	return hero_sid(id, buf, sizeof(buf));
}

char* get_hero_name(HeroID id)
{
	static char buf[128];
	return hero_name(id, buf, sizeof(buf));
}

char* get_hero_desc(HeroID id)
{
	static char buf[1024];
	return hero_desc(id, buf, sizeof(buf));
}

char* get_hero_skill_name(HeroID id, int index)
{
	static char buf[128];
	return hero_skill_name(id, index, buf, sizeof(buf));
}



