#ifndef __CARD_H__
#define __CARD_H__



#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif


//card type
enum CardType
{
	CardType_Unknown = -1,
	CardType_None = 0,
	CardType_Normal = 1,      // normal card
	CardType_Strategy = 2,    // strategy card with a specified skill
	CardType_Armor = 3,       // used when be attacked
	CardType_Weapon = 4,      // used when attack a target
	CardType_HorseChase = 5,  // attack the target distance add 1 unit.
	CardType_HorseRun = 6,    // be attacked the attacker distance dec 1 unit.
};

// card identification
enum CardID
{
	CardID_None = 0,
	CardID_Attack = 1,   // SHA - a normal attack card, the target lose 1 life unless the target out a SHAN card
	CardID_Defend,   // SHAN - a normal defend card, counteract the effect of the SHA card
	CardID_Medicine, // TAO - a normal medicine card, the target restore one life when used to.
	CardID_Lightning, // SHANDIAN
	CardID_Happy,     // LEBUSISHU
	CardID_Unassailable, // WUXIEJEJI
	CardID_OtherAttack,   // JIEDAOSHAREN
	CardID_Foison,    // WUGUFENGDENG
	CardID_GenWithAir, // WUZHONGSHENGYOU
	CardID_Duel, // JUEDOU
	CardID_MakeBrother, // TAOYUANJIEYI
	CardID_Aggression, // NANMANRUQING
	CardID_AllAttack, // WANJIANQIFA - attack all player except Attacker. all of the target lose 1 life unless the target out a SHAN card
	CardID_Snitch, // SHUNGSHOUQIANYANG
	CardID_KickLadder, // GUOHECAIQIAO
	CardID_ZhuaHuang, 
	CardID_DiLu, 
	CardID_JueYing, 
	CardID_ChiTu, 
	CardID_ZiXing, 
	CardID_DaWan, 
	CardID_LianNu,
	CardID_SwordHanBing,
	CardID_SwordQingHong,
	CardID_SwordChiXiong,
	CardID_AxeGuanShi,
	CardID_SwordQingLong,
	CardID_SpearZhangBa,
	CardID_HalberdFangTian,
	CardID_BowQiLing,
	CardID_BaGua,
	CardID_SheildRenWang,
	CardID_Max,
};

// card color
enum CardColor
{
	CardColor_Unknown = -1,      // unknown color 
	CardColor_None = 0,          // no color 
	CardColor_Spade = 1,
	CardColor_Club = 2,
	CardColor_Heart = 3,
	CardColor_Diamond = 4,
	CardColor_GeneralBlack = 5,   // general color black. spade or club
	CardColor_GeneralRed = 6,     // general color red Heart or Diamond
};

// card value
enum CardValue
{
	CardValue_Unknown = -1,
	CardValue_None = 0,

	CardValue_2 = 1,
	CardValue_3 = 2,
	CardValue_4 = 3,
	CardValue_5 = 4,
	CardValue_6 = 5,
	CardValue_7 = 6,
	CardValue_8 = 7,
	CardValue_9 = 8,
	CardValue_10 = 9,
	CardValue_J = 10,
	CardValue_Q = 11,
	CardValue_K = 12,
	CardValue_A = 13,
};



enum CardFlag
{
	CardFlag_None = 0,
	CardFlag_PrepareOut = 1,   // the card prepare to out, in prepare event, cannot calc the card effect
	CardFlag_PrepareDiscard = 2,   // the card prepare to discard, in prepare event, cannot calc the card effect
	CardFlag_PrepareGive = 3,   // the card prepare to give other player, in prepare event, cannot calc the card effect
	CardFlag_FromHand = 11,
	CardFlag_FromEquip = 12,
	CardFlag_FromJudge = 13,
};


enum TargetType
{
	Target_None,     // not need target
	Target_Self,     // self
	Target_OtherOne, // one of other role except me
	Target_AnyOne,   // any one role include me
	Target_OtherOneIn1, // one of other role except me in distance 1
	Target_AnyOneIn1,   // any one role include me in distance 1
	Target_OtherAll, // all role except me
	Target_All,      // all role include me
};


typedef struct tagCard Card;
typedef struct tagGameContext GameContext;
typedef struct tagGameEventContext GameEventContext;

typedef YESNO  (*CARDCHECKFUN)(GameContext*, GameEventContext*, int);
typedef RESULT (*CARDOUTFUN)(GameContext*, GameEventContext*, int);


typedef struct tagCardConfig
{
	CardID       id;
	CardType     type;
	char         name[MAX_NAME_LEN];
	char         desc[MAX_DESC_LEN];
	TargetType   target;  // target select strategy
	CARDCHECKFUN check;   // called when card want to out(use, activity)   default NULL. can not be used in activity
	CARDOUTFUN   out;     // called when card out (activity)               default NULL. can not be used in activity
}CardConfig;

typedef struct tagCard Card;


struct tagCard
{
	CardID    id;
	CardColor color;
	CardValue value;
	CardFlag  flag;
};



typedef struct tagCardPattern CardPattern;

struct tagCardPattern
{
	CardID id;
	CardColor color;
	CardValue value_min;
	CardValue value_max;
};

#define RESET_CARD(pCard)   ST_ZERO(*pCard)
#define CARD_VALID(p)   ((p)->id != CardID_None)

#define CARD_EQUAL(p1, p2)    ( CARD_EQUAL_ID((p1), (p2))  && CARD_EQUAL_COLOR((p1), (p2)) && CARD_EQUAL_VALUE((p1), (p2)) )
#define CARD_EQUAL_ID(p1, p2)   ((p1)->id == (p2)->id)
#define CARD_EQUAL_COLOR(p1, p2)   ((p1)->color == (p2)->color)
#define CARD_EQUAL_VALUE(p1, p2)   ((p1)->value == (p2)->value)


const char* card_type_str(CardType type);
const char* card_id_str(CardID id);
const char* card_color_str(CardColor color);
const char* card_value_str(CardValue value);

const CardConfig* get_card_config(CardID id);

void card_dump(const Card* pCard);

#define card_str(pCard, buffer, buflen) card_str_n((pCard),1,(buffer),(buflen))
char* card_str_n(const Card* pCard, int num, char* buffer, int buflen);

char* card_simple_str(const Card* pCard, char* buffer, int buflen);

#define card_str_def(c, b, l, d) ( ((c)->id == CardID_None) ? d : card_str((c),(b),(l)) )

RESULT card_match(const Card* pCard, const CardPattern* pPattern, int num);

#define card_pattern_str(pattern, buffer, buflen) card_pattern_str_n((pattern),1,(buffer),(buflen))
char* card_pattern_str_n(const CardPattern* patterns, int num, char* buffer, int buflen);


#ifdef __cplusplus
}
#endif


#endif /* __CARD_H__ */


