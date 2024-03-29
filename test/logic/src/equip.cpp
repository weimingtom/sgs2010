#include "config.h"
#include "game.h"
#include "event.h"
#include "discard.h"
#include "equip.h"
#include "card.h"
#include "player.h"
#include "script.h"
#include "cmd.h"

//////////////////////////////////////////////////////////////////////////
// script exports




///////////////////////////////////////////////////////////////////////////////


RESULT game_player_equip_card(lua_State* L, GameContext* pGame, GameEventContext* pParentEvent, int player, int hand_pos, int equip_pos)
{
	char buf[128];
	Player* pPlayer;
	EquipCard  equip_cards;
	VCard   vcard;
	GameEventContext  event;

	if(!IS_PLAYER_VALID(pGame, player))
	{
		luaL_error(GL(L), "game_player_equip_card: invalid player index - %d", player );
		return R_E_PARAM;
	}

	if(equip_pos < 0 || equip_pos >= EquipIdx_Max)
	{
		luaL_error(GL(L), "game_player_equip_card: invalid equip pos - %d", equip_pos );
		return R_E_PARAM;
	}


	pPlayer = get_game_player(pGame, player);

	ST_ZERO(equip_cards);


	equip_cards.supply = player;
	equip_cards.equip_pos = equip_pos;

	// get card from hand
	if(R_SUCC != get_player_card(pPlayer, CardWhere_PlayerHand, hand_pos, &vcard) || !VCARD_IS_REAL(&vcard))
	{
		luaL_error(GL(L), "game_player_equip_card: invalid hand card pos - %d", hand_pos );
		return R_E_PARAM;
	}
	
	equip_cards.card.card = vcard.vcard;
	equip_cards.card.where = CardWhere_PlayerHand;
	equip_cards.card.pos = hand_pos;


	// add event per equip card

	INIT_EVENT(&event, GameEvent_PerEquipCard, player, INVALID_PLAYER, pParentEvent);
	event.equip_card = &equip_cards;

	trigger_game_event(pGame, &event);

	// some skill can cancel equip a card? 
	RET_CHECK_CANCEL_RET(event.result, R_CANCEL);

	if(R_SUCC == player_remove_card(pPlayer, CardWhere_PlayerHand, hand_pos, NULL))
	{

		if(CARD_VALID(&pPlayer->equip_cards[equip_pos]))
		{
			game_player_discard_card(pGame, pParentEvent, player, CardWhere_PlayerEquip, equip_pos);
		}

		MSG_OUT("��%s��װ��[%s]�� %s\n", pPlayer->name, equip_idx_str(equip_pos), card_str(&equip_cards.card.card, buf, sizeof(buf)));

		pPlayer->equip_cards[equip_pos] = equip_cards.card.card;
		pPlayer->equip_cards[equip_pos].flag = CardFlag_None;


		equip_cards.card.where = CardWhere_PlayerEquip;
		equip_cards.card.pos = equip_pos;


		// add event post equip card

		INIT_EVENT(&event, GameEvent_PostEquipCard, player, INVALID_PLAYER, pParentEvent);
		event.equip_card = &equip_cards;

		trigger_game_event(pGame, &event);

		// ignore event result
	}

	return R_SUCC;
}


