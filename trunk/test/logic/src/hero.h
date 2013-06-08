#ifndef __HERO_H__
#define __HERO_H__


#include "config.h"


enum HeroID
{
	HeroID_None = 0,
	HeroID_CaoCao = 1,
	HeroID_LiuBei = 2,
	HeroID_ShunQuan = 3,


	HeroID_Min = HeroID_CaoCao,
	HeroID_Max = HeroID_ShunQuan,
};


typedef struct  tagHeroConfig
{
	int  id;
	char name[MAX_NAME_LEN];
	int  life;
	int  isMaster;

} HeroConfig;




const HeroConfig* get_hero_config(int id);

int random_hero_id(int isMaster);


#endif /* __HERO_H__ */




