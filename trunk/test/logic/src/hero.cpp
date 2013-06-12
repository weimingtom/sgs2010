#include "config.h"
#include "hero.h"


extern const HeroConfig* get_hero_cacao();
extern const HeroConfig* get_hero_liube();
extern const HeroConfig* get_hero_sunquan();
extern const HeroConfig* get_hero_guanyu();
extern const HeroConfig* get_hero_zhangfei();
extern const HeroConfig* get_hero_zhaoyu();
extern const HeroConfig* get_hero_machao();
extern const HeroConfig* get_hero_xiahoudun();
extern const HeroConfig* get_hero_zhanliao();
extern const HeroConfig* get_hero_luxun();
extern const HeroConfig* get_hero_sunshangxiang();



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


