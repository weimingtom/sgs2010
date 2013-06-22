#include "config.h"
#include "card.h"
#include "card_impl/card_impl.h"


const char* card_type_str(CardType type)
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


const char* card_id_str(CardID id)
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

const char* card_color_str(CardColor color)
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

const char* card_value_str(CardValue value)
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






const CardConfig* get_card_config(CardID id)
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


char* card_simple_str(const Card* pCard, char* buffer, int buflen)
{
	buffer[0] = 0;
	return buffer;
}


void card_dump(const Card* pCard)
{
	char buffer[128];
	printf("%s", card_str(pCard, buffer, sizeof(buffer)));	
}



static RESULT card_match_one(const Card* pCard, const CardPattern* pPattern)
{
	if(pPattern->id != CardID_None)
	{
		if(pPattern->id < 0)
		{
			// a id type
			const CardConfig* pCardConfig = get_card_config(pCard->id);
			if(pCardConfig == NULL || pCardConfig->type != -pPattern->id)
				return R_E_FAIL;
		}
		else
		{
			// a real id
			if(pCard->id != pPattern->id)
				return R_E_FAIL;
		}
	}

	if(pPattern->color != CardColor_None)
	{
		switch(pPattern->color)
		{
		case CardColor_Spade:
		case CardColor_Club:
		case CardColor_Heart:
		case CardColor_Diamond:
			if(pCard->color != pPattern->color)
				return R_E_FAIL;
			break;
		case CardColor_GeneralBlack:
			if(pCard->color != CardColor_Spade && pCard->color != CardColor_Club)
				return R_E_FAIL;
			break;
		case CardColor_GeneralRed:
			if(pCard->color != CardColor_Heart && pCard->color != CardColor_Diamond)
				return R_E_FAIL;
			break;
		default:
			return R_E_FAIL;
		}

	}

	// check card value
	if(pPattern->value_min != CardValue_None)
	{
		if(pCard->value < pPattern->value_min)
			return R_E_FAIL;
	}
	if(pPattern->value_max != CardValue_None)
	{
		if(pCard->value > pPattern->value_min)
			return R_E_FAIL;
	}

	return R_SUCC;
}

RESULT card_match(const Card* pCard, const CardPattern* pPattern, int num)
{
	int n, m, p;
	int index [MAX_RCARD_NUM];

	// too many card
	if(num > MAX_RCARD_NUM)
		return R_E_PARAM;

	// fill_array_inc_i(index, num, 0, 1);


	for(n = 0; n < num; /*n++*/)
	{
		// calc first index[n] = ?
		for(m = 0; m < num; m++)
		{
			// first not used index
			for(p = 0; p < n; p++)
			{
				if(index[p] == m)
					break;
			}
			if(p == n)
			{
				index[n] = m;
				break;
			}
		}

		// check a card

		while(0 != card_match_one(pCard+n, pPattern+index[n]))
		{
			// NEXT GROUP INDEX

			for(; n >= 0; n--)
			{
				// calc next index[n]
				for(m = index[n] + 1; m < num; m++)
				{
					// first not used index
					for(p = 0; p < n; p++)
					{
						if(index[p] == m)
							break;
					}
					if(p == n)
					{
						index[n] = m;
						break;
					}
				}
				if(m < num)
				{
					break;
				}
			}

			// next group failed ?
			if(n < 0)
			{
				break;
			}

		}
		if(n < 0)
		{
			break;
		}

		n++;
	}

	return n == num ? R_SUCC : R_E_FAIL;
}


