#include "config.h"
#include "game.h"
#include "event.h"
#include "player.h"
#include "life.h"
#include "comm.h"




RESULT game_player_add_life(GameContext* pGame, GameEventContext* pParentEvent, int player, int life_inc)
{
	//RESULT ret;
	GameEventContext   event;
	Player* pPlayer;

	// event perlost life
	INIT_EVENT(&event, life_inc < 0 ? GameEvent_PerLostLife: GameEvent_PerAddLife, player, 0, pParentEvent);
	event.pNum = &life_inc;

	trigger_game_event(pGame, &event);

	if(event.result == R_CANCEL || life_inc == 0)
		return R_CANCEL;

	pPlayer = GAME_PLAYER(pGame, player);

	pPlayer->curLife += life_inc;

	if(life_inc < 0)
		MSG_OUT("[%s] lost %d life, cur life is %d/%d\n", pPlayer->name, -life_inc, pPlayer->curLife, pPlayer->maxLife);
	else
		MSG_OUT("[%s] add %d life, cur life is %d/%d\n", pPlayer->name, life_inc, pPlayer->curLife, pPlayer->maxLife);

	if(pPlayer->curLife > pPlayer->maxLife)
		pPlayer->curLife = pPlayer->maxLife;
	else if(pPlayer->curLife <= 0)
	{
		// event perdead ?
		INIT_EVENT(&event, GameEvent_PerDead, player, 0, pParentEvent);
		trigger_game_event(pGame, &event);

		// event dead

		if(pPlayer->curLife <= 0)
		{
			INIT_EVENT(&event, GameEvent_Dead, player, 0, pParentEvent);
			trigger_game_event(pGame, &event);
		}
	}

	INIT_EVENT(&event, life_inc < 0 ? GameEvent_PostLostLife: GameEvent_PostAddLife, player, 0, pParentEvent);
	event.pNum = &life_inc;

	trigger_game_event(pGame, &event);

	return R_SUCC;
}


