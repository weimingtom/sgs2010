#ifndef __HERO_H__
#define __HERO_H__


#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

// tolua_begin

typedef enum _HeroID
{
	HeroID_None = 0,
	// other hero ids are registered in script
	/*
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
	*/
}HeroID;


typedef enum _HeroSex
{
	HeroSex_None = 0,
	HeroSex_Male = 1,
	HeroSex_Female = 2,
}HeroSex;

typedef enum _HeroGroup
{
	HeroGroup_None = 0,
	HeroGroup_Wei,
	HeroGroup_Shu,
	HeroGroup_Wu,
	HeroGroup_People,
}HeroGroup;

typedef enum _SkillFlag
{
	SkillFlag_None = 0,    
	SkillFlag_Passive = 1, // 锁定技
	SkillFlag_Master = 2,  // 主公技
}SkillFlag;


// tolua_end


typedef struct tagGameContext GameContext;
typedef struct tagGameEventContext GameEventContext;

/*
typedef YESNO  (*SKILLCHECKFUN)(GameContext*, GameEventContext*, int);
typedef RESULT (*SKILLUSEFUN)(GameContext*, GameEventContext*, int);

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
	HeroID     id;
	char       name[MAX_NAME_LEN];
	int        life;
	YESNO      isMaster;
	HeroGroup  group;
	HeroSex    sex;
	char desc[MAX_DESC_LEN];
	int  skillNum;
	HeroSkill skills[MAX_HERO_SKILL_NUM];
} HeroConfig;




const HeroConfig* get_hero_config(HeroID id);

*/


// tolua_begin

const char* hero_group_str(HeroGroup group);
const char* hero_sex_str(HeroSex sex);

// tolua_end

// wrapper from lua to c code

HeroID hero_maxid(void);
HeroID hero_sid2id(const char* sid);
char* hero_sid(HeroID id, char* buf, int buflen);
char* hero_name(HeroID id, char* buf, int buflen);
char* hero_desc(HeroID id, char* buf, int buflen);

HeroGroup hero_group(HeroID id);
HeroSex hero_sex(HeroID id);

int hero_life(HeroID id);
YESNO hero_master(HeroID id);

int hero_skill_num(HeroID id);
char* hero_skill_name(HeroID id, int index, char* buf, int buflen);
int hero_skill_flag(HeroID id, int index);


CANUSE call_hero_skill_can_use(HeroID id, int index, GameContext* pGame, GameEventContext* pEvent, int player);
RESULT call_hero_skill_event(HeroID id, int index, GameContext* pGame, GameEventContext* pEvent, int player);


#ifdef __cplusplus
}
#endif



#endif /* __HERO_H__ */




