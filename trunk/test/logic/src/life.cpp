#include "config.h"
#include "game.h"
#include "event.h"
#include "player.h"
#include "life.h"
#include "comm.h"



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
	INIT_EVENT(&event, GameEvent_PerChangeLife, player, INVALID_PLAYER, pParentEvent);
	event.change_life = &stChgLife;

	trigger_game_event(pGame, &event);

	// in event some skill can modify the delta, for fix the damage
	if(event.result == R_CANCEL || stChgLife.delta == 0)
		return R_CANCEL;

	pPlayer = GAME_PLAYER(pGame, player);

	pPlayer->cur_life += stChgLife.delta;

	if(stChgLife.delta < 0)
		MSG_OUT("[%s] lost %d life, cur life is %d/%d\n", pPlayer->name, -stChgLife.delta, pPlayer->cur_life, pPlayer->max_life);
	else
		MSG_OUT("[%s] add %d life, cur life is %d/%d\n", pPlayer->name, stChgLife.delta, pPlayer->cur_life, pPlayer->max_life);

	if(pPlayer->cur_life > pPlayer->max_life)
	{
		pPlayer->cur_life = pPlayer->max_life;
	}

	stChgLife.after_life = pPlayer->cur_life;

	if(pPlayer->cur_life <= 0)
	{
		// event perdead ?
		INIT_EVENT(&event, GameEvent_PerDead, player, INVALID_PLAYER, pParentEvent);
		trigger_game_event(pGame, &event);

		// event dead

		if(pPlayer->cur_life <= 0)
		{
			// set the player to dead
			pPlayer->status = PlayerStatus_Dead;

			// check the game over?
			game_check_gameover(pGame, player);

			INIT_EVENT(&event, GameEvent_Dead, player, INVALID_PLAYER, pParentEvent);
			trigger_game_event(pGame, &event);
		}
	}

	INIT_EVENT(&event, GameEvent_PostChangeLife, player, INVALID_PLAYER, pParentEvent);
	event.change_life = &stChgLife;

	trigger_game_event(pGame, &event);

	return R_SUCC;
}


