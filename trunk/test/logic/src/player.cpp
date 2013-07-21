#include "config.h"
#include "player.h"
#include "hero.h"
#include "comm.h"

RESULT init_player(Player* pPlayer, PlayerID id, HeroID hero)
{
	const HeroConfig*  pHeroConfig = get_hero_config(hero);

	if(id < PlayerID_Valid_Min || id > PlayerID_Valid_Max)
	{
		MSG_OUT("init player: invalid player ID %d\n", id);
		return R_E_PARAM;
	}

	if(pHeroConfig == NULL)
	{
		MSG_OUT("init player: invalid hero ID %d\n", hero);
		return R_E_FAIL;
	}

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
	pPlayer->max_life = pHeroConfig->life;
	pPlayer->status = PlayerStatus_Hide;

	// master
	if(id == PlayerID_Master)
	{
		pPlayer->max_life += 1;
		pPlayer->status = PlayerStatus_Show;
	}

	pPlayer->cur_life = pPlayer->max_life;

	strncpy(pPlayer->name, pHeroConfig->name, MAX_NAME_LEN);

	pPlayer->hand_card_num = 0;
	pPlayer->judgment_card_num = 0;

	MSG_OUT("init player: id [%s] hero [%s] life [%d]\n", player_id_str(pPlayer->id), pHeroConfig->name, pPlayer->max_life);

	return R_SUCC;
}




const char* player_id_str(PlayerID id)
{
	switch(id)
	{
	case PlayerID_Unknown: return "身份未知";
	case PlayerID_None: return "无";
	case PlayerID_Master: return "主公";
	case PlayerID_Minster: return "忠臣";
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
	case EquipIdx_HorseInc: return "马(+1)";
	case EquipIdx_HorseDec: return "马(-1)";
	}
	return "装备";
}

Card* get_player_handcard(Player* pPlayer, int index)
{
	if(index < 0 || index>= pPlayer->hand_card_num)
		return NULL;
	return PLAYER_HANDCARD(pPlayer, index);
}

Card* get_player_judgecard(Player* pPlayer, int index)
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





RESULT player_add_hand_card(Player* pPlayer, Card* pCard)
{
	if(is_player_handfull(pPlayer))
		return R_E_OVERFLOW;

	arrray_insert_t(pPlayer->hand_cards, sizeof(pPlayer->hand_cards[0]), &pPlayer->hand_card_num, -1, pCard);
	return R_SUCC;
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
			pPlayer->judgment_cards[pos].flag = flag;
			return R_SUCC;
		}
		break;
	default:
		break;
	}

	return R_E_PARAM;
}


RESULT get_player_card(Player* pPlayer, CardWhere where, int pos, Card* pCard)
{
	switch(where)
	{
	case CardWhere_PlayerHand:
		if(0 <= pos && pos < pPlayer->hand_card_num)
		{
			*pCard = pPlayer->hand_cards[pos];
			pCard->flag = CardFlag_FromHand;
			return R_SUCC;
		}
		break;
	case CardWhere_PlayerEquip:
		if(0 <= pos && pos < EquipIdx_Max && CARD_VALID(&pPlayer->equip_cards[pos]))
		{
			*pCard = pPlayer->equip_cards[pos];
			pCard->flag = CardFlag_FromEquip;
			return R_SUCC;
		}
		break;
	case CardWhere_PlayerJudgment:
		if(0 <= pos && pos < pPlayer->judgment_card_num)
		{
			*pCard = pPlayer->judgment_cards[pos];
			pCard->flag = CardFlag_FromJudge;
			return R_SUCC;
		}
		break;
	default:
		break;
	}

	return R_E_PARAM;

}


RESULT player_remove_card(Player* pPlayer, CardWhere where, int pos, Card* pCard)
{
	switch(where)
	{
	case CardWhere_PlayerHand:
		if(0 <= pos && pos < pPlayer->hand_card_num)
		{
			if(pCard != NULL) *pCard = pPlayer->hand_cards[pos];
			arrray_remove_t(pPlayer->hand_cards, sizeof(pPlayer->hand_cards[0]), &pPlayer->hand_card_num, pos, NULL);
			return R_SUCC;
		}

		break;
	case CardWhere_PlayerEquip:
		if(0 <= pos && pos < EquipIdx_Max && pPlayer->equip_cards[pos].id != CardID_None && CARD_VALID(&pPlayer->equip_cards[pos]))
		{
			if(pCard != NULL) *pCard = pPlayer->equip_cards[pos];
			RESET_CARD(&pPlayer->equip_cards[pos]);
			return R_SUCC;
		}
		break;
	case CardWhere_PlayerJudgment:
		if(0 <= pos && pos < pPlayer->judgment_card_num)
		{
			if(pCard != NULL) *pCard = pPlayer->judgment_cards[pos];
			arrray_remove_t(pPlayer->judgment_cards, sizeof(pPlayer->judgment_cards[0]), &pPlayer->judgment_card_num, pos, NULL);
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


