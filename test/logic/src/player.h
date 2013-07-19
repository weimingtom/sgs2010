#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "card.h"
#include "hero.h"
#include "out.h"


#ifdef __cplusplus
extern "C" {
#endif
	
	
// tolua_begin

enum PlayerID
{
	PlayerID_Unknown = -1,
	PlayerID_None = 0,
	PlayerID_Master = 1,
	PlayerID_Minster = 2,
	PlayerID_Spy = 3,
	PlayerID_Mutineer = 4,


	PlayerID_Valid_Min = PlayerID_Master,
	PlayerID_Valid_Max = PlayerID_Mutineer,

};


enum PlayerStatus
{
	PlayerStatus_Hide = 0,
	PlayerStatus_Show = 1,
	PlayerStatus_Dead = 2,
};


enum PlayerFlag
{
	PlayerFlag_None = 0,
	PlayerFlag_SkipNextRound = 0x1,
	PlayerFlag_SkipThisRound = 0x2,
	PlayerFlag_SkipThisRoundJudge = 0x4,
	PlayerFlag_SkipThisRoundGet = 0x8,
	PlayerFlag_SkipThisRoundOut = 0xf,
	PlayerFlag_SkipThisRoundDiscard = 0x10,
	PlayerFlag_AllThisSkipFlag = PlayerFlag_SkipThisRound
		|PlayerFlag_SkipThisRoundJudge|PlayerFlag_SkipThisRoundGet
		|PlayerFlag_SkipThisRoundOut|PlayerFlag_SkipThisRoundDiscard,
	PlayerFlag_AllSkipFlag = PlayerFlag_SkipNextRound|PlayerFlag_AllThisSkipFlag,
};



#define MAX_HAND_CARD   20
#define MAX_JUDGMENT_CARD  8
#define MAX_PLAYER_PARAM  10

#define MAX_OUT_CARD   40


enum EquipIdx {
	EquipIdx_Weapon = 0,
	EquipIdx_Armor,
	EquipIdx_HorseInc,
	EquipIdx_HorseDec,
	EquipIdx_Max,
};


// tolua_end

typedef  struct  tagPlayer
{
	PlayerID id;
	HeroID   hero;
	int      max_life;
	int      cur_life;
	char     name[MAX_NAME_LEN];
	int      hand_card_num;
	int      judgment_card_num;
	Card     hand_cards[MAX_HAND_CARD];
	Card     equip_cards[EquipIdx_Max];
	Card     judgment_cards[MAX_JUDGMENT_CARD];
	PlayerStatus   status;
	PlayerFlag     flag;
	int      params[MAX_PLAYER_PARAM];
} Player;




//#define PLAYER_CARD_WHERE(pos)     ((pos>>8) & 0xff)
//#define PLAYER_CARD_INDEX(pos)     ((pos) & 0xff)

#define IS_PLAYER_DEAD(p)   ((p)->status == PlayerStatus_Dead)
#define IS_PLAYER_SHOW(p)  ((p)->status == PlayerStatus_Show)

#define IS_PLAYER_PERDEAD(p)   ((p)->status != PlayerStatus_Dead && (p)->cur_life <= 0)


#define PLAYER_CHK_FLAG(p,f)  (((p)->flag & f) == f)
#define PLAYER_SET_FLAG(p,f)  ((p)->flag |= f)
#define PLAYER_CLR_FLAG(p,f)  ((p)->flag &= ~f)
#define PLAYER_CLR_ALL_FLAG(p)  ((p)->flag = PlayerFlag_None)


#define PLAYER_WEAPON(p)   (&(p)->equip_cards[EquipIdx_Weapon])
#define PLAYER_ARMOR(p)   (&(p)->equip_cards[EquipIdx_Armor])
#define PLAYER_HORSEINC(p)   (&(p)->equip_cards[EquipIdx_HorseInc])
#define PLAYER_HORSEDEC(p)   (&(p)->equip_cards[EquipIdx_HorseDec])

#define PLAYER_HANDCARD(p, idx)   (&(p)->hand_cards[idx])

#define PLAYER_JUDGECARD(p, idx)   (&(p)->judgment_cards[idx])

RESULT init_player(Player* pPlayer, PlayerID id, HeroID hero);



RESULT player_add_hand_card(Player* pPlayer, Card* pCard);


RESULT set_player_card_flag(Player* pPlayer, CardWhere where, int pos, CardFlag flag);

RESULT player_add_judge_card(Player* pPlayer, Card* pCard);

RESULT get_player_card(Player* pPlayer, CardWhere where, int pos, Card* pCard);
RESULT player_remove_card(Player* pPlayer, CardWhere where, int pos, Card* pCard);

RESULT player_add_judge_card(Player* pPlayer, Card* pCard);
RESULT player_card_idx_to_pos(Player* player, int idx, CardWhere* where, int* pos);



// tolua_begin
// {{

const char* player_id_str(PlayerID id);
const char* equip_idx_str(int idx);

PlayerID get_player_id(Player* pPlayer);
const char* get_player_name(Player* pPlayer);
HeroID get_player_hero(Player* pPlayer);
int get_player_curlife(Player* pPlayer);
int get_player_maxlife(Player* pPlayer);

int get_player_handcard_num(Player* pPlayer);
int get_player_judgecard_num(Player* pPlayer);
Card* get_player_handcard(Player* pPlayer, int index);
Card* get_player_judgecard(Player* pPlayer, int index);
Card* get_player_equipcard(Player* pPlayer, int index);

PlayerStatus  get_player_status(Player* pPlayer);
PlayerFlag  get_player_flag(Player* pPlayer);

YESNO is_player_dead(Player* pPlayer);
YESNO is_player_show(Player* pPlayer);
YESNO is_player_perdead(Player* pPlayer);
YESNO is_player_handfull(Player* player);

int get_player_param(Player* pPlayer, int index);
void set_player_param(Player* pPlayer, int index, int val);



// }}
// tolua_end


#ifdef __cplusplus
}
#endif


#endif /* __PLAYER_H__ */


