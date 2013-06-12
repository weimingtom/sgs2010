#include "config.h"
#include "card.h"



const char* card_type_str(int type)
{
	switch(type)
	{
	case CardType_Unknown: return "未知";
	case CardType_None: return "无";
	case CardType_Normal: return "普通";
	case CardType_Strategy: return "锦囊";
	case CardType_Armor: return "防具";
	case CardType_Weapon: return "武器";
	case CardType_HorseChase: return "马(+1)";
	case CardType_HorseRun: return "马(-1)";
	default: return "Invalid";
	}
}


const char* card_id_str(int id)
{
	switch(id)
	{
	case CardID_Attack: return "杀";
	case CardID_Defend: return "闪";
	case CardID_Medicine: return "桃";
	case CardID_AllAttack: return "万箭齐发";
	//case CardID_AllMedicine: return "桃园结义";
	case CardID_Deul: return "决斗";
	case CardID_Aggression: return "南蛮入侵";
	default: return "Invalid";
	}
}

const char* card_color_str(int color)
{
	switch(color)
	{
	case CardColor_Unknown: return "未知";
	case CardColor_None: return "无"; 
	case CardColor_Spade: return "黑桃";
	case CardColor_Club: return "梅花";
	case CardColor_Heart: return "红桃";
	case CardColor_Diamond : return "方块";
	case CardColor_GeneralBlack: return "普通黑色";
	case CardColor_GeneralRed: return "普通红色";
	default: return "Invalid";
	};
}

const char* card_value_str(int value)
{
	switch(value)
	{
	case CardValue_Unknown: return "未知";
	case CardValue_None: return "无";
	case CardValue_2: return "２";
	case CardValue_3: return "３";
	case CardValue_4: return "４";
	case CardValue_5: return "５";
	case CardValue_6: return "６";
	case CardValue_7: return "７";
	case CardValue_8: return "８";
	case CardValue_9: return "９";
	case CardValue_10: return "１０";
	case CardValue_J: return "Ｊ";
	case CardValue_Q: return "Ｑ";
	case CardValue_K: return "Ｋ";
	case CardValue_A: return "Ａ";
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
