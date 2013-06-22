// skill.cpp 

#include "config.h"
#include "comm.h"
#include "game.h"
#include "event.h"
#include "card.h"
#include "skill.h"
#include "cmd.h"



RESULT game_useskill(GameContext* pGame, GameEventContext* pEvent, int idx)
{
	Player* p = CUR_PLAYER(pGame);

	const HeroConfig* pHero = get_hero_config(p->hero);

	if(pHero == NULL)
	{
		printf("skill (%d) not exist!\n", idx );
		return R_E_PARAM;
	}

	if(idx < 1 || idx > pHero->skillNum)
	{
		printf("invalid skill (%d) !\n", idx );
		return R_E_PARAM;
	}


	if(pHero->skills[idx-1].check == NULL || (*pHero->skills[idx-1].check)(pGame, NULL, pGame->nCurPlayer) != YES)
	{
		printf("cannot use skill '%s'!\n", pHero->skills[idx-1].name );
		return R_E_STATUS;
	}

	// todo: trigger event per use skill

	return  (*pHero->skills[idx-1].use)(pGame, NULL, pGame->nCurPlayer);

	// post trigger use skill

	return R_SUCC;
}


RESULT game_cancelskill(GameContext* pGame, GameEventContext* pEvent)
{
	return R_CANCEL;
}


