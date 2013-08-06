#ifndef __CARD_H__
#define __CARD_H__



#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

// tolua_begin

//card type
typedef enum _CardType
{
	CardType_Unknown = -1,
	CardType_None = 0,
	CardType_Normal = 1,      // normal card
	CardType_Strategy = 2,    // strategy card with a specified skill
	CardType_DelayStrategy = 3,    // strategy card with a specified skill it calculated in next round
	CardType_Armor = 4,       // used when be attacked
	CardType_Weapon = 5,      // used when attack a target
	CardType_HorseInc = 6,    // be attacked the target distance from attack add 1 unit.
	CardType_HorseDec = 7,    // attack other , target distance from attack  dec 1 unit.
}CardType;

// card identification
typedef enum _CardID
{
	CardID_None = 0,
	// other card ids are registered in script.
	//
	/*
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
	*/
}CardID;

// card color
typedef enum _CardColor
{
	CardColor_Unknown = -1,      // unknown color 
	CardColor_None = 0,          // no color 
	CardColor_Spade = 1,
	CardColor_Club = 2,
	CardColor_Heart = 3,
	CardColor_Diamond = 4,
	CardColor_GeneralBlack = 5,   // general color black. spade or club
	CardColor_GeneralRed = 6,     // general color red Heart or Diamond
}CardColor;

// card value
typedef enum _CardValue
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
}CardValue;



typedef enum _CardFlag
{
	CardFlag_None = 0,
	CardFlag_PrepareOut = 1,   // the card prepare to out, in prepare event, cannot calc the card effect
	CardFlag_PrepareDiscard = 2,   // the card prepare to discard, in prepare event, cannot calc the card effect
	CardFlag_PrepareSupply = 3,   // the card prepare to give other player, in prepare event, cannot calc the card effect
	CardFlag_InUse = 4,
	CardFlag_FromGetStack = 10,
	CardFlag_FromHand = 11,
	CardFlag_FromEquip = 12,
	CardFlag_FromJudge = 13,
} CardFlag;


typedef enum _TargetType
{
	Target_None,     // not need target
	Target_Self,     // self
	Target_OtherOne, // one of other role except me
	Target_AnyOne,   // any one role include me
	Target_OtherOneIn1, // one of other role except me in distance 1
	Target_AnyOneIn1,   // any one role include me in distance 1
	Target_OtherAll, // all role except me
	Target_All,      // all role include me
}TargetType;


typedef enum _CardWhere
{
	CardWhere_None = 0,
	CardWhere_PlayerHand = 1,
	CardWhere_PlayerEquip,
	CardWhere_PlayerJudgment,
	CardWhere_GetStack,
	CardWhere_CurDiscardStack,
	CardWhere_DiscardStack,
} CardWhere;


// need or bits
typedef enum _PatternCardWhere{
	PatternWhere_None = 0,
	PatternWhere_Hand = (1<<CardWhere_PlayerHand),
	PatternWhere_Equip = (1<<CardWhere_PlayerEquip),
	PatternWhere_Judgment = (1<<CardWhere_PlayerJudgment),
} PatternCardWhere;


// tolua_end

typedef struct tagCard Card;
typedef struct tagPosCard PosCard;
typedef struct tagCardPattern CardPattern;
typedef struct tagCardList CardList;
typedef struct tagPosCardList PosCardList;


typedef struct tagGameContext GameContext;
typedef struct tagGameEventContext GameEventContext;

/*
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
*/

// tolua_begin

typedef struct tagCard 
{
	CardID    id;
	CardColor color;
	CardValue value;
	CardFlag  flag;
} Card;


typedef struct tagCardPattern
{
	CardID id;
	CardColor color;
	CardValue value_min;
	CardValue value_max;
} CardPattern;





typedef struct tagPosCard
{
	Card       card;
	CardWhere  where;
	int        pos;
}PosCard;


#define MAX_CARD_LIST_NUM  20


typedef struct tagCardList
{
	int    num;
	Card   cards[MAX_CARD_LIST_NUM];
} CardList;


typedef struct tagPosCardList
{
	int      num;
	PosCard  pcards[MAX_CARD_LIST_NUM];
}PosCardList;

// each virtual card have max 4 read cards
#define MAX_RCARD_NUM  4

typedef struct tagVCard {
	Card  vcard;     // virtual as a which card
	int   rnum;      // virtual card contains real cards number
	Card  rcards[MAX_RCARD_NUM];  // list of real cards
} VCard; 

// virtual card with pos information
typedef struct tagPosVCard
{
	Card     vcard; 
	PosCardList list;
} PosVCard;


// tolua_end

#define RESET_CARD(pCard)   ST_ZERO(*pCard)
#define CARD_VALID(p)   ((p)->id != CardID_None)

#define CARD_EQUAL(p1, p2)    ( CARD_EQUAL_ID((p1), (p2))  && CARD_EQUAL_COLOR((p1), (p2)) && CARD_EQUAL_VALUE((p1), (p2)) )
#define CARD_EQUAL_ID(p1, p2)   ((p1)->id == (p2)->id)
#define CARD_EQUAL_COLOR(p1, p2)   ((p1)->color == (p2)->color)
#define CARD_EQUAL_VALUE(p1, p2)   ((p1)->value == (p2)->value)

#define VCARD_IS_REAL(vc)   ((vc)->rnum == 1 && CARD_EQUAL(&(vc)->vcard, &(vc)->rcards[0]))


#define INIT_CARDPATTERN_USE_ID(cp, _id)  ((cp)->id=(_id), (cp)->color=CardColor_None, (cp)->value_min=CardValue_None, (cp)->value_max=CardValue_None)
#define INIT_CARDPATTERN_USE_COLOR(cp, _c)  ((cp)->id=CardID_None, (cp)->color=(c), (cp)->value_min=CardValue_None, (cp)->value_max=CardValue_None)
#define INIT_CARDPATTERN_USE_VALUE(cp, _v)  ((cp)->id=CardID_None, (cp)->color=CardColor_None, (cp)->value_min=(_v), (cp)->value_max=(_v))
#define INIT_CARDPATTERN_USE_VALUE_RANGE(cp, _v1, _v2)  ((cp)->id=CardID_None, (cp)->color=CardColor_None, (cp)->value_min=(_v1), (cp)->value_max=(_v2))



#define CHECK_WHERE_PATTERN(where, where_pattern)   (((where_pattern)&(1<<(where)))!=0)




// const CardConfig* get_card_config(CardID id);

void card_dump(const Card* pCard);

#define card_str(pCard, buffer, buflen) card_str_n((pCard),1,(buffer),(buflen))
char* card_str_n(const Card* pCard, int num, char* buffer, int buflen);

#define vcard_str(pCard, buffer, buflen)  vcard_str_n((pCard),1,(buffer),(buflen))
char* vcard_str_n(const VCard* pCard, int num, char* buffer, int buflen);

char* card_simple_str(const Card* pCard, char* buffer, int buflen);

#define card_str_def(c, b, l, d) ( ((c)->id == CardID_None) ? d : card_str((c),(b),(l)) )

void set_vcard_from_card(VCard* pVCard, const Card* pCard);

#define card_pattern_str(pattern, buffer, buflen) card_pattern_str_n((pattern),1,(buffer),(buflen))
char* card_pattern_str_n(const CardPattern* patterns, int num, char* buffer, int buflen);


// card  pattern: <{sid}><color><val>
//         each <...> can be [<from>-<to>] or [<p1><p2><p3>] ...
//        <{sid}> : the card sid name. can be empty, that means any sid is valid, equal to {none}
//        <color> : one of char - can be empty, that means any color
//             s  : spade
//             h  : heart
//             d  : diamond
//             c  : club
//             b  : black (spade or club)
//             r  : red (heart or diamond)
//             n  : none (any color)
//        <val> : one of '2 - 10, J, Q, K, A' , can use [from-to] format, if it is empty, means any value.
RESULT load_card_pattern(CardPattern* pCardPattern, const char* szPattern, int len);

RESULT card_match(const Card* pCard, size_t offset, int nCardNum, const CardPattern* pPattern, int nPatternNum);


// tolua_begin

const char* card_type_str(CardType type);
//const char* card_id_str(CardID id);
const char* card_color_str(CardColor color);
const char* card_value_str(CardValue value);
const char* card_flag_str(CardFlag flag);

const char* get_card_str(Card* card);

/*
void set_card_id(Card* pCard, CardID id);
void set_card_color(Card* pCard, CardColor cl);
void set_card_value(Card* pCard, CardValue val);
void set_card_flag(Card* pCard, CardFlag flag);
*/



// tolua_end


// wrapper from lua to c
CardID  card_maxid();
CardID  card_sid2id(const char* sid);
CardType card_type(CardID  id);
char* card_sid(CardID  id, char* buf, int buflen);
char* card_name(CardID  id, char* buf, int buflen);
char* card_desc(CardID  id, char* buf, int buflen);
YESNO  call_card_can_out(CardID  id, GameContext* pGame, GameEventContext* pEvent, int player, PosCard* pos_card);
CANUSE  call_card_can_use(CardID  id, GameContext* pGame, GameEventContext* pEvent, int player, PosCard* pos_card);
RESULT  call_card_event(CardID  id, GameContext* pGame, GameEventContext* pEvent, int player);



#ifdef __cplusplus
}
#endif


#endif /* __CARD_H__ */


