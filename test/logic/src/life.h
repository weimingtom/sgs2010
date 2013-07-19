#ifndef __LIFE_H__
#define __LIFE_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif
	

typedef struct tagGameContext  GameContext;
typedef struct tagGameEventContext  GameEventContext;
typedef struct tagOutCard  OutCard;


typedef struct tagChangeLife
{
	int  delta;    // how many life is changed
	int  after_life; 
	int  src_player;  // the damage come from (player index)
	OutCard   src_cards; // the damage come from (cards) if damage is not from card, it will be empty
	int  src_skill;   // if the damage comes rom a skill, otherwise 0;
} ChangeLife;


// tolua_begin

// script exports

int  get_changelife_delta(ChangeLife* change_life);
int  get_changelife_after_life(ChangeLife* change_life);
int  get_changelife_src_player(ChangeLife* change_life);
OutCard*  get_changelife_src_cards(ChangeLife* change_life);
int  get_changelife_src_skill(ChangeLife* change_life);

void  set_changelife_delta(ChangeLife* change_life, int delta);


RESULT game_player_add_life(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, int life_inc, int src_player, OutCard* src_cards, int src_skill);



// tolua_end


#ifdef __cplusplus
}
#endif


#endif /* __LIFE_H__ */
