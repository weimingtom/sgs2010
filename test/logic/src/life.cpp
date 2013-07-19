#include "config.h"
#include "game.h"
#include "event.h"
#include "player.h"
#include "life.h"
#include "comm.h"


int  get_changelife_delta(ChangeLife* pChangeLife)
{
	return pChangeLife->delta;
}

int  get_changelife_after_life(ChangeLife* pChangeLife)
{
	return pChangeLife->after_life;
}

int  get_changelife_src_player(ChangeLife* pChangeLife)
{
	return pChangeLife->src_player;
}

OutCard*  get_changelife_src_cards(ChangeLife* pChangeLife)
{
	return &pChangeLife->src_cards;
}

int  get_changelife_src_skill(ChangeLife* pChangeLife)
{
	return pChangeLife->src_skill;
}


void  set_changelife_delta(ChangeLife* pChangeLife, int delta)
{
	pChangeLife->delta = delta;
}


RESULT game_player_add_life(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, int life_inc, int src_player, OutCard* src_cards, int src_skill)
{
	//RESULT ret;
	GameEventContext   event;
	Player* pPlayer;
	ChangeLife   stChgLife;


	if(!IS_PLAYER_VALID(pGame, player))
	{
		if(L) {
			luaL_error(L, "game_player_add_life: invalid player index - %d", player );
		} else {
			MSG_OUT("game_player_add_life: invalid player index - %d\n", player );
		}
		return R_E_FAIL;
	}


	ST_ZERO(stChgLife);

	stChgLife.delta = life_inc;
	stChgLife.after_life = 0;
	stChgLife.src_player = src_player;
	stChgLife.src_cards = *src_cards;
	stChgLife.src_skill = src_skill;

	// event per change life
	INIT_EVENT(&event, GameEvent_PerChangeLife, player, 0, pParentEvent);
	event.pChangeLife = &stChgLife;

	trigger_game_event(pGame, &event);

	// in event some skill can modify the delta, for fix the damage
	if(event.result == R_CANCEL || stChgLife.delta == 0)
		return R_CANCEL;

	pPlayer = GAME_PLAYER(pGame, player);

	pPlayer->curLife += stChgLife.delta;

	if(stChgLife.delta < 0)
		MSG_OUT("[%s] lost %d life, cur life is %d/%d\n", pPlayer->name, -stChgLife.delta, pPlayer->curLife, pPlayer->maxLife);
	else
		MSG_OUT("[%s] add %d life, cur life is %d/%d\n", pPlayer->name, stChgLife.delta, pPlayer->curLife, pPlayer->maxLife);

	if(pPlayer->curLife > pPlayer->maxLife)
	{
		pPlayer->curLife = pPlayer->maxLife;
	}

	stChgLife.after_life = pPlayer->curLife;

	if(pPlayer->curLife <= 0)
	{
		// event perdead ?
		INIT_EVENT(&event, GameEvent_PerDead, player, 0, pParentEvent);
		trigger_game_event(pGame, &event);

		// event dead

		if(pPlayer->curLife <= 0)
		{
			// set the player to dead
			pPlayer->status = PlayerStatus_Dead;

			// check the game over?
			game_check_gameover(pGame, player);

			INIT_EVENT(&event, GameEvent_Dead, player, 0, pParentEvent);
			trigger_game_event(pGame, &event);
		}
	}

	INIT_EVENT(&event, GameEvent_PostChangeLife, player, 0, pParentEvent);
	event.pChangeLife = &stChgLife;

	trigger_game_event(pGame, &event);

	return R_SUCC;
}


