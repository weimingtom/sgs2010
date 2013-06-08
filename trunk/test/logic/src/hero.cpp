#include "config.h"
#include "hero.h"


static 	HeroConfig  s_config_CaoCao = {
	HeroID_CaoCao,
	"ChaoCao",
	4,
	1,
};

static 	HeroConfig  s_config_LiuBei = {
	HeroID_LiuBei,
	"LiuBei",
	4,
	1,
};

static 	HeroConfig  s_config_ShunQuan = {
	HeroID_ShunQuan,
	"ShunQuan",
	3,
	1,
};


const HeroConfig* get_hero_config(int id)
{
	switch(id)
	{
	case HeroID_CaoCao: return &s_config_CaoCao;
	case HeroID_LiuBei: return &s_config_LiuBei;
	case HeroID_ShunQuan: return &s_config_ShunQuan;
	}
	return NULL;
}



int random_hero_id(int isMaster)
{
	return rand() % (HeroID_Max-HeroID_Min+1) + HeroID_Min;
}
