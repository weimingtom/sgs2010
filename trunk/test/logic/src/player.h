#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "card.h"
#include "hero.h"


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

// need or bits
enum PlayerCardWhere{
	PlayerCard_Invald = 0,
	PlayerCard_Hand = 1,
	PlayerCard_Equip = 2,
	PlayerCard_Judgment = 4,
};


// tolua_end

typedef  struct  tagPlayer
{
	PlayerID id;
	HeroID   hero;
	int      maxLife;
	int      curLife;
	char     name[MAX_NAME_LEN];
	int      nHandCardNum;
	int      nJudgmentCardNum;
	Card     stHandCards[MAX_HAND_CARD];
	Card     stEquipCard[EquipIdx_Max];
	Card     stJudgmentCards[MAX_JUDGMENT_CARD];
	PlayerStatus   status;
	PlayerFlag  flag;
	int      params[MAX_PLAYER_PARAM];
} Player;




//#define PLAYER_CARD_WHERE(pos)     ((pos>>8) & 0xff)
//#define PLAYER_CARD_INDEX(pos)     ((pos) & 0xff)

#define IS_PLAYER_DEAD(p)   ((p)->status == PlayerStatus_Dead)
#define IS_PLAYER_SHOW(p)  ((p)->status == PlayerStatus_Show)

#define IS_PLAYER_PERDEAD(p)   ((p)->status != PlayerStatus_Dead && (p)->curLife <= 0)


#define PLAYER_CHK_FLAG(p,f)  (((p)->flag & f) == f)
#define PLAYER_SET_FLAG(p,f)  ((p)->flag |= f)
#define PLAYER_CLR_FLAG(p,f)  ((p)->flag &= ~f)
#define PLAYER_CLR_ALL_FLAG(p)  ((p)->flag = PlayerFlag_None)


#define PLAYER_WEAPON(p)   (&(p)->stEquipCard[EquipIdx_Weapon])
#define PLAYER_ARMOR(p)   (&(p)->stEquipCard[EquipIdx_Armor])
#define PLAYER_HORSEINC(p)   (&(p)->stEquipCard[EquipIdx_HorseInc])
#define PLAYER_HORSEDEC(p)   (&(p)->stEquipCard[EquipIdx_HorseDec])

#define PLAYER_HANDCARD(p, idx)   (&(p)->stHandCards[idx])

#define PLAYER_JUDGECARD(p, idx)   (&(p)->stJudgmentCards[idx])

RESULT init_player(Player* pPlayer, PlayerID id, HeroID hero);


const char* player_id_str(PlayerID id);
const char* equip_idx_str(int idx);

RESULT player_add_hand_card(Player* pPlayer, Card* pCard);


RESULT set_player_card_flag(Player* pPlayer, int where, int pos, CardFlag flag);

RESULT player_add_judge_card(Player* pPlayer, Card* pCard);

RESULT get_player_card(Player* pPlayer, int where, int pos, Card* pCard);
RESULT player_remove_card(Player* pPlayer, int where, int pos, Card* pCard);

RESULT player_add_judge_card(Player* pPlayer, Card* pCard);
RESULT player_card_idx_to_pos(Player* player, int idx, int* where, int* pos);

YESNO is_player_handfull(Player* player);


#ifdef __cplusplus
}
#endif


#endif /* __PLAYER_H__ */


