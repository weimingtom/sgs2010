#include "config.h"
#include "player.h"
#include "hero.h"
#include "comm.h"

RESULT init_player(Player* pPlayer, PlayerID id, HeroID hero)
{
	const HeroConfig*  pHeroConfig = get_hero_config(hero);

	if(id < PlayerID_Valid_Min || id > PlayerID_Valid_Max)
	{
		printf("init player: invalid player ID %d\n", id);
		return R_E_PARAM;
	}

	if(pHeroConfig == NULL)
	{
		printf("init player: invalid hero ID %d\n", hero);
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
	pPlayer->maxLife = pHeroConfig->life;

	// master
	if(id == PlayerID_Master)
	{
		pPlayer->maxLife += 1;
	}

	pPlayer->curLife = pPlayer->maxLife;

	strncpy(pPlayer->name, pHeroConfig->name, MAX_NAME_LEN);

	pPlayer->nHandCardNum = 0;
	pPlayer->nJudgmentCardNum = 0;

	printf("init player: id [%s] hero [%s] life [%d]\n", player_id_str(pPlayer->id), pHeroConfig->name, pPlayer->maxLife);

	return R_SUCC;
}




const char* player_id_str(PlayerID id)
{
	switch(id)
	{
	case PlayerID_Unknown: return "未知";
	case PlayerID_None: return "无";
	case PlayerID_Master: return "主公";
	case PlayerID_Minster: return "忠臣";
	case PlayerID_Spy: return "内奸";
	case PlayerID_Mutineer: return "反贼";
	default: return "Invalid";
	}
}


YESNO is_player_handfull(Player* player)
{
	return B2YESNO(player->nHandCardNum < MAX_HAND_CARD);
}


RESULT player_add_hand_card(Player* pPlayer, Card* pCard)
{
	if(is_player_handfull(pPlayer))
		return R_E_OVERFLOW;

	arrray_insert_t(pPlayer->stHandCards, sizeof(pPlayer->stHandCards[0]), &pPlayer->nHandCardNum, -1, pCard);
	return R_SUCC;
}


RESULT player_remove_card(Player* pPlayer, int where, int pos, Card* pCard)
{
	switch(where)
	{
	case PlayerCard_Hand:
		if(0 <= pos && pos < pPlayer->nHandCardNum)
		{
			if(pCard != NULL) *pCard = pPlayer->stHandCards[pos];
			arrray_remove_t(pPlayer->stHandCards, sizeof(pPlayer->stHandCards[0]), &pPlayer->nHandCardNum, pos, NULL);
			return R_SUCC;
		}

		break;
	case PlayerCard_Equip:
		if(0 <= pos && pos < EquipIdx_Max && pPlayer->stEquipCard[pos].id != CardID_None)
		{
			if(pCard != NULL) *pCard = pPlayer->stEquipCard[pos];
			RESET_CARD(&pPlayer->stEquipCard[pos]);
			return R_SUCC;
		}
		break;
	case PlayerCard_Judgment:
		if(0 <= pos && pos < pPlayer->nJudgmentCardNum)
		{
			if(pCard != NULL) *pCard = pPlayer->stJudgmentCards[pos];
			arrray_remove_t(pPlayer->stJudgmentCards, sizeof(pPlayer->stJudgmentCards[0]), &pPlayer->nJudgmentCardNum, pos, NULL);
			return R_SUCC;
		}
		break;
	}

	return R_E_PARAM;
}


RESULT player_card_idx_to_pos(Player* player, int idx, int* where, int* pos)
{
	int n;
	if(idx < 1 )
		return R_E_PARAM;

	if(idx <= player->nHandCardNum)
	{
		*where = PlayerCard_Hand;
		*pos = idx - 1;
		return R_SUCC;
	}

	idx -= player->nHandCardNum;

	for(n = 0; n < EquipIdx_Max; n++)
	{
		if(player->stEquipCard[n].id != CardID_None)
		{
			if(idx == 1)
			{
				*where = PlayerCard_Equip;
				*pos = n;
				return R_SUCC;
			}
			idx--;
		}
	}

	if(idx <= player->nJudgmentCardNum)
	{
		*where = PlayerCard_Judgment;
		*pos = idx - 1;
		return R_SUCC;
	}

	return R_E_FAIL;
}


