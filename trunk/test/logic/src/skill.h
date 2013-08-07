// skill.h 

#ifndef __SKILL_H__
#define __SKILL_H__


#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif



typedef struct tagGameContext  GameContext;
typedef struct tagGameEventContext  GameEventContext;


typedef struct tagSkillCanUse
{
	HeroID  hero_id;
	int     skill_index;
	CANUSE  can_use;
}SkillCanUse;



typedef struct tagCardCanUse
{
	PosCard pos_card;
	CANUSE  can_use;
}CardCanUse;


RESULT game_cmd_use_skill(GameContext* pGame, GameEventContext* pEvent, int idx);
RESULT game_cmd_use_weapon(GameContext* pGame, GameEventContext* pEvent);
RESULT game_cmd_use_armor(GameContext* pGame, GameEventContext* pEvent);

RESULT game_cmd_cancelskill(GameContext* pGame, GameEventContext* pEvent);


CANUSE game_card_can_use(GameContext* pGame, GameEventContext* pEvent, int player, PosCard* pPosCard);
CANUSE game_skill_can_use(GameContext* pGame, GameEventContext* pEvent, int player, HeroID heroid, int skillindex);

#ifdef __cplusplus
}
#endif

#endif /* __SKILL_H__ */




