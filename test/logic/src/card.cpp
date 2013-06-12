#include "config.h"
#include "card.h"



const char* card_type_str(int type)
{
	switch(type)
	{
	case CardType_Unknown: return "δ֪";
	case CardType_None: return "��";
	case CardType_Normal: return "��ͨ";
	case CardType_Strategy: return "����";
	case CardType_Armor: return "����";
	case CardType_Weapon: return "����";
	case CardType_HorseChase: return "��(+1)";
	case CardType_HorseRun: return "��(-1)";
	default: return "Invalid";
	}
}


const char* card_id_str(int id)
{
	switch(id)
	{
	case CardID_Attack: return "ɱ";
	case CardID_Defend: return "��";
	case CardID_Medicine: return "��";
	case CardID_AllAttack: return "����뷢";
	//case CardID_AllMedicine: return "��԰����";
	case CardID_Deul: return "����";
	case CardID_Aggression: return "��������";
	default: return "Invalid";
	}
}

const char* card_color_str(int color)
{
	switch(color)
	{
	case CardColor_Unknown: return "δ֪";
	case CardColor_None: return "��"; 
	case CardColor_Spade: return "����";
	case CardColor_Club: return "÷��";
	case CardColor_Heart: return "����";
	case CardColor_Diamond : return "����";
	case CardColor_GeneralBlack: return "��ͨ��ɫ";
	case CardColor_GeneralRed: return "��ͨ��ɫ";
	default: return "Invalid";
	};
}

const char* card_value_str(int value)
{
	switch(value)
	{
	case CardValue_Unknown: return "δ֪";
	case CardValue_None: return "��";
	case CardValue_2: return "��";
	case CardValue_3: return "��";
	case CardValue_4: return "��";
	case CardValue_5: return "��";
	case CardValue_6: return "��";
	case CardValue_7: return "��";
	case CardValue_8: return "��";
	case CardValue_9: return "��";
	case CardValue_10: return "����";
	case CardValue_J: return "��";
	case CardValue_Q: return "��";
	case CardValue_K: return "��";
	case CardValue_A: return "��";
	default: return "Invalid";
	}
}

const CardConfig* get_card_config(int id)
{
	switch(id)
	{
	case CardID_Attack:  return NULL;
	}
	return NULL;
}


void card_dump(const Card* pCard)
{
	printf("(id:%s, color:%s, value:%s)", card_id_str(pCard->id), card_color_str(pCard->color), card_value_str(pCard->value));	
}
