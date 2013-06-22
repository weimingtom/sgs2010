// event.cpp 



#include "config.h"
#include "event.h"
#include "game.h"
#include "player.h"
#include "card.h"
#include "hero.h"




static RESULT game_event_check_player(GameContext* pGame, GameEventContext* pEvent, int index)
{
	// check player skill
	Player* pPlayer;
	const HeroConfig* pHero;

	pPlayer = &pGame->players[index];

	pHero = get_hero_config(pPlayer->hero);


	// check player out card

	return R_SUCC;
}



RESULT trigger_game_event(GameContext* pGame, GameEventContext* pEvent)
{
	int n;
	int m = pGame->nCurPlayer;

	for(n = 0; n < pGame->nPlayerCount; n++)
	{
		game_event_check_player(pGame, pEvent, m);
		if(pEvent->block)
			break;
		m = game_next_player(pGame, m);
	}

	pGame->nCurPlayer = m;
	return R_SUCC;
}
