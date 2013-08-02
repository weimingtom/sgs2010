#include "card_impl.h"

static YESNO card_spearzhangba_check(GameContext* pGame, GameEventContext* pEvent, int player)
{
	// in out card round , can equip this card
	if(pEvent->id == GameEvent_RoundOutCard)
		return YES;
	return NO;
}


static RESULT card_spearzhangba_out(GameContext* pGame, GameEventContext* pEvent, int player)
{
	if(pEvent->id == GameEvent_OutCard)
	{
		if(pEvent->out_card->list.num == 1 && pEvent->out_card->list.cards[0].id == CardID_SpearZhangBa)
		{
			// equip pos Weapon
			if(R_SUCC == game_player_equip_card(pGame, pEvent, player, EquipIdx_Weapon, &pEvent->out_card->list.cards[0]))
			{
				pEvent->out_card->list.num = 0;
				ST_ZERO(pEvent->out_card->list.cards[0]);
			}
			return R_SUCC;
		}
	}
	return R_SUCC;
}



const CardConfig* get_card_spearzhangba()
{
	static CardConfig card_spearzhangba = {
		CardID_SpearZhangBa,
		CardType_Weapon,
		"spearzhangba",
		"",
		Target_Self,
		card_spearzhangba_check,
		card_spearzhangba_out,
	};

	return &card_spearzhangba;

}

