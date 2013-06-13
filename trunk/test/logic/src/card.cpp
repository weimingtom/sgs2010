#include "config.h"
#include "card.h"



const char* card_type_str(int type)
{
	switch(type)
	{
	case CardType_Unknown: return "Î´Öª";
	case CardType_None: return "ÎÞ";
	case CardType_Normal: return "ÆÕÍ¨";
	case CardType_Strategy: return "½õÄÒ";
	case CardType_Armor: return "·À¾ß";
	case CardType_Weapon: return "ÎäÆ÷";
	case CardType_HorseChase: return "Âí(+1)";
	case CardType_HorseRun: return "Âí(-1)";
	default: return "Invalid";
	}
}


const char* card_id_str(int id)
{
	switch(id)
	{
	case CardID_None: return "ÎÞ";
	case CardID_Attack: return "É±";
	case CardID_Defend: return "ÉÁ";
	case CardID_Medicine: return "ÌÒ";
	case CardID_Lightning: return "ÉÁµç";
	case CardID_Happy: return "ÀÖ²»Ë¼Êñ";
	case CardID_Unassailable: return "ÎÞÐ¸¿É»÷";
	case CardID_OtherAttack: return "½èµ¶É±ÈË";
	case CardID_Foison: return "Îå¹È·áµÇ";
	case CardID_GenWithAir: return "ÎÞÖÐÉúÓÐ";
	case CardID_Duel: return "¾ö¶·";
	case CardID_MakeBrother: return "ÌÒÔ°½áÒå";
	case CardID_Aggression: return "ÄÏÂùÈëÇÖ";
	case CardID_AllAttack: return "Íò¼ýÆë·¢";
	case CardID_Snitch: return "Ë³ÊÖÇ£Ñò";
	case CardID_KickLadder: return "¹ýºÓ²ðÇÅ";
	case CardID_ZhuaHuang: return "×¦»Æ·Éµç";
	case CardID_DiLu: return "µÄÂ¬";
	case CardID_JueYing: return "¾øÓ°";
	case CardID_ChiTu: return "³àÍÃ";
	case CardID_ZiXing: return "×ÏóU";
	case CardID_DaWan: return "´óÍð";
	case CardID_LianNu: return "Öî¸ðÁ¬åó";
	case CardID_SwordHanBing: return "º®±ù½£";
	case CardID_SwordQingHong: return "ÇàâG½£";
	case CardID_SwordChiXiong: return "´ÆÐÛË«¹É½£";
	case CardID_AxeGuanShi: return "¹áÊ¯¸«";
	case CardID_SwordQingLong: return "ÇàÁúÙÈÔÂµ¶";
	case CardID_SpearZhangBa: return "ÕÉ°ËÉßÃ¬";
	case CardID_HalberdFangTian: return "·½Ìì»­êª";
	case CardID_BowQiLing: return "÷è÷ë¹­";
	case CardID_BaGua: return "°ËØÔÕó";
	case CardID_SheildRenWang: return "ÈÊÍõ¶Ü";

	default: return "Invalid";
	}
}

const char* card_color_str(int color)
{
	switch(color)
	{
	case CardColor_Unknown: return "Î´Öª";
	case CardColor_None: return "ÎÞ"; 
	case CardColor_Spade: return "ºÚÌÒ";
	case CardColor_Club: return "Ã·»¨";
	case CardColor_Heart: return "ºìÌÒ";
	case CardColor_Diamond : return "·½¿é";
	case CardColor_GeneralBlack: return "ÆÕÍ¨ºÚÉ«";
	case CardColor_GeneralRed: return "ÆÕÍ¨ºìÉ«";
	default: return "Invalid";
	};
}

const char* card_value_str(int value)
{
	switch(value)
	{
	case CardValue_Unknown: return "Î´Öª";
	case CardValue_None: return "ÎÞ";
	case CardValue_2: return "£²";
	case CardValue_3: return "£³";
	case CardValue_4: return "£´";
	case CardValue_5: return "£µ";
	case CardValue_6: return "£¶";
	case CardValue_7: return "£·";
	case CardValue_8: return "£¸";
	case CardValue_9: return "£¹";
	case CardValue_10: return "10";
	case CardValue_J: return "£Ê";
	case CardValue_Q: return "£Ñ";
	case CardValue_K: return "£Ë";
	case CardValue_A: return "£Á";
	default: return "Invalid";
	}
}



extern const CardConfig* get_card_attack();
extern const CardConfig* get_card_defend();
extern const CardConfig* get_card_medicine();
extern const CardConfig* get_card_lightning();
extern const CardConfig* get_card_happy();
extern const CardConfig* get_card_unassailable();
extern const CardConfig* get_card_otherattack();
extern const CardConfig* get_card_foison();
extern const CardConfig* get_card_genwithair();
extern const CardConfig* get_card_duel();
extern const CardConfig* get_card_makebrother();
extern const CardConfig* get_card_aggression();
extern const CardConfig* get_card_allattack();
extern const CardConfig* get_card_snitch();
extern const CardConfig* get_card_kickladder();
extern const CardConfig* get_card_zhuahuang();
extern const CardConfig* get_card_dilu();
extern const CardConfig* get_card_jueying();
extern const CardConfig* get_card_chitu();
extern const CardConfig* get_card_zixing();
extern const CardConfig* get_card_dawan();
extern const CardConfig* get_card_liannu();
extern const CardConfig* get_card_swordhanbing();
extern const CardConfig* get_card_swordqinghong();
extern const CardConfig* get_card_swordchixiong();
extern const CardConfig* get_card_axeguanshi();
extern const CardConfig* get_card_swordqinglong();
extern const CardConfig* get_card_spearzhangba();
extern const CardConfig* get_card_halberdfangtian();
extern const CardConfig* get_card_bowqiling();
extern const CardConfig* get_card_bagua();
extern const CardConfig* get_card_sheildrenwang();




const CardConfig* get_card_config(int id)
{
	switch(id)
	{
	case CardID_Attack: return get_card_attack();
	case CardID_Defend: return get_card_defend();
	case CardID_Medicine: return get_card_medicine();
	case CardID_Lightning: return get_card_lightning();
	case CardID_Happy: return get_card_happy();
	case CardID_Unassailable: return get_card_unassailable();
	case CardID_OtherAttack: return get_card_otherattack();
	case CardID_Foison: return get_card_foison();
	case CardID_GenWithAir: return get_card_genwithair();
	case CardID_Duel: return get_card_duel();
	case CardID_MakeBrother: return get_card_makebrother();
	case CardID_Aggression: return get_card_aggression();
	case CardID_AllAttack: return get_card_allattack();
	case CardID_Snitch: return get_card_snitch();
	case CardID_KickLadder: return get_card_kickladder();
	case CardID_ZhuaHuang: return get_card_zhuahuang();
	case CardID_DiLu: return get_card_dilu();
	case CardID_JueYing: return get_card_jueying();
	case CardID_ChiTu: return get_card_chitu();
	case CardID_ZiXing: return get_card_zixing();
	case CardID_DaWan: return get_card_dawan();
	case CardID_LianNu: return get_card_liannu();
	case CardID_SwordHanBing: return get_card_swordhanbing();
	case CardID_SwordQingHong: return get_card_swordqinghong();
	case CardID_SwordChiXiong: return get_card_swordchixiong();
	case CardID_AxeGuanShi: return get_card_axeguanshi();
	case CardID_SwordQingLong: return get_card_swordqinglong();
	case CardID_SpearZhangBa: return get_card_spearzhangba();
	case CardID_HalberdFangTian: return get_card_halberdfangtian();
	case CardID_BowQiLing: return get_card_bowqiling();
	case CardID_BaGua: return get_card_bagua();
	case CardID_SheildRenWang: return get_card_sheildrenwang();
	}
	return NULL;
}

char* card_str(const Card* pCard, char* buffer, int buflen)
{
	snprintf(buffer, buflen, "(%s, %s %s)", card_id_str(pCard->id), card_color_str(pCard->color), card_value_str(pCard->value));
	return buffer;
}


void card_dump(const Card* pCard)
{
	char buffer[128];
	printf("%s", card_str(pCard, buffer, sizeof(buffer)));	
}


