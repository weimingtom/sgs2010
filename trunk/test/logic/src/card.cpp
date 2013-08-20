#include "config.h"
#include "card.h"
#include "comm.h"
#include "script.h"
//#include "card_impl/card_impl.h"


const char* card_type_str(CardType type)
{
	switch(type)
	{
	case CardType_Unknown: return "未知";
	case CardType_None: return "任意种类";
	case CardType_Normal: return "普通";
	case CardType_Strategy: return "锦囊";
	case CardType_DelayStrategy: return "延时锦囊";
	case CardType_Armor: return "防具";
	case CardType_Weapon: return "武器";
	case CardType_HorseInc: return "马(+1)";
	case CardType_HorseDec: return "马(-1)";
	default: return "Invalid";
	}
}

/*
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
*/


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

/*
const char* card_flag_str(CardFlag flag)
{
	static char buf[64];
	switch(flag)
	{
	case CardFlag_None: return "CardFlag_None";
	case CardFlag_PrepareOut: return "CardFlag_PrepareOut";   // the card prepare to out, in prepare event, cannot calc the card effect
	case CardFlag_PrepareDiscard: return "CardFlag_PrepareDiscard";   // the card prepare to discard, in prepare event, cannot calc the card effect
	case CardFlag_PrepareSupply: return "CardFlag_PrepareSupply";   // the card prepare to give other player, in prepare event, cannot calc the card effect
	case CardFlag_InUse: return "CardFlag_InUse";
	case CardFlag_InJudge: return "CardFlag_InJudge";
	case CardFlag_FromGetStack: return "CardFlag_FromGetStack";
	case CardFlag_FromHand: return "CardFlag_FromHand";
	case CardFlag_FromEquip: return "CardFlag_FromEquip";
	case CardFlag_FromJudge: return "CardFlag_FromJudge";
	default: snprintf(buf, sizeof(buf), "0x%x", (int)flag); return buf;
	}
}
*/

const char* get_card_str(const Card* card)
{
	static char buffer[128];
	return card_str_n(card, 1, buffer, sizeof(buffer));
}

const char* get_vcard_str(const VCard* vcard)
{
	static char buffer[256];
	return vcard_str_n(vcard, 1, buffer, sizeof(buffer));
}

const char* get_pattern_str(const CardPattern* cardp)
{
	static char buffer[256];
	return card_pattern_str_n(cardp, 1, buffer, sizeof(buffer));
}


/*
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
*/


char* card_str_n(const Card* pCard, int num, char* buffer, int buflen)
{
	int n;
	int len = 0;
	//char  name[128];
	for(n = 0; n < num; n++)
	{
		if(CARD_VALID(&pCard[n]))
		{
			len += snprintf(buffer + len, buflen - len, "(%s, %s %s)", get_card_name(pCard[n].id), card_color_str(pCard[n].color), card_value_str(pCard[n].value));
		}
		else
		{
			len += snprintf(buffer + len, buflen - len, "(空)");
		}
	}
	return buffer;
}




char* card_simple_str(const Card* pCard, char* buffer, int buflen)
{
	buffer[0] = 0;
	return buffer;
}

char* vcard_str_n(const VCard* pCard, int num, char* buffer, int buflen)
{
	int n, m;
	int len = 0;
	// char  name[128];
	for(n = 0; n < num; n++)
	{
		if(pCard->rnum == 1 && CARD_EQUAL(&pCard->vcard, &pCard->rcards[0]))
		{
			// 按真实牌，这里不再输出
			card_str(&pCard[n].vcard, buffer + len, buflen - len);
			len += strlen(buffer + len);
		}
		else
		{
			len += snprintf(buffer + len, buflen - len, "[");
			for(m = 0; m < pCard->rnum; m++)
			{
				card_str(&pCard[n].rcards[m], buffer + len, buflen - len);
				len += strlen(buffer + len);
			}
			len += snprintf(buffer + len, buflen - len, "=>");
			card_str(&pCard[n].vcard, buffer + len, buflen - len);
			len += strlen(buffer + len);
			len += snprintf(buffer + len, buflen - len, "]");
		}
	}
	return buffer;
}


void card_dump(const Card* pCard)
{
	char buffer[128];
	MSG_OUT("%s", card_str(pCard, buffer, sizeof(buffer)));	
}


void set_vcard_from_card(VCard* pVCard, const Card* pCard)
{
	pVCard->vcard = *pCard;
	pVCard->rnum = 1;
	pVCard->rcards[0] = *pCard;
}


RESULT load_card_pattern(CardPattern* pCardPattern, const char* szPattern, int len)
{
	const char* p;
	const char* pe;
	int   tln;
	char  tmp[128];

	p = szPattern;

	pe = len > 0 ? p + len : p + strlen(p);

	if(p >= pe)
	{
		// empty pattern string
		return R_E_PARAM;
	}

	// <{sid}>
	if(p < pe && *p == '{')
	{
		p++;
		// get {sid}
		tln = 0;
		while(p < pe && *p != '}')
		{
			if(tln > (int)sizeof(tmp) - 1)
			{
				// too long sid string
				return R_E_FAIL;
			}
			tmp[tln] = *p;
			tln++;
			p++;
		}
		tmp[tln] = 0;

		if(p >= pe || *p != '}')
		{
			// expected '}' for end of sid
			return R_E_FAIL;
		}

		p++;

		if(0 == strcmp(tmp, "none"))
		{
			pCardPattern->id = CardID_None;
		}
		else
		{
			pCardPattern->id = card_sid2id(tmp);
			if(pCardPattern->id == (CardID_None))
			{
				// invalid card sid
				return R_E_FAIL;
			}
		}
	}

	if(p < pe)
	{

		switch(*p)
		{
		case 's': pCardPattern->color = CardColor_Spade; break;
		case 'h': pCardPattern->color = CardColor_Heart; break;
		case 'd': pCardPattern->color = CardColor_Diamond; break;
		case 'c': pCardPattern->color = CardColor_Club; break;
		case 'b': pCardPattern->color = CardColor_GeneralBlack; break;
		case 'r': pCardPattern->color = CardColor_GeneralRed; break;
		case 'n': pCardPattern->color = CardColor_None; break;
		default:  goto  __v_parse;
		}
		p++;
	}

__v_parse:
	if(p < pe)
	{

		switch(*p)
		{
		case 'A': pCardPattern->value_min = CardValue_A; break;
		case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
			pCardPattern->value_min = (CardValue)(CardValue_2 + (*p - '2'));
			break;
		case '1':
			if(p+1 < pe && *(p+1) == '0')
			{
				p++;
				pCardPattern->value_min = CardValue_10;
			}
			else
			{
				// expected '0' for card value 10
				return R_E_FAIL;
			}
			break;
		case 'J': pCardPattern->value_min = CardValue_J; break;
		case 'Q': pCardPattern->value_min = CardValue_Q; break;
		case 'K': pCardPattern->value_min = CardValue_K; break;
		case 'N': pCardPattern->value_min = CardValue_None; break;
		default: goto __fini_parse;
		}
		p++;

		if(p < pe)
		{
			if(*p != '-')
			{
				// error format
				return R_E_FAIL;
			}
			p++;

			if(p >= pe)
			{
				// error format
				return R_E_FAIL;
			}

			switch(*p)
			{
			case 'A': pCardPattern->value_max = CardValue_A; break;
			case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
				pCardPattern->value_max = (CardValue)(CardValue_2 + (*p - '2'));
				break;
			case '1':
				if(p+1 < pe && *(p+1) == '0')
				{
					p++;
					pCardPattern->value_max = CardValue_10;
				}
				else
				{
					// expected '0' for card value 10
					return R_E_FAIL;
				}
				break;
			case 'J': pCardPattern->value_max = CardValue_J; break;
			case 'Q': pCardPattern->value_max = CardValue_Q; break;
			case 'K': pCardPattern->value_max = CardValue_K; break;
			case 'N': pCardPattern->value_max = CardValue_None; break;
			default: return R_E_FAIL;
			}
			p++;

		}
		else
		{
			pCardPattern->value_max = pCardPattern->value_min;
		}
	}

__fini_parse:

	if(p != pe)
		return R_E_FAIL;

	return R_SUCC;
}


char* card_pattern_str_n(const CardPattern* patterns, int num, char* buffer, int buflen)
{
	//char name[128];
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
			patterns[n].id >= CardID_None ? get_card_name(patterns[n].id) : card_type_str((CardType)-patterns[n].id), 
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
			// const CardConfig* pCardConfig = get_card_config(pCard->id);
			//if(pCardConfig == NULL || pCardConfig->type != -pPattern->id)
			//	return R_E_FAIL;
			if(card_type(pCard->id) != -pPattern->id)
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

RESULT card_match(const Card* pCard, size_t offset,  int nCardNum, const CardPattern* pPattern, int nPatternNum)
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

		while(0 != card_match_one((Card*)((char*)pCard+(n*offset)), pPattern+index[n]))
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

/*

void set_card_id(Card* pCard, CardID id)
{
	pCard->id = id;
}

void set_card_color(Card* pCard, CardColor cl)
{
	pCard->color = cl;
}

void set_card_value(Card* pCard, CardValue val)
{
	pCard->value = val;
}

void set_card_flag(Card* pCard, CardFlag flag)
{
	pCard->flag = flag;
}
*/



CardID  card_maxid()
{
	CardID id = CardID_None;
	lua_State* L = get_game_script();
	lua_getglobal(L, "get_card_maxid");
	lua_call(L, 0, 1);
	if(lua_isnumber(L, -1))
	{
		id = (CardID)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return id;
}


CardID  card_sid2id(const char* sid)
{
	CardID id = CardID_None;
	lua_State* L = get_game_script();
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

CardType card_type(CardID  id)
{
	CardType  t  =  CardType_Unknown;
	lua_State* L = get_game_script();
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

char* card_sid(CardID  id, char* buf, int buflen)
{
	lua_State* L = get_game_script();
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

char* card_name(CardID  id, char* buf, int buflen)
{
	lua_State* L = get_game_script();
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

char* card_desc(CardID  id, char* buf, int buflen)
{
	lua_State* L = get_game_script();
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

YESNO  call_card_can_out(CardID  id, GameContext* pGame, GameEventContext* pEvent, int player, PosCard* pos_card)
{
	YESNO ret = NO;
	lua_State* L = get_game_script();
	lua_getglobal(L, "call_card_can_out");	
	tolua_pushnumber(L, id);
	tolua_pushusertype(L, pGame, "GameContext");
	tolua_pushusertype(L, pEvent, "GameEventContext");
	tolua_pushnumber(L, player);
	tolua_pushusertype(L, pos_card, "PosCard");
	lua_call(L, 5, 1);
	if(lua_isnumber(L, -1))
	{
		ret = (YESNO)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return ret;
}


CANUSE  call_card_can_use(CardID  id, GameContext* pGame, GameEventContext* pEvent, int player, PosCard* pos_card)
{
	CANUSE ret = USE_CANNOT;
	lua_State* L = get_game_script();
	lua_getglobal(L, "call_card_can_use");	
	tolua_pushnumber(L, id);
	tolua_pushusertype(L, pGame, "GameContext");
	tolua_pushusertype(L, pEvent, "GameEventContext");
	tolua_pushnumber(L, player);
	tolua_pushusertype(L, pos_card, "PosCard");
	lua_call(L, 5, 1);
	if(lua_isnumber(L, -1))
	{
		ret = (CANUSE)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return ret;
}


RESULT  call_card_event(CardID  id, GameContext* pGame, GameEventContext* pEvent, int player)
{
	RESULT ret = R_DEF;
	lua_State* L = get_game_script();
	lua_getglobal(L, "call_card_event");	
	tolua_pushnumber(L, id);
	tolua_pushusertype(L, pGame, "GameContext");
	tolua_pushusertype(L, pEvent, "GameEventContext");
	tolua_pushnumber(L, player);
	//tolua_pushusertype(L, pos_card, "PosCard");
	lua_call(L, 4, 1);
	if(lua_isnumber(L, -1))
	{
		ret = (RESULT)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);
	return ret;
}



const char* get_card_sid(CardID  id)
{
	static char buf[128];
	return card_sid(id, buf, sizeof(buf));
}

const char* get_card_name(CardID  id)
{
	static char buf[128];
	return card_name(id, buf, sizeof(buf));
}

const char* get_card_desc(CardID  id)
{
	static char buf[1024];
	return card_desc(id, buf, sizeof(buf));
}

