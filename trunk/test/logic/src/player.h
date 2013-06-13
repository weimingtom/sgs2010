#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "card.h"

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


enum PlayerFlag
{
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


#define PLAYER_CHK_FLAG(p,f)  (((p)->flag & f) == f)
#define PLAYER_SET_FLAG(p,f)  ((p)->flag |= f)
#define PLAYER_CLR_FLAG(p,f)  ((p)->flag &= ~f)
#define PLAYER_CLR_ALL_FLAG(p)  ((p)->flag = 0)


#define MAX_HAND_CARD   20
#define MAX_JUDGMENT_CARD  8
#define MAX_PLAYER_PARAM  10



enum EquipIdx {
	EquipIdx_Weapon = 0,
	EquipIdx_Aomor,
	EquipIdx_HorseInc,
	EquipIdx_HorseDec,
	EquipIdx_Max,
};


enum PlayerCardWhere{
	PlayerCard_Hand = 1,
	PlayerCard_Equip = 2,
	PlayerCard_Judgment = 3,
};

typedef  struct  tagPlayer
{
	int id;
	int hero;
	int maxLife;
	int curLife;
	char name[MAX_NAME_LEN];
	int   nHandCardNum;
	int   nJudgmentCardNum;
	Card  stHandCards[MAX_HAND_CARD];
	Card  stEquipCard[EquipIdx_Max];
	Card  stJudgmentCards[MAX_JUDGMENT_CARD];
	unsigned long flag;
	int   params[MAX_PLAYER_PARAM];
} Player;




#define PLAYER_CARD_WHERE(pos)     ((pos>>8) & 0xff)
#define PLAYER_CARD_INDEX(pos)     ((pos) & 0xff)


int init_player(Player* pPlayer, int id, int hero);


const char* player_id_str(int id);

int player_remove_card(Player* pPlayer, int pos);



#endif /* __PLAYER_H__ */


