#include "config.h"
#include "card.h"
#include "comm.h"
#include "card_impl/card_impl.h"


const char* card_type_str(CardType type)
{
	switch(type)
	{
	case CardType_Unknown: return "未知";
	case CardType_None: return "任意种类";
	case CardType_Normal: return "普通";
	case CardType_Strategy: return "锦囊";
	case CardType_Armor: return "防具";
	case CardType_Weapon: return "武器";
	case CardType_HorseChase: return "马(+1)";
	case CardType_HorseRun: return "马(-1)";
	default: return "Invalid";
	}
}


const char* card_id_str(CardID id)
{
	switch(id)
	{
	case CardID_None: return "任意名称";
	case CardID_Attack: return "杀";
	case CardID_Defend: return "闪";
	case CardID_Medicine: return "桃";
	case CardID_Lightning: return "闪电";
	case CardID_Happy: return "乐不思蜀";
	case CardID_Unassailable: return "无懈可击";
	case CardID_OtherAttack: return "借刀杀人";
	case CardID_Foison: return "五谷丰登";
	case CardID_GenWithAir: return "无中生有";
	case CardID_Duel: return "决斗";
	case CardID_MakeBrother: return "桃园结义";
	case CardID_Aggression: return "南蛮入侵";
	case CardID_AllAttack: return "万箭齐发";
	case CardID_Snitch: return "顺手牵羊";
	case CardID_KickLadder: return "过河拆桥";
	case CardID_ZhuaHuang: return "爪黄飞电";
	case CardID_DiLu: return "的卢";
	case CardID_JueYing: return "绝影";
	case CardID_ChiTu: return "赤兔";
	case CardID_ZiXing: return "紫骍";
	case CardID_DaWan: return "大宛";
	case CardID_LianNu: return "诸葛连弩";
	case CardID_SwordHanBing: return "寒冰剑";
	case CardID_SwordQingHong: return "青釭剑";
	case CardID_SwordChiXiong: return "雌雄双股剑";
	case CardID_AxeGuanShi: return "贯石斧";
	case CardID_SwordQingLong: return "青龙偃月刀";
	case CardID_SpearZhangBa: return "丈八蛇矛";
	case CardID_HalberdFangTian: return "方天画戟";
	case CardID_BowQiLing: return "麒麟弓";
	case CardID_BaGua: return "八卦阵";
	case CardID_SheildRenWang: return "仁王盾";

	default: return "Invalid";
	}
}

const char* card_color_str(CardColor color)
{
	switch(color)
	{
	case CardColor_Unknown: return "未知";
	case CardColor_None: return "任意花色"; 
	case CardColor_Spade: return "黑桃";
	case CardColor_Club: return "梅花";
	case CardColor_Heart: return "红桃";
	case CardColor_Diamond : return "方块";
	case CardColor_GeneralBlack: return "黑色";
	case CardColor_GeneralRed: return "红色";
	default: return "Invalid";
	};
}

const char* card_value_str(CardValue value)
{
	switch(value)
	{
	case CardValue_Unknown: return "未知";
	case CardValue_None: return "任意点数";
	case CardValue_2: return "２";
	case CardValue_3: return "３";
	case CardValue_4: return "４";
	case CardValue_5: return "５";
	case CardValue_6: return "６";
	case CardValue_7: return "７";
	case CardValue_8: return "８";
	case CardValue_9: return "９";
	case CardValue_10: return "10";
	case CardValue_J: return "Ｊ";
	case CardValue_Q: return "Ｑ";
	case CardValue_K: return "Ｋ";
	case CardValue_A: return "Ａ";
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
	default: return NULL;
	}
	return NULL;
}

char* card_str_n(const Card* pCard, int num, char* buffer, int buflen)
{
	int n;
	int len = 0;
	for(n = 0; n < num; n++)
	{
		len += snprintf(buffer + len, buflen - len, "(%s, %s %s)", card_id_str(pCard->id), card_color_str(pCard->color), card_value_str(pCard->value));
	}
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
	MSG_OUT("%s", card_str(pCard, buffer, sizeof(buffer)));	
}


char* card_pattern_str_n(const CardPattern* patterns, int num, char* buffer, int buflen)
{
	char tmp[128];
	int  len = 0;
	int n;

	for(n = 0; n < num; n++)
	{
		if(patterns[n].value_min == CardValue_None && patterns[n].value_max == CardValue_None)
		{
			sprintf(tmp, "%s", card_value_str(CardValue_None));
		}
		else if(patterns[n].value_min == CardValue_None)
		{
			sprintf(tmp, "点数不超过%s", card_value_str(patterns[n].value_max));
		}
		else if(patterns[n].value_max == CardValue_None)
		{
			sprintf(tmp, "点数至少为%s", card_value_str(patterns[n].value_min));
		}
		else if(patterns[n].value_min == patterns[n].value_max )
		{
			sprintf(tmp, "%s", card_value_str(patterns[n].value_min));
		}
		else
		{
			sprintf(tmp, "点数在%s和%s之间", card_value_str(patterns[n].value_min),card_value_str(patterns[n].value_max));
		}

		len += snprintf(buffer + len, buflen - len, "(%s, %s %s)", 
			patterns[n].id >= CardID_None ? card_id_str(patterns[n].id) : card_type_str((CardType)-patterns[n].id), 
			card_color_str(patterns[n].color), tmp);
	}
	return buffer;

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

RESULT card_match(const Card* pCard,  int nCardNum, const CardPattern* pPattern, int nPatternNum)
{
	int n, m, p;
	int index [MAX_CARD_LIST_NUM];

	// too many card
	if(nPatternNum > MAX_CARD_LIST_NUM)
		return R_E_PARAM;

	if(nCardNum > nPatternNum)
		return R_E_FAIL;

	// fill_array_inc_i(index, num, 0, 1);


	for(n = 0; n < nCardNum; /*n++*/)
	{
		// Calc. first index[n] = ?
		for(m = 0; m < nPatternNum; m++)
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
				for(m = index[n] + 1; m < nPatternNum; m++)
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
				if(m < nPatternNum)
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

	return n == nCardNum ? R_SUCC : R_E_FAIL;
}




CardID  card_maxid(GameContext* pGame)
{
	CardID id = CardID_None;
	lua_State* L = pGame->L;
	lua_getglobal(L, "get_card_maxid");
	lua_call(L, 0, 1);
	if(lua_isnumber(L, -1))
	{
		id = (CardID)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return id;
}


CardID  card_sid2id(GameContext* pGame, const char* sid)
{
	CardID id = CardID_None;
	lua_State* L = pGame->L;
	lua_getglobal(L, "get_card_id_by_sid");
	lua_pushstring(L, sid);
	lua_call(L, 1, 1);
	if(lua_isnumber(L, -1))
	{
		id = (CardID)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return id;
}

CardType card_type(GameContext* pGame, CardID  id)
{
	CardType  t  =  CardType_Unknown;
	lua_State* L = pGame->L;
	lua_getglobal(L, "get_card_type");
	lua_pushnumber(L, id);	
	lua_call(L, 1, 1);
	if(lua_isnumber(L, -1))
	{
		t = (CardType)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return t;
}

char* card_sid(GameContext* pGame, CardID  id, char* buf, int buflen)
{
	lua_State* L = pGame->L;
	lua_getglobal(L, "get_card_sid");
	lua_pushnumber(L, id);	
	lua_call(L, 1, 1);
	if(lua_isstring(L, -1))
	{
		strncpy(buf, lua_tostring(L, -1), buflen);
	}
	else
	{
		buf[0] = 0;
	}
	lua_pop(L, 1);
	return buf;
}

char* card_name(GameContext* pGame, CardID  id, char* buf, int buflen)
{

	lua_State* L = pGame->L;
	lua_getglobal(L, "get_card_name");
	lua_pushnumber(L, id);	
	lua_call(L, 1, 1);
	if(lua_isstring(L, -1))
	{
		strncpy(buf, lua_tostring(L, -1), buflen);
	}
	else
	{
		buf[0] = 0;
	}
	lua_pop(L, 1);
	return buf;
}

char* card_desc(GameContext* pGame, CardID  id, char* buf, int buflen)
{
	lua_State* L = pGame->L;
	lua_getglobal(L, "get_card_desc");
	lua_pushnumber(L, id);	
	lua_call(L, 1, 1);
	if(lua_isstring(L, -1))
	{
		strncpy(buf, lua_tostring(L, -1), buflen);
	}
	else
	{
		buf[0] = 0;
	}
	lua_pop(L, 1);
	return buf;

}

RESULT  card_check_call(CardID  id, GameContext* pGame, GameEventContext* pEvent, int player)
{
	RESULT ret = R_DEF;
	lua_State* L = pGame->L;
	lua_getglobal(L, "call_card_check");	
	tolua_pushnumber(L, id);
	tolua_pushusertype(L, pGame, "GameContext");
	tolua_pushusertype(L, pEvent, "GameEventContext");
	tolua_pushnumber(L, player);
	lua_call(L, 4, 1);
	if(lua_isnumber(L, -1))
	{
		ret = (RESULT)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return ret;
}


RESULT  card_out_call(CardID  id, GameContext* pGame, GameEventContext* pEvent, int player)
{
	RESULT ret = R_DEF;
	lua_State* L = pGame->L;
	lua_getglobal(L, "call_card_out");	
	tolua_pushnumber(L, id);
	tolua_pushusertype(L, pGame, "GameContext");
	tolua_pushusertype(L, pEvent, "GameEventContext");
	tolua_pushnumber(L, player);
	lua_call(L, 4, 1);
	if(lua_isnumber(L, -1))
	{
		ret = (RESULT)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return ret;
}



