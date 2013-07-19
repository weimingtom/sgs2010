#ifndef __EQUIP_H__
#define __EQUIP_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct tagCard        Card;
typedef struct tagGameContext GameContext;
typedef struct tagGameEventContext GameEventContext;



typedef struct tagEquipCard
{
	int equip_pos;
	int supply;
	PosCard  card;
} EquipCard;



// tolua_begin


// read equipcard struct for script

int get_equipcard_equip_pos(EquipCard* equip_card);
int get_equipcard_supply(EquipCard* equip_card);
Card* get_equipcard_card(EquipCard* equip_card);
CardWhere get_equipcard_where(EquipCard* equip_card);
int get_equipcard_pos(EquipCard* equip_card);


// do equip a card 
RESULT game_player_equip_card(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, int hand_pos, int equip_pos);


// tolua_end


#ifdef __cplusplus
}
#endif

#endif /* __EQUIP_H__ */


