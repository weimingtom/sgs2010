#ifndef __CARD_H__
#define __CARD_H__

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

	CardValue_2 = 2,
	CardValue_3 = 3,
	CardValue_4 = 4,
	CardValue_5 = 5,
	CardValue_6 = 6,
	CardValue_7 = 7,
	CardValue_8 = 8,
	CardValue_9 = 9,
	CardValue_10 = 10,
	CardValue_J = 11,
	CardValue_Q = 12,
	CardValue_K = 13,
	CardValue_A = 14,
};

typedef struct tagGameContext GameContext;
typedef struct tagGameEventContext GameEventContext;
typedef YESNO  (*CARDCHECKFUN)(GameContext*, GameEventContext*, int);
typedef int  (*CARDOUTFUN)(GameContext*, GameEventContext*, int);
typedef int  (*CARDCALCFUN)(GameContext*, GameEventContext*, int);
typedef int  (*CARDFINIFUN)(GameContext*, GameEventContext*, int);


typedef struct tagCardConfig
{
	char id;
	char type;
	char name[MAX_NAME_LEN];
	char desc[MAX_DESC_LEN];
	CARDCHECKFUN check;   // called when card want to out(use, activity)   default NULL. can not be used in activity
	CARDOUTFUN   out;     // called when card out (activity)               default NULL. can not be used in activity
	CARDCALCFUN  calc;    // called when card effect is needed to calc     default NULL. no effect
	CARDCALCFUN  fini;    // called when card calc finished (if card still exist after calc)  default NULL, discard to out card stack
}CardConfig;


typedef struct tagCard
{
	union {
		struct {
			char id;
			char color;
			char value;
			char flag;
		};
		unsigned long ul;
	};
}Card;


typedef struct tagPosCard
{
	Card card;
	int  where;
	int  pos;
}PosCard;

typedef struct tagOutCard OutCard;

#define MAX_RCARD_NUM  5

struct tagOutCard
{
	PosCard vcard;    //  use as card (virtaul) 
	int  nrcard;   //  real card number ,. if 0 vcard is also real card 
	PosCard rcards[MAX_RCARD_NUM];  // rcard array;
};


typedef struct tagCardPattern CardPattern;

struct tagCardPattern
{
	char id;
	char color;
	char value_min;
	char value_max;
};


typedef struct tagOutCardPattern OutCardPattern;

struct tagOutCardPattern
{
	int num;
	CardPattern patterns[MAX_RCARD_NUM];
};



#define INIT_CARDPATTERN_USE_ID(cp, _id)  ((cp)->id=(_id), (cp)->color=CardColor_None, (cp)->value_min=CardValue_None, (cp)->value_max=CardValue_None)
#define INIT_CARDPATTERN_USE_COLOR(cp, _c)  ((cp)->id=CardID_None, (cp)->color=(c), (cp)->value_min=CardValue_None, (cp)->value_max=CardValue_None)
#define INIT_CARDPATTERN_USE_VALUE(cp, _v)  ((cp)->id=CardID_None, (cp)->color=CardColor_None, (cp)->value_min=(_v), (cp)->value_max=(_v))
#define INIT_CARDPATTERN_USE_VALUE_RANGE(cp, _v1, _v2)  ((cp)->id=CardID_None, (cp)->color=CardColor_None, (cp)->value_min=(_v1), (cp)->value_max=(_v2))


const char* card_type_str(int type);
const char* card_id_str(int id);
const char* card_color_str(int color);
const char* card_value_str(int value);

const CardConfig* get_card_config(int id);

void card_dump(const Card* pCard);

char* card_str(const Card* pCard, char* buffer, int buflen);

char* card_simple_str(const Card* pCard, char* buffer, int buflen);

#define card_str_def(c, b, l, d) ( ((c)->id == CardID_None) ? d : card_str((c),(b),(l)) )


int card_match(const Card* pCard, const CardPattern* pPattern, int num);


#endif /* __CARD_H__ */


