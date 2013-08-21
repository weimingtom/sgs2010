#include "config.h"
#include "player.h"
#include "hero.h"
#include "comm.h"

RESULT init_player(Player* pPlayer, PlayerID id, HeroID hero, int index)
{
	//const HeroConfig*  pHeroConfig = get_hero_config(hero);

	if(id < _PlayerID_Valid_Min || id > _PlayerID_Valid_Max)
	{
		MSG_OUT("init player: invalid player ID %d\n", id);
		return R_E_PARAM;
	}

	//if(pHeroConfig == NULL)
	//{
	//	MSG_OUT("init player: invalid hero ID %d\n", hero);
	//	return R_E_FAIL;
	//}

	// is master ?
	/*
	if(id == PlayerID_Master)
	{
		if(pHeroConfig->isMaster == 0)
			return R_E_PARAM;
	}
	*/

	ST_ZERO(*pPlayer);


	pPlayer->id = id;
	pPlayer->hero = hero;

	if(hero != HeroID_None)
	{
		pPlayer->max_life = hero_life(hero);
		hero_name(hero, pPlayer->name, sizeof(pPlayer->name));
	}
	else
	{
		pPlayer->max_life = 3;
		snprintf(pPlayer->name, sizeof(pPlayer->name), "玩家%d", index + 1);
	}

	pPlayer->status = PlayerStatus_Hide;

	if(pPlayer->max_life <= 0)
	{
		pPlayer->max_life = 3;
	}

	// master
	if(id == PlayerID_Master)
	{
		pPlayer->max_life += 1;
		pPlayer->status = PlayerStatus_Show;
	}

	pPlayer->cur_life = pPlayer->max_life;

	pPlayer->hand_card_num = 0;
	pPlayer->judgment_card_num = 0;

	MSG_OUT("init player: id [%s] hero [%s] name [%s] life [%d]\n", player_id_str(pPlayer->id), get_hero_sid(hero), pPlayer->name, pPlayer->max_life);

	return R_SUCC;
}




const char* player_id_str(PlayerID id)
{
	switch(id)
	{
	case PlayerID_Unknown: return "身份未知";
	case PlayerID_None: return "无";
	case PlayerID_Master: return "主公";
	case PlayerID_Minister: return "忠臣";
	case PlayerID_Spy: return "内奸";
	case PlayerID_Mutineer: return "反贼";
	default: return "Invalid";
	}
}

const char* equip_idx_str(int idx)
{
	switch(idx)
	{
	case EquipIdx_Weapon: return "武器";
	case EquipIdx_Armor: return "防具";
	case EquipIdx_HorseInc: return "马+1";
	case EquipIdx_HorseDec: return "马-1";
	}
	return "装备";
}

Card* get_player_handcard(Player* pPlayer, int index)
{
	if(index < 0 || index>= pPlayer->hand_card_num)
		return NULL;
	return PLAYER_HANDCARD(pPlayer, index);
}

VCard* get_player_judgecard(Player* pPlayer, int index)
{
	if(index < 0 || index>= pPlayer->judgment_card_num)
		return NULL;
	return PLAYER_JUDGECARD(pPlayer, index);
}

Card* get_player_equipcard(Player* pPlayer, int index)
{
	if(index < 0 || index>= EquipIdx_Max)
		return NULL;

	if(CARD_VALID(&pPlayer->equip_cards[index]))
		return &pPlayer->equip_cards[index];

	return NULL;
}

int find_player_handcard(Player* pPlayer, CardID id)
{
	int n;
	for(n = 0; n < pPlayer->hand_card_num; n++)
	{
		if(PLAYER_HANDCARD(pPlayer, n)->id == id)
			return n;
	}
	return -1;
}

int find_player_judgecard(Player* pPlayer, CardID id)
{
	int n;
	for(n = 0; n < pPlayer->judgment_card_num; n++)
	{
		if(PLAYER_JUDGECARD(pPlayer, n)->vcard.id == id)
			return n;
	}
	return -1;
}



YESNO is_player_dead(Player* pPlayer)
{
	return B2YESNO(IS_PLAYER_DEAD(pPlayer));
}

YESNO is_player_show(Player* pPlayer)
{
	return B2YESNO(IS_PLAYER_SHOW(pPlayer));
}

YESNO is_player_perdead(Player* pPlayer)
{
	return B2YESNO(IS_PLAYER_PERDEAD(pPlayer));
}

YESNO is_player_handfull(Player* player)
{
	return B2YESNO(player->hand_card_num == MAX_HAND_CARD);
}


int get_player_param(Player* pPlayer, int index)
{
	if(index >= 0 && index < MAX_PLAYER_PARAM)
		return pPlayer->params[index];
	return 0;
}

void set_player_param(Player* pPlayer, int index, int val)
{
	if(index >= 0 && index < MAX_PLAYER_PARAM)
	{
		pPlayer->params[index] = val;
	}
}

void player_set_flag(Player* pPlayer, int flag)
{
	PLAYER_SET_FLAG(pPlayer, flag);
}

void player_clear_flag(Player* pPlayer, int flag)
{
	PLAYER_CLR_FLAG(pPlayer, flag);
}

void player_clear_all_flag(Player* pPlayer)
{
	PLAYER_CLR_ALL_FLAG(pPlayer);
}

YESNO player_check_flag(Player* pPlayer, int flag)
{
	return B2YESNO(PLAYER_CHK_FLAG(pPlayer, flag));
}



int  player_count_card(Player* pPlayer, int where)
{
	int n;
	int count = 0;
	if(where | PatternWhere_Hand)
	{
		for(n = 0; n < pPlayer->hand_card_num; n++)
		{
			if(PLAYER_HANDCARD(pPlayer, n)->flag == CardFlag_None)
			{
				count++;
			}
		}
	}

	if(where | PatternWhere_Equip)
	{
		for(n = 0; n < EquipIdx_Max; n++)
		{
			if(CARD_VALID(PLAYER_EQUIPCARD(pPlayer,n)) && PLAYER_EQUIPCARD(pPlayer,n)->flag == CardFlag_None)
			{
				count++;
			}
		}
	}
	if(where | PatternWhere_Judgment)
	{
		for(n = 0; n < pPlayer->judgment_card_num; n++)
		{
			if(PLAYER_JUDGECARD(pPlayer, n)->vcard.flag == CardFlag_None)
			{
				count++;
			}
		}
	}
	return count;
}



RESULT player_add_hand_card(Player* pPlayer, Card* pCard)
{
	int pos; 

	if(is_player_handfull(pPlayer))
		return R_E_OVERFLOW;

	pos = arrray_insert_t(pPlayer->hand_cards, sizeof(pPlayer->hand_cards[0]), &pPlayer->hand_card_num, -1, pCard);
	PLAYER_HANDCARD(pPlayer, pos)->flag = CardFlag_None;
	return R_SUCC;
}


RESULT player_add_judgment_card(Player* pPlayer, VCard* pCard)
{
	int n;
	if(pPlayer->judgment_card_num >= MAX_JUDGMENT_CARD)
	{
		return R_E_OVERFLOW;
	}

	// 不能有相同ID的判定牌
	for(n = 0; n < pPlayer->judgment_card_num; n++)
	{
		if(pPlayer->judgment_cards[n].vcard.id == pCard->vcard.id)
		{
			MSG_OUT("player_add_judgment_card: failed - 不能有相同ID的判定牌！\n");
			return R_E_CONFLICT;
		}
	}
	
	pPlayer->judgment_cards[pPlayer->judgment_card_num] = *pCard;
	pPlayer->judgment_cards[pPlayer->judgment_card_num].vcard.flag = CardFlag_None;
	pPlayer->judgment_card_num++;
	return R_SUCC;
}

CardFlag get_player_card_flag(Player* pPlayer, CardWhere where, int pos)
{
	switch(where)
	{
	case CardWhere_PlayerHand:
		if(0 <= pos && pos < pPlayer->hand_card_num)
		{
			return pPlayer->hand_cards[pos].flag;
		}
		break;
	case CardWhere_PlayerEquip:
		if(0 <= pos && pos < EquipIdx_Max && CARD_VALID(&pPlayer->equip_cards[pos]))
		{
			return pPlayer->equip_cards[pos].flag;
		}
		break;
	case CardWhere_PlayerJudgment:
		if(0 <= pos && pos < pPlayer->judgment_card_num)
		{
			return pPlayer->judgment_cards[pos].vcard.flag;
		}
		break;
	default:
		break;
	}
	
	return CardFlag_None;
}


RESULT set_player_card_flag(Player* pPlayer,  CardWhere where, int pos, CardFlag flag)
{
	switch(where)
	{
	case CardWhere_PlayerHand:
		if(0 <= pos && pos < pPlayer->hand_card_num)
		{
			pPlayer->hand_cards[pos].flag = flag;
			return R_SUCC;
		}
		break;
	case CardWhere_PlayerEquip:
		if(0 <= pos && pos < EquipIdx_Max && CARD_VALID(&pPlayer->equip_cards[pos]))
		{
			pPlayer->equip_cards[pos].flag = flag;
			return R_SUCC;
		}
		break;
	case CardWhere_PlayerJudgment:
		if(0 <= pos && pos < pPlayer->judgment_card_num)
		{
			pPlayer->judgment_cards[pos].vcard.flag = flag;
			return R_SUCC;
		}
		break;
	default:
		break;
	}

	return R_E_PARAM;
}

static void set_vcard_flag(VCard* pCard, CardFlag flag)
{
	int n;
	pCard->vcard.flag = flag;
	for(n = 0; n < pCard->rnum; n++)
	{
		pCard->rcards[n].flag = flag;
	}
}

RESULT get_player_card(Player* pPlayer, CardWhere where, int pos, VCard* pCard)
{
	switch(where)
	{
	case CardWhere_PlayerHand:
		if(0 <= pos && pos < pPlayer->hand_card_num)
		{
			set_vcard_from_card(pCard, &pPlayer->hand_cards[pos]);
			set_vcard_flag(pCard, CardFlag_FromHand);
			return R_SUCC;
		}
		break;
	case CardWhere_PlayerEquip:
		if(0 <= pos && pos < EquipIdx_Max && CARD_VALID(&pPlayer->equip_cards[pos]))
		{
			set_vcard_from_card(pCard, &pPlayer->equip_cards[pos]);
			set_vcard_flag(pCard, CardFlag_FromEquip);
			return R_SUCC;
		}
		break;
	case CardWhere_PlayerJudgment:
		if(0 <= pos && pos < pPlayer->judgment_card_num)
		{
			*pCard = pPlayer->judgment_cards[pos];
			set_vcard_flag(pCard, CardFlag_FromJudge);
			return R_SUCC;
		}
		break;
	default:
		break;
	}

	return R_E_PARAM;

}


RESULT player_remove_card(Player* pPlayer, CardWhere where, int pos, VCard* pCard)
{
	switch(where)
	{
	case CardWhere_PlayerHand:
		if(0 <= pos && pos < pPlayer->hand_card_num)
		{
			if(pCard != NULL) set_vcard_from_card(pCard, &pPlayer->hand_cards[pos]);
			arrray_remove_t(pPlayer->hand_cards, sizeof(pPlayer->hand_cards[0]), &pPlayer->hand_card_num, pos, NULL);
			return R_SUCC;
		}

		break;
	case CardWhere_PlayerEquip:
		if(0 <= pos && pos < EquipIdx_Max && pPlayer->equip_cards[pos].id != CardID_None && CARD_VALID(&pPlayer->equip_cards[pos]))
		{
			if(pCard != NULL) set_vcard_from_card(pCard, &pPlayer->equip_cards[pos]);
			RESET_CARD(&pPlayer->equip_cards[pos]);
			return R_SUCC;
		}
		break;
	case CardWhere_PlayerJudgment:
		if(0 <= pos && pos < pPlayer->judgment_card_num)
		{
			if(pCard != NULL) *pCard = pPlayer->judgment_cards[pos];
			arrray_remove_t(pPlayer->judgment_cards, sizeof(pPlayer->judgment_cards[0]), &pPlayer->judgment_card_num, pos, NULL);
			// the number of card in judging, if pos in it, decrease it
			if(pPlayer->card_in_judge_num > pos)
			{
				pPlayer->card_in_judge_num--;
			}
			return R_SUCC;
		}
		break;
	default:
		break;
	}

	return R_E_PARAM;
}


RESULT player_card_idx_to_pos(Player* player, int idx, CardWhere* where, int* pos)
{
	int n;
	if(idx < 1 )
		return R_E_PARAM;

	if(idx <= player->hand_card_num)
	{
		*where = CardWhere_PlayerHand;
		*pos = idx - 1;
		return R_SUCC;
	}

	idx -= player->hand_card_num;

	for(n = 0; n < EquipIdx_Max; n++)
	{
		if(CARD_VALID(&player->equip_cards[n]))
		{
			if(idx == 1)
			{
				*where = CardWhere_PlayerEquip;
				*pos = n;
				return R_SUCC;
			}
			idx--;
		}
	}

	if(idx <= player->judgment_card_num)
	{
		*where = CardWhere_PlayerJudgment;
		*pos = idx - 1;
		return R_SUCC;
	}

	return R_E_FAIL;
}

/*
RESULT player_get_cards_pos(Player* pPlayer, const int* idx, int num, PosVCard*  pPosCards)
{
	int n, m;
	CardWhere   where;
	int  pos;
	for(n = 0; n < num; n++)
	{
		if(R_SUCC != player_card_idx_to_pos(pPlayer, idx[n], &where, &pos))
			return R_E_POS;
		for(m = 0; m < n; m++)
		{
			if(pPosCards[m].where == where && pPosCards[m].pos == pos)
				return R_E_POS;
		}
		if(R_SUCC != get_player_card(pPlayer, where, pos, &pPosCards[n].card))
			return R_E_POS;

		pPosCards[n].where = where;
		pPosCards[n].pos = pos;

	}

	return R_SUCC;
}

RESULT player_set_cards_flag(Player* pPlayer, const PosCard* pPosCards, int num, CardFlag flag)
{
	RESULT  ret;
	int n;
	for(n = 0; n < num; n++)
	{
		ret = set_player_card_flag(pPlayer, pPosCards[n].where, pPosCards[n].pos, flag);

		if(ret != R_SUCC)
			return ret;
	}
	return R_SUCC;
}

*/