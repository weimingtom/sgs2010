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



#define MAX_HAND_CARD   16
#define MAX_JUDGMENT_CARD  8


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
	Card  stWeaponCard;
	Card  stAomorCard;
	Card  stHorseChaseCard;
	Card  stHorseRunCard;
	Card  stJudgmentCards[MAX_JUDGMENT_CARD];
} Player;


int init_player(Player* pPlayer, int id, int hero);


#endif /* __PLAYER_H__ */


