#include "config.h"
#include "game.h"
#include "player.h"
#include "card.h"
#include "event.h"
#include "cmd.h"
#include "select.h"
#include "comm.h"




RESULT game_select_target(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, int base_dist, YESNO self_select, YESNO may_cancel,const char* alter_text, int* out_target)
{
	int n;
	int t;
	RESULT  ret;
	AttackDis   dis;
	GameEventContext  event;
	Player* pPlayer; 
	Player* pTarget;
	int idcnt;
	int idbegin ;
	SelOption   sel_opts[MAX_PLAYER_NUM + 1];

	ST_ZERO(sel_opts);

	set_game_cur_player(pGame,player );


	pPlayer = get_game_player(pGame, player);
	(void)pPlayer;

	idbegin = self_select == YES ? 0 : 1;
	idcnt = 0;

	for(n = idbegin; n < pGame->player_count; n++)
	{
		t = (player + n) % pGame->player_count;
		pTarget = get_game_player(pGame, t);
		if(!IS_PLAYER_DEAD(pTarget))
		{
			snprintf(sel_opts[idcnt].text, sizeof(sel_opts[idcnt].text), "%s, %s, life: %d/%d%s, hand cards: %d", 
				player_id_str( (t == player || IS_PLAYER_SHOW(pTarget) || IS_PLAYER_DEAD(pTarget)) ? pTarget->id : PlayerID_Unknown),
				pTarget->name, pTarget->cur_life, pTarget->max_life, IS_PLAYER_DEAD(pTarget)?"(Dead)":"", pTarget->hand_card_num);

			snprintf(sel_opts[idcnt].input, sizeof(sel_opts[idcnt].input), "%d", idbegin + idcnt);
			sel_opts[idcnt].value = t;
			idcnt++;
		}
	}

	// 
	if(may_cancel == YES)
	{
		snprintf(sel_opts[idcnt].text, sizeof(sel_opts[idcnt].text),"Cancel");
		snprintf(sel_opts[idcnt].input, sizeof(sel_opts[idcnt].input), "%s\n%s", "c", "cancel");
		sel_opts[idcnt].value =  -100;
		idcnt++;
	}


	while(1)
	{

		ret = select_loop(pGame, pParentEvent, sel_opts, idcnt, alter_text ? alter_text : "Select a target role:", &t);

		if(ret != R_SUCC)
			return ret;

		if(t == -100)
			return R_CANCEL;


		pTarget = get_game_player(pGame, t);

		if(pTarget == NULL)
		{
			MSG_OUT("error select target return : %d\n", t);
			return R_E_FAIL;
		}


		// can set target?
		INIT_EVENT(&event, GameEvent_SelectTarget, player, t, pParentEvent);

		trigger_game_event(pGame, &event);

		if(event.result == R_CANCEL)
		{
			// cannot select this player as target
			MSG_OUT("你选择的角色不能成为目标, 请重新选择!\n");
			continue;
		}

		// need check dis ?  check dist 
		if(base_dist >= 0)
		{
			// calc final dist, if base_distancc is -1, means ignore distance to target
			dis.base = base_dist;   // calc the attack range
			dis.inc = 0;            // calc the attack range append increase
			dis.dis = game_player_dis(pGame, player, t);   // calc the player to target's distance
			dis.flag = 0;

			INIT_EVENT(&event, GameEvent_CalcAttackDis, player, t, pParentEvent);
			event.attack_dis = &dis;

			// calc the skill and equip effect to target distance or attack range
			// target effect
			//trigger_player_event(pGame, &event, t);
			// attacker effect
			//trigger_player_event(pGame, &event, player);

			trigger_game_event(pGame, &event);

			if(dis.base + dis.inc < dis.dis)
			{
				// Attack range less the distance to taget
				MSG_OUT("你选择的角色不在你的攻击范围内, 请重新选择!\n");
				continue;
			}
		}
		*out_target = t;
		return R_SUCC;
	}

	return R_E_FAIL;
}

YESNO game_select_yesno(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, const char* alter_text)
{
	int result;
	SelOption   sel_opts[2];
	ST_ZERO(sel_opts);

	set_game_cur_player(pGame,player );


	snprintf(sel_opts[0].text, sizeof(sel_opts[0].text),"[ Yes ]");
	snprintf(sel_opts[0].input, sizeof(sel_opts[0].input), "y\nyes");
	sel_opts[0].value =  YES;

	snprintf(sel_opts[1].text, sizeof(sel_opts[0].text),"[ No  ]");
	snprintf(sel_opts[1].input, sizeof(sel_opts[0].input), "n\nno");
	sel_opts[1].value =  NO;

	if(select_loop(pGame, pParentEvent, sel_opts, 2, alter_text, &result) == R_SUCC)
		return (YESNO) result;

	return NO;
}

// 多项选择(选项串为多行文本，每行一个选项。 返回 1~n 行号。)(如果有放弃的需要，把这个选项也要通过items给出)
int game_select_custom(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, const char* items)
{

	return 0;
}
