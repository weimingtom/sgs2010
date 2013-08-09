#include "config.h"
#include "game.h"
#include "judge.h"
#include "event.h"
#include "discard.h"
#include "comm.h"


static RESULT game_judge_card(GameContext* pGame, GameEventContext* pEvent, int player)
{
	RESULT  ret;
	Player* pPlayer;
	VCard    vcard;
	PosVCard stCard;
	GameEventContext  event;
	char buf[128];
	int pos;
	int n;

	pPlayer = get_game_player(pGame, player);


	pos = pPlayer->card_in_judge_num - 1;
	//vcard = pPlayer->judgment_cards[pPlayer->judgment_card_num-1];
	if(R_SUCC != get_player_card(pPlayer, CardWhere_PlayerJudgment,  pos, &vcard))
	{
		pPlayer->card_in_judge_num-- ;
		return R_SKIP; // next judgment card
	}

	MSG_OUT("【%s】开始结算判定区牌 %s\n", pPlayer->name, vcard_str(&vcard, buf, sizeof(buf)));


	stCard.vcard = vcard.vcard;
	stCard.list.num = vcard.rnum;
	for(n = 0; n < vcard.rnum; n++)
	{
		stCard.list.pcards[n].card = vcard.rcards[n];
		stCard.list.pcards[n].where = CardWhere_PlayerJudgment;
		stCard.list.pcards[n].pos = pos;
	}

	// 先放入当前弃牌堆
	pPlayer->card_in_judge_num--;

	// remove from judgment list
	player_remove_card(pPlayer, CardWhere_PlayerJudgment, pos, NULL);

	// discard card 
	for(n = 0; n < stCard.list.num; n++)
	{
		ret = game_add_discard_cur(pGame, &stCard.list.pcards[n].card, &stCard.list.pcards[n].pos);
		stCard.list.pcards[n].where = CardWhere_CurDiscardStack;
		// CHECK_RET(ret,ret); // must be succes
	}

	// calc

	if(1)
	{
		INIT_EVENT(&event, GameEvent_PerCardCalc, pGame->round_player, INVALID_PLAYER, pEvent);
		event.pos_vcard = &stCard;
		trigger_game_event(pGame, &event);

		if(event.result != R_CANCEL) // if card calc is cancel .
		{
			INIT_EVENT(&event, GameEvent_CardCalc, pGame->round_player, INVALID_PLAYER, pEvent);
			event.pos_vcard = &stCard;
			//(*pCardConfig->out)(pGame, &event, pGame->cur_player);
			call_card_event(stCard.vcard.id, pGame, &event, pGame->round_player);

			// post calc , before discard
			INIT_EVENT(&event, GameEvent_PostCardCalc, pGame->round_player, INVALID_PLAYER, pEvent);
			event.pos_vcard = &stCard;
			trigger_game_event(pGame, &event);

		}


		// after calc

		INIT_EVENT(&event, GameEvent_FiniCardCalc, pGame->round_player, INVALID_PLAYER, pEvent);
		event.pos_vcard = &stCard;

		call_card_event(stCard.vcard.id, pGame, &event, pGame->cur_player);

	}

	return R_SUCC;
}

RESULT game_round_do_judge(GameContext* pGame, GameEventContext* pEvent, int player)
{
	RESULT  ret;
	Player* pPlayer;
	GameEventContext  event;

	// judge cards
	pPlayer = get_game_player(pGame, player);

	//const CardConfig* pCardConfig;


	pPlayer->card_in_judge_num = pPlayer->judgment_card_num;

	INIT_EVENT(&event, GameEvent_RoundJudgeCard, player, INVALID_PLAYER, pEvent);


	while(pPlayer->card_in_judge_num > 0)
	{
		ret = game_judge_card(pGame, &event, player);
		(void) ret; // not yet used
		game_flush_discard_cur(pGame);
	}
	return R_SUCC;

}


