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
	PlayerID_Minister = 2,
	PlayerID_Spy = 3,
	PlayerID_Mutineer = 4,


	_PlayerID_Valid_Min = PlayerID_Master,
	_PlayerID_Valid_Max = PlayerID_Mutineer,

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
	PlayerFlag_SkipThisRoundOut = 0x10,
	PlayerFlag_SkipThisRoundDiscard = 0x20,
	//PlayerFlag_DiasbleWapon = 0x100,
	//PlayerFlag_DiasbleArmor = 0x200,
	//PlayerFlag_DiasbleHroseInc = 0x400,
	//PlayerFlag_DiasbleHorseDec = 0x800,
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
	VCard    judgment_cards[MAX_JUDGMENT_CARD];
	PlayerStatus   status;
	int      flag;
	int      params[MAX_PLAYER_PARAM];
	int      card_in_judge_num;
} Player;




//#define PLAYER_CARD_WHERE(pos)     ((pos>>8) & 0xff)
//#define PLAYER_CARD_INDEX(pos)     ((pos) & 0xff)

#define IS_PLAYER_DEAD(p)       ((p)->status == PlayerStatus_Dead)
#define IS_PLAYER_SHOW(p)       ((p)->status == PlayerStatus_Show)
#define IS_PLAYER_PERDEAD(p)    ((p)->status != PlayerStatus_Dead && (p)->cur_life <= 0)


#define PLAYER_CHK_FLAG(p,f)    (((p)->flag & (f)) == (f))
#define PLAYER_SET_FLAG(p,f)    ((p)->flag |= (f))
#define PLAYER_CLR_FLAG(p,f)    ((p)->flag &= ~(f))
#define PLAYER_CLR_ALL_FLAG(p)  ((p)->flag = PlayerFlag_None)


#define PLAYER_EQUIPCARD(p, idx)   (&(p)->equip_cards[idx])
#define PLAYER_WEAPON(p)		   PLAYER_EQUIPCARD((p), EquipIdx_Weapon)
#define PLAYER_ARMOR(p)            PLAYER_EQUIPCARD((p), EquipIdx_Armor)
#define PLAYER_HORSEINC(p)         PLAYER_EQUIPCARD((p), EquipIdx_HorseInc)
#define PLAYER_HORSEDEC(p)         PLAYER_EQUIPCARD((p), EquipIdx_HorseDec)


#define PLAYER_HANDCARD(p, idx)    (&(p)->hand_cards[idx])
#define PLAYER_JUDGECARD(p, idx)   (&(p)->judgment_cards[idx])



RESULT init_player(Player* pPlayer, PlayerID id, HeroID hero, int index);


RESULT player_add_hand_card(Player* pPlayer, Card* pCard);
RESULT player_add_judgment_card(Player* pPlayer, VCard* pCard);


RESULT set_player_card_flag(Player* pPlayer, CardWhere where, int pos, CardFlag flag);
CardFlag get_player_card_flag(Player* pPlayer, CardWhere where, int pos);


RESULT get_player_card(Player* pPlayer, CardWhere where, int pos, VCard* pCard);
RESULT player_remove_card(Player* pPlayer, CardWhere where, int pos, VCard* pCard);

RESULT player_card_idx_to_pos(Player* pPlayer, int idx, CardWhere* where, int* pos);

// RESULT player_get_cards_pos(Player* pPlayer, const int* idx, int num, PosVCard*  pPosCards);
// RESULT player_set_cards_flag(Player* pPlayer, const PosCard* pPosCards, int num, CardFlag flag);

// tolua_begin
// {{

const char* player_id_str(PlayerID id);
const char* equip_idx_str(int idx);

Card* get_player_handcard(tolua_notnull Player* pPlayer, int index);
VCard* get_player_judgecard(tolua_notnull Player* pPlayer, int index);
Card* get_player_equipcard(tolua_notnull Player* pPlayer, int index);

// 查找指定ID的牌(返回index,找不到返回-1)
int find_player_handcard(tolua_notnull Player* pPlayer, CardID id);
int find_player_judgecard(tolua_notnull Player* pPlayer, CardID id);


YESNO is_player_dead(tolua_notnull Player* pPlayer);
YESNO is_player_show(tolua_notnull Player* pPlayer);
YESNO is_player_perdead(tolua_notnull Player* pPlayer);
YESNO is_player_handfull(tolua_notnull Player* player);

int   player_count_card(tolua_notnull Player* pPlayer, int where);

int get_player_param(tolua_notnull Player* pPlayer, int index);
void set_player_param(tolua_notnull Player* pPlayer, int index, int val);

void player_set_flag(tolua_notnull Player* pPlayer, int   flag);
void player_clear_flag(tolua_notnull Player* pPlayer, int   flag);
void player_clear_all_flag(tolua_notnull Player* pPlayer);
YESNO player_check_flag(tolua_notnull Player* pPlayer, int   flag);


// }}
// tolua_end


#ifdef __cplusplus
}
#endif


#endif /* __PLAYER_H__ */


