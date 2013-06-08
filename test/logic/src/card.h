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
	CardID_Attack = 1,   // SHA - a normal attack card, the target lose 1 life unless the target out a SHAN card
	CardID_Defend = 2,   // SHAN - a normal defend card, counteract the effect of the SHA card
	CardID_Medicine = 3, // TAO - a normal medicine card, the target restore one life when used to.
	CardID_AllAttack = 4, // WANJIANQIFA - attack all player except Attacker. all of the target lose 1 life unless the target out a SHAN card
	CardID_AllMedicine = 5, // TAOYUANJIEYI - medicine all player include out player.  all of the targets restore one life.
	CardID_Deul = 6, // JUEDOU
	CardID_Aggression = 7, // NANMANRUQING
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




struct Card
{
	int type;
	int id;
	int color;
	int value;
};


#define CARD_STACK_SIZE    108

struct CardStack
{
	int    count;
	Card   cards[CARD_STACK_SIZE];
};


int init_card_stack(CardStack* pCardStack);

int card_stack_random(CardStack* pCardStack);


void card_stack_dump(const CardStack* pCardStack);





#endif /* __CARD_H__ */


