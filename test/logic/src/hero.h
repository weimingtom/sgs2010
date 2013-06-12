#ifndef __HERO_H__
#define __HERO_H__


#include "config.h"


enum HeroID
{
	HeroID_None = 0,
	HeroID_CaoCao = 1,
	HeroID_LiuBei,
	HeroID_SunQuan,

	HeroID_GuanYu,
	HeroID_ZhangFei,
	HeroID_ZhaoYun,
	HeroID_MaChao,
	HeroID_XiaHouDun,
	HeroID_ZhangLiao,
	HeroID_LuXun,
	HeroID_SunShangXiang,


	HeroID_Max,

};


enum HeroSex
{
	HeroSex_None = 0,
	HeroSex_Male = 1,
	HeroSex_Female = 2,
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


#define MAX_HERO_SKILL_NUM    5

typedef struct  tagHeroConfig
{
	int  id;
	char name[MAX_NAME_LEN];
	int  life;
	YESNO  isMaster;
	int  group;
	int  sex;
	char desc[MAX_DESC_LEN];
	int  skillNum;
	HeroSkill skills[MAX_HERO_SKILL_NUM];
} HeroConfig;




const HeroConfig* get_hero_config(int id);



#endif /* __HERO_H__ */




