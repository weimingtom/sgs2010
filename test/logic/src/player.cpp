#include "config.h"
#include "player.h"
#include "hero.h"
#include "comm.h"

int init_player(Player* pPlayer, int id, int hero)
{
	const HeroConfig*  pHeroConfig = get_hero_config(hero);

	if(id < PlayerID_Valid_Min || id > PlayerID_Valid_Max)
	{
		printf("init player: invalid player ID %d\n", id);
		return -1;
	}

	if(pHeroConfig == NULL)
	{
		printf("init player: invalid hero ID %d\n", hero);
		return -2;
	}

	// is master ?
	/*
	if(id == PlayerID_Master)
	{
		if(pHeroConfig->isMaster == 0)
			return -3;
	}
	*/

	memset(pPlayer, 0, sizeof(*pPlayer));


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

	return 0;
}




const char* player_id_str(int id)
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




int player_remove_card(Player* pPlayer, int pos)
{
	int where = PLAYER_CARD_WHERE(pos);
	int index = PLAYER_CARD_INDEX(pos);

	switch(where)
	{
	case PlayerCard_Hand:
		if(pos < pPlayer->nHandCardNum)
		{
			arrray_remove_t(pPlayer->stHandCards, sizeof(pPlayer->stHandCards[0]), &pPlayer->nHandCardNum, index, NULL);
			return 0;
		}

		break;
	case PlayerCard_Equip:
		if(pos < EquipIdx_Max && pPlayer->stEquipCard[index].id != CardID_None)
		{
			memset(&pPlayer->stEquipCard[index], 0, sizeof(pPlayer->stEquipCard[index]));
			return 0;
		}
		break;
	case PlayerCard_Judgment:
		if(pos < pPlayer->nJudgmentCardNum)
		{
			arrray_remove_t(pPlayer->stJudgmentCards, sizeof(pPlayer->stJudgmentCards[0]), &pPlayer->nJudgmentCardNum, index, NULL);
			return 0;
		}
		break;
	}

	return -1;
}



