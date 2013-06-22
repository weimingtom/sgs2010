// out.cpp 
#include "config.h"
#include "game.h"
#include "event.h"
#include "cmd.h"
#include "comm.h"
#include "out.h"
#include "card.h"
//#include "hero.h"
#include "player.h"




RESULT game_outcard(GameContext* pGame, GameEventContext* pEvent,  int* idx, int num)
{
	char buffer[128];
	if(game_status(pGame) != Status_Round_Out)
		return R_E_STATUS;

	int where;
	int pos;

	if(0 != player_card_idx_to_pos(CUR_PLAYER(pGame), *idx, &where, &pos))
	{
		printf("input card idx error!\n");
		return R_E_PARAM;
	}

	if(where != PlayerCard_Hand)
	{
		printf("only can out hand card!\n");
		return R_E_PARAM;
	}

	// check can out?
	Card stCard = CUR_PLAYER(pGame)->stHandCards[pos];

	const CardConfig* pCardConfig = get_card_config(stCard.id);

	if(pCardConfig == NULL || pCardConfig->check == NULL || YES != (*pCardConfig->check)(pGame, NULL, pGame->nCurPlayer))
	{
		printf("can not out this card: %s!\n", card_str(&stCard, buffer, sizeof(buffer)));
		return R_E_PARAM;
	}


	return (*pCardConfig->out)(pGame, NULL, pGame->nCurPlayer);

	return R_SUCC;
}



