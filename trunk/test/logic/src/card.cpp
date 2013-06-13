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
	case CardID_None: return "��";
	case CardID_Attack: return "ɱ";
	case CardID_Defend: return "��";
	case CardID_Medicine: return "��";
	case CardID_Lightning: return "����";
	case CardID_Happy: return "�ֲ�˼��";
	case CardID_Unassailable: return "��и�ɻ�";
	case CardID_OtherAttack: return "�赶ɱ��";
	case CardID_Foison: return "��ȷ��";
	case CardID_GenWithAir: return "��������";
	case CardID_Duel: return "����";
	case CardID_MakeBrother: return "��԰����";
	case CardID_Aggression: return "��������";
	case CardID_AllAttack: return "����뷢";
	case CardID_Snitch: return "˳��ǣ��";
	case CardID_KickLadder: return "���Ӳ���";
	case CardID_ZhuaHuang: return "צ�Ʒɵ�";
	case CardID_DiLu: return "��¬";
	case CardID_JueYing: return "��Ӱ";
	case CardID_ChiTu: return "����";
	case CardID_ZiXing: return "���U";
	case CardID_DaWan: return "����";
	case CardID_LianNu: return "�������";
	case CardID_SwordHanBing: return "������";
	case CardID_SwordQingHong: return "���G��";
	case CardID_SwordChiXiong: return "����˫�ɽ�";
	case CardID_AxeGuanShi: return "��ʯ��";
	case CardID_SwordQingLong: return "�������µ�";
	case CardID_SpearZhangBa: return "�ɰ���ì";
	case CardID_HalberdFangTian: return "���컭�";
	case CardID_BowQiLing: return "���빭";
	case CardID_BaGua: return "������";
	case CardID_SheildRenWang: return "������";

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
	case CardValue_10: return "10";
	case CardValue_J: return "��";
	case CardValue_Q: return "��";
	case CardValue_K: return "��";
	case CardValue_A: return "��";
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


