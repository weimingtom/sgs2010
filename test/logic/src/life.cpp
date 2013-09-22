#include "config.h"
#include "game.h"
#include "event.h"
#include "player.h"
#include "life.h"
#include "script.h"
#include "cmd.h"

static RESULT game_player_dead(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player)
{
	GameEventContext   event;
	Player* pPlayer;

	pPlayer = get_game_player(pGame, player);

	MSG_OUT("��%s���������ͣ��������״̬��\n", pPlayer->name);

	// event perdead ?
	INIT_EVENT(&event, GameEvent_PerDead, player, INVALID_PLAYER, pParentEvent);
	trigger_game_event(pGame, &event);

	// ignore event result


	// event dead

	if(pPlayer->cur_life <= 0)
	{

		// check the game over?
		game_check_gameover(pGame, player);

		INIT_EVENT(&event, GameEvent_Dead, player, INVALID_PLAYER, pParentEvent);
		trigger_game_event(pGame, &event);

		// set the player to dead
		pPlayer->status = PlayerStatus_Dead;

		// after the player dead
		INIT_EVENT(&event, GameEvent_PostDead, player, INVALID_PLAYER, pParentEvent);
		trigger_game_event(pGame, &event);
	}
	return R_SUCC;
}

RESULT game_player_add_life(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, int life_inc, int src_player, OutCard* src_cards, int src_skill)
{
	//RESULT ret;
	GameEventContext   event;
	Player* pPlayer;
	ChangeLife   stChgLife;

 	if(!IS_PLAYER_VALID(pGame, player))
 	{
		luaL_error(GL(L), "game_player_add_life: invalid player index - %d", player );
 		return R_E_FAIL;
 	}


	pPlayer = get_game_player(pGame, player);

	if(life_inc + pPlayer->cur_life > pPlayer->max_life)
	{
		life_inc = pPlayer->max_life - pPlayer->cur_life;
		// �������Ҫ����������ֱ�ӷ��سɹ���
		if(life_inc == 0)
			return R_SUCC;
	}


	ST_ZERO(stChgLife);

	stChgLife.delta = life_inc;
	stChgLife.after_life = 0;
	stChgLife.src_player = src_player;
	if(src_cards)
	{
		stChgLife.src_cards = *src_cards;
	}
	stChgLife.src_skill = src_skill;

	// event per change life
	INIT_EVENT(&event, GameEvent_PerChangeLife, player, INVALID_PLAYER, pParentEvent);
	event.change_life = &stChgLife;

	trigger_game_event(pGame, &event);

	// in event some skill can modify the delta, for fix the damage
	if(event.result == R_CANCEL || stChgLife.delta == 0)
		return R_CANCEL;


	INIT_EVENT(&event, GameEvent_ChangeLife, player, INVALID_PLAYER, pParentEvent);
	event.change_life = &stChgLife;



	pPlayer->cur_life += stChgLife.delta;

	if(stChgLife.delta < 0)
		MSG_OUT("��%s��ʧȥ[%d]������, ��ǰ����Ϊ: %d/%d��\n", pPlayer->name, -stChgLife.delta, pPlayer->cur_life, pPlayer->max_life);
	else
		MSG_OUT("��%s���ظ�[%d]������, ��ǰ����Ϊ: %d/%d��\n", pPlayer->name, stChgLife.delta, pPlayer->cur_life, pPlayer->max_life);

// 	if(pPlayer->cur_life > pPlayer->max_life)
// 	{
// 		pPlayer->cur_life = pPlayer->max_life;
// 	}

	stChgLife.after_life = pPlayer->cur_life;

	// ֻ���ڿۼ�������ʱ�����Ҫ�����������ظ�����ʱ�����ǰ����<=0��˵�����ڱ���״̬�����˸�����һ�����
	if(stChgLife.delta <= 0 && pPlayer->cur_life <= 0)
	{
		game_player_dead(L, pGame, &event, player );
	}


	if(!IS_PLAYER_DEAD(pPlayer))
	{
		// ��������Ҳ��ٴ����¼���
		INIT_EVENT(&event, GameEvent_PostChangeLife, player, INVALID_PLAYER, pParentEvent);
		event.change_life = &stChgLife;

		trigger_game_event(pGame, &event);
	}

	return R_SUCC;
}


