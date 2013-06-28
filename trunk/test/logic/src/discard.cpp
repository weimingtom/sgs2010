#include "config.h"
#include "game.h"
#include "event.h"
#include "player.h"
#include "card.h"
#include "discard.h"
#include "stack.h"
#include "comm.h"


RESULT game_add_discard_cur(GameContext* pGame, const Card* pCard)
{
	if(pGame->nCurDiscardCardNum >= MAX_CUR_DISCARD_NUM)
		return R_E_FAIL;

	pGame->stCurDiscardCards[pGame->nCurDiscardCardNum] = *pCard;
	pGame->nCurDiscardCardNum++;
	
	return R_SUCC;
}

RESULT game_get_discard_cur(GameContext* pGame, int pos, Card* pCard)
{
	if(pos < 0 || pos >= pGame->nCurDiscardCardNum || !CARD_VALID(&pGame->stCurDiscardCards[pos]))
		return R_E_FAIL;

	*pCard = pGame->stCurDiscardCards[pos];
	
	return R_SUCC;
}

RESULT game_clr_discard_cur(GameContext* pGame, int pos)
{
	if(pos < 0 || pos >= pGame->nCurDiscardCardNum)
		return R_E_FAIL;

	ST_ZERO(pGame->stCurDiscardCards[pos]);
	return R_SUCC;
}

void game_flush_discard_cur(GameContext* pGame)
{
	int n;
	for(n = 0; n < pGame->nCurDiscardCardNum; n++)
	{
		if(CARD_VALID(&pGame->stCurDiscardCards[n]))
		{
			card_stack_push(&pGame->stDiscardCardStack, &pGame->stCurDiscardCards[n]);
		}
	}
	pGame->nCurDiscardCardNum = 0;
	ST_ZERO(pGame->stCurDiscardCards);

}

RESULT game_round_discard_card(GameContext* pGame, GameEventContext* pParentEvent, int player)
{
	return R_SUCC;
}

RESULT game_cmd_discard_card(GameContext* pGame, GameEventContext* pParentEvent, int* idx, int num)
{
	return R_E_STATUS;
}



RESULT game_player_discard_card(GameContext* pGame, GameEventContext* pParentEvent, int player, int where, int pos)
{
	PosCard  stCard;
	char buf[128];
	Player* pPlayer = GAME_PLAYER(pGame, player);

	// event: per lost card 

	if(R_SUCC == player_remove_card(pPlayer, where, pos, &stCard.card))
	{
		stCard.card.flag = CardFlag_None;
		stCard.where = where;
		stCard.pos = pos;

		MSG_OUT("[%s] discard a card %s\n", pPlayer->name, card_str(&stCard.card, buf, sizeof(buf)));

		// event: post lost card

		game_add_discard_cur(pGame, &stCard.card);
	}
	return R_SUCC;
}

