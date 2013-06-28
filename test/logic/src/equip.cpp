#include "config.h"
#include "game.h"
#include "event.h"
#include "discard.h"
#include "equip.h"
#include "card.h"
#include "player.h"
#include "comm.h"



RESULT game_player_equip_card(GameContext* pGame, GameEventContext* pParentEvent, int player, int pos, Card* pCard)
{
	char buf[128];
	Player* pPlayer = GAME_PLAYER(pGame, player);

	// add event per equip card

	if(CARD_VALID(&pPlayer->stEquipCard[pos]))
	{
		game_player_discard_card(pGame, pParentEvent, player, PlayerCard_Equip, pos);
	}


	MSG_OUT("[%s] equip a [%s] card %s\n", pPlayer->name, equip_idx_str(pos), card_str(pCard, buf, sizeof(buf)));

	pPlayer->stEquipCard[pos] = *pCard;
	pPlayer->stEquipCard[pos].flag = CardFlag_None;

	// add event post equip card

	return R_SUCC;
}


