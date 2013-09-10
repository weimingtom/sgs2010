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
	HeroID_Begin = 1,
	HeroID_Unknown = -1,
	// other hero ids are registered in script

	HeroID_Marked = 1000,
	HeroID_Max = 1001,
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
	SkillFlag_Passive = 1, // ������
	SkillFlag_Master = 2,  // ������
}SkillFlag;


// tolua_end


typedef struct tagGameContext GameContext;
typedef struct tagGameEventContext GameEventContext;




// tolua_begin

const char* hero_group_str(HeroGroup group);
const char* hero_sex_str(HeroSex sex);

// tolua_end

// wrapper from lua to c code

HeroID hero_maxid(void);
int hero_id_valid(HeroID id);
int hero_sid_valid(const char* sid);
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

/*
�������¼��Է���ֵ������Ҫ���⣨ͨ����ǿ�ƻص��¼�,��OutCard����������ͨ���¼���Ӧ�ķ���ֵ�������£�

R_SUCC   -  �¼�ִ�гɹ�������ִ����һ��Handle
R_DEF    -  �¼�ִ���޽��������ִ����һ��Handle
R_BACK   -  �¼�ִ���޽�������жϸ��¼��Ĵ������ش���������
R_CANCEL -  �¼�ִ��ʧ�ܣ� ���жϸ��¼��Ĵ������ش���������
R_DONE   -  �¼�ִ�гɹ������жϸ��¼��Ĵ������ش���������ĳЩ�����Ӧ������������Ĺ��̡�

ĳЩ�¼�����Է���ֵ��ĳЩ�¼�����ֻ�������޼�������ֵ��������Ĭ�ϴ����ɲμ��¼�����ϸ˵��
*/
RESULT call_hero_skill_event(HeroID id, int index, GameContext* pGame, GameEventContext* pEvent, int player);


char* get_hero_sid(HeroID id);
char* get_hero_name(HeroID id);
char* get_hero_desc(HeroID id);
char* get_hero_skill_name(HeroID id, int index);


#ifdef __cplusplus
}
#endif



#endif /* __HERO_H__ */




