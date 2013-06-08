#include "config.h"
#include "player.h"
#include "hero.h"

int init_player(Player* pPlayer, int id, int hero)
{
	const HeroConfig*  pHeroConfig = get_hero_config(hero);

	if(id < PlayerID_Valid_Min || id > PlayerID_Valid_Max)
		return -1;

	if(pHeroConfig == NULL)
		return -2;

	// is master ?
	if(id == PlayerID_Master)
	{
		if(pHeroConfig->isMaster == 0)
			return -3;
	}

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

	return 0;
}




















