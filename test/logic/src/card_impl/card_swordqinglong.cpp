#include "card_impl.h"

static YESNO card_swordqinglong_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card round , can equip this card
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
	return NO;
}


static RESULT card_swordqinglong_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	if(pEvent->id == GameEvent_OutCard)
	{
		if(pEvent->pOut->list.num == 1 && pEvent->pOut->list.cards[0].id == CardID_SwordQingLong)
		{
			// equip pos Weapon
			if(R_SUCC == game_player_equip_card(pGame, pEvent, player, EquipIdx_Weapon, &pEvent->pOut->list.cards[0]))
			{
				pEvent->pOut->list.num = 0;
				ST_ZERO(pEvent->pOut->list.cards[0]);
			}
			return R_SUCC;
		}
	}
	return R_SUCC;
}



const CardConfig* get_card_swordqinglong()
{
	static CardConfig card_swordqinglong = {
		CardID_SwordQingLong,
		CardType_Weapon,
		"swordqinglong",
		"",
		Target_Self,
		card_swordqinglong_check,
		card_swordqinglong_out,
	};

	return &card_swordqinglong;

}

