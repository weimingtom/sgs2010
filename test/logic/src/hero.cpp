#include "config.h"
#include "hero.h"
#include "hero_impl/hero_impl.h"


const char* hero_group_str(int group)
{
	switch(group)
	{
	case HeroGroup_None: return "��";
	case HeroGroup_Wei: return "κ";
	case HeroGroup_Shu: return "��";
	case HeroGroup_Wu: return "��";
	case HeroGroup_People: return "Ⱥ��";
	}
	return "Invalid";
}

const char* hero_sex_str(int sex)
{
	switch(sex)
	{
	case HeroSex_None: return "��";
	case HeroSex_Male: return "��";
	case HeroSex_Female: return "Ů";
	}
	return "Invalid";	
}




const HeroConfig* get_hero_config(int id)
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
	}
	return NULL;
}


