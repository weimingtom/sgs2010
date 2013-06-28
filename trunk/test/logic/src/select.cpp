#include "config.h"
#include "game.h"
#include "player.h"
#include "card.h"
#include "event.h"
#include "cmd.h"
#include "select.h"




RESULT game_select_target(GameContext* pGame, GameEventContext* pParentEvent, int player, int base_dist, YESNO self_select, YESNO may_cancel,const char* alter_text, int* out_target)
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


	pPlayer = GAME_PLAYER(pGame, player);
	(void)pPlayer;

	idbegin = self_select == YES ? 0 : 1;
	idcnt = 0;

	for(n = idbegin; n < pGame->nPlayerCount; n++)
	{
		t = (player + n) % pGame->nPlayerCount;
		pTarget = GAME_PLAYER(pGame, t);
		if(!IS_PLAYER_DEAD(pTarget))
		{
			snprintf(sel_opts[idcnt].text, sizeof(sel_opts[idcnt].text), "%s, %s, life: %d/%d%s, hand cards: %d", 
				player_id_str( (t == player || IS_PLAYER_SHOW(pTarget) || IS_PLAYER_DEAD(pTarget)) ? pTarget->id : PlayerID_Unknown),
				pTarget->name, pTarget->curLife, pTarget->maxLife, IS_PLAYER_DEAD(pTarget)?"(Dead)":"", pTarget->nHandCardNum);

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

		if(idcnt == -100)
			return R_CANCEL;


		pTarget = GAME_PLAYER(pGame, t);


		// can set target?
		INIT_EVENT(&event, GameEvent_SelectTarget, player, t, pParentEvent);

		trigger_game_event(pGame, &event);

		if(event.result == R_CANCEL)
		{
			// cannot select this player as target
			printf("The selected role cannot be as a target, please select another!\n");
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
			event.pAttackDis = &dis;

			// calc the skill and equip effect to target distance or attack range
			// target effect
			//trigger_player_event(pGame, &event, t);
			// attacker effect
			//trigger_player_event(pGame, &event, player);

			trigger_game_event(pGame, &event);

			if(dis.base + dis.inc < dis.dis)
			{
				// Attack range less the distance to taget
				printf("The selected role is out of your attack range, please select another!\n");
				continue;
			}
		}
		*out_target = t;
		return R_SUCC;
	}

	return R_E_FAIL;
}

YESNO game_select_yesno(GameContext* pGame, GameEventContext* pParentEvent, int player, const char* alter_text)
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

