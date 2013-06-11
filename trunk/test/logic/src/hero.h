#ifndef __HERO_H__
#define __HERO_H__


#include "config.h"


enum HeroID
{
	HeroID_None = 0,
	HeroID_CaoCao = 1,
	HeroID_LiuBei = 2,
	HeroID_SunQuan = 3,

	HeroID_GuanYu = 4,
	HeroID_ZhangFei = 5,
	HeroID_ZhaoYun = 6,
	HeroID_MaChao = 7,
	HeroID_XiaHouDun = 8,
	HeroID_ZhangHe = 9,
	HeroID_LuXun = 10,


	HeroID_Min = HeroID_CaoCao,
	HeroID_Max = HeroID_LuXun,

};


enum HeroGroup
{
	HeroGroup_None = 0,
	HeroGroup_Wei,
	HeroGroup_Shu,
	HeroGroup_Wu,
	HeroGroup_People,
};

enum SkillFlag
{
	SkillFlag_None = 0,    
	SkillFlag_Passive = 1, // 锁定技
	SkillFlag_Master = 2,  // 主公技
};

typedef struct tagGameContext GameContext;
typedef struct tagGameEventContext GameEventContext;
typedef YESNO  (*SKILLCHECKFUN)(GameContext*, GameEventContext*, int);
typedef int  (*SKILLUSEFUN)(GameContext*, GameEventContext*, int);

typedef struct tagHeroSkill
{
	char name[MAX_NAME_LEN];
	char desc[MAX_DESC_LEN];
	char flag;
	SKILLCHECKFUN  check;
	SKILLUSEFUN   use;
} HeroSkill;


typedef struct  tagHeroConfig
{
	int  id;
	char name[MAX_NAME_LEN];
	int  life;
	int  isMaster;
	int  group;
	char desc[MAX_NAME_LEN];
} HeroConfig;




const HeroConfig* get_hero_config(int id);

int random_hero_id(int isMaster);


#endif /* __HERO_H__ */




