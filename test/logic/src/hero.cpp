#include "config.h"
#include "hero.h"


static 	HeroConfig  s_config_CaoCao = {
	HeroID_CaoCao,
	"�ܲ�",
	4,
	1,
};

static 	HeroConfig  s_config_LiuBei = {
	HeroID_LiuBei,
	"����",
	4,
	1,
};

static 	HeroConfig  s_config_SunQuan = {
	HeroID_SunQuan,
	"��Ȩ",
	3,
	1,
};


static 	HeroConfig  s_config_GuanYu = {
	HeroID_GuanYu,
	"����",
	3,
	0,
};

static 	HeroConfig  s_config_ZhangFei = {
	HeroID_ZhangFei,
	"�ŷ�",
	3,
	0,
};


static 	HeroConfig  s_config_ZhaoYun = {
	HeroID_ZhaoYun,
	"����",
	3,
	0,
};

static 	HeroConfig  s_config_MaChao = {
	HeroID_MaChao,
	"��",
	3,
	0,
};

static 	HeroConfig  s_config_XiaHouDun = {
	HeroID_XiaHouDun,
	"�ĺ",
	3,
	0,
};


static 	HeroConfig  s_config_ZhangHe = {
	HeroID_ZhangHe,
	"���",
	3,
	0,
};


static 	HeroConfig  s_config_LuXun = {
	HeroID_LuXun,
	"½ѷ",
	3,
	0,
};


const HeroConfig* get_hero_config(int id)
{
	switch(id)
	{
	case HeroID_CaoCao: return &s_config_CaoCao;
	case HeroID_LiuBei: return &s_config_LiuBei;
	case HeroID_SunQuan: return &s_config_SunQuan;
	case HeroID_GuanYu: return &s_config_GuanYu;
	case HeroID_ZhangFei: return &s_config_ZhangFei;
	case HeroID_ZhaoYun: return &s_config_ZhaoYun;
	case HeroID_MaChao: return &s_config_MaChao;
	case HeroID_XiaHouDun: return &s_config_XiaHouDun;
	case HeroID_ZhangHe: return &s_config_ZhangHe;
	case HeroID_LuXun: return &s_config_LuXun;
	}
	return NULL;
}



int random_hero_id(int isMaster)
{
	return rand() % (HeroID_Max-HeroID_Min+1) + HeroID_Min;
}


