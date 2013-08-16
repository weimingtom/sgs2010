#include "config.h"
#include "game.h"
#include "player.h"
#include "card.h"
#include "event.h"
#include "cmd.h"
#include "select.h"
#include "comm.h"




RESULT game_select_target(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player,CardID cid, 
							YESNO self_select, YESNO force,const char* alter_text, int* out_target)
{
	int n;
	int t;
	RESULT  ret;
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
	if(force != YES)
	{
		snprintf(sel_opts[idcnt].text, sizeof(sel_opts[idcnt].text),"Cancel");
		snprintf(sel_opts[idcnt].input, sizeof(sel_opts[idcnt].input), "%s\n%s", "c", "cancel");
		sel_opts[idcnt].value =  -100;
		idcnt++;
	}


	while(1)
	{

		ret = select_loop(pGame, pParentEvent, sel_opts, idcnt, alter_text ? alter_text : "请选择目标角色:", &t);

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

		if(R_SUCC != game_check_attack(pGame, pParentEvent, player, t, cid, 1))
			continue;


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

	set_game_cur_player(pGame,player);


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
int game_select_items(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, const char* items, const char* alter_text)
{
	const int max_items = 32;
	SelOption   opts[max_items];   // 最多支持32项
	int num;
	const char*  p;
	const char*  q;
	int   result;

	if(!IS_PLAYER_VALID(pGame, player))
	{
		if(L) {
			luaL_error(L, "game_passive_out: invalid player index - %d", player );
		} else {
			MSG_OUT("game_passive_out: invalid player index - %d\n", player );
		}
		return R_E_PARAM;
	}


	ST_ZERO(opts);

	num = 0;
	p = items;
	
	while(*p)
	{
		if(num >= max_items)
		{
			luaL_error(L, "game_select_custom: too many items is specified!");
		}

		q = strchr(p, '\n');


		if(q == NULL)
		{
			q = p + strlen(p);
		}

		memcpy(opts[num].text, p, MIN(q-p, (int)sizeof(opts[num].text)-1));
		opts[num].text[MIN(q-p, (int)sizeof(opts[num].text)-1)] = '\0';
		opts[num].input[0] = '\0';
		opts[num].value = num + 1;
		num++;

		p = q;
		if(*p == '\n')
		{
			p++;
		}
	}

	set_game_cur_player(pGame,player);

	if(R_SUCC != select_loop(pGame, pParentEvent, opts, num, alter_text, &result))
	{
		luaL_error(L, "game_select_custom: select_loop return failed!");
	}

	return result;
}


RESULT game_check_attack(GameContext* pGame, GameEventContext* pParentEvent, int player, int target, CardID  cid, int tip)
{
	//RESULT  ret;
	SelectTarget  select_target;
	AttackDis   dis;
	GameEventContext  event;
	Player* pTarget;
	char  temp[128];

	pTarget = get_game_player(pGame, target);


	ST_ZERO(dis);
	dis.card.id = cid;
	dis.base = 1;           // calc the attack range
	dis.inc = 0;            // calc the attack range append increase
	dis.dis = game_player_dis(pGame, player, target);   // calc the player to target's distance
	dis.flag = 0;

	INIT_EVENT(&event, GameEvent_GetBaseAttackDis, player, target, pParentEvent);
	event.attack_dis = &dis;

	call_card_event(cid, pGame, &event, player);


	// need check dis ?  check dist 
	// calc final dist, if base_distancc is -1, means ignore distance to target
	if(dis.base >= 0)
	{

		// calc the skill and equip effect to target distance or attack range
		// target effect
		//trigger_player_event(pGame, &event, t);
		// attacker effect
		//trigger_player_event(pGame, &event, player);

		trigger_game_event(pGame, &event);

		if(dis.base + dis.inc < dis.dis)
		{
			// Attack range less the distance to taget
			if(tip)
			{
				MSG_OUT("选择的角色不在攻击范围内!\n");
			}
			return R_E_DISTANCE;
		}
	}


	ST_ZERO(select_target);

	// can set target?
	INIT_EVENT(&event, GameEvent_SelectTarget, player, target, pParentEvent);

	select_target.card.id = cid;
	event.select_target = &select_target;
	trigger_game_event(pGame, &event);

	if(event.result == R_CANCEL)
	{
		// cannot select this player as target
		if(tip)
		{
			if(strlen(select_target.message) > 0)
			{
				MSG_OUT("%s\n", select_target.message);
			}
			else
			{
				MSG_OUT("【%s】当前不能成为【%s】的目标!\n", pTarget->name, card_name(cid, temp, sizeof(temp)));
			}
		}
		return R_E_TARGET;
	}


	return R_SUCC;
}
